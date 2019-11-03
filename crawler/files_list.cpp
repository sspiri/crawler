#include <QMessageBox>
#include <QFileDialog>
#include <QDesktopServices>
#include <QDateTime>
#include <QShortcut>
#include <QUrl>
#include <QMenu>
#include <QProcess>
#include <QApplication>

#include <functional>
#include <fstream>


#include "filesystem.hpp"
#include "string_dialog.hpp"
#include "directory_iterator.hpp"
#include "file_operation.hpp"
#include "file_operation_progress.hpp"
#include "utility.hpp"
#include "files_list.hpp"


files_list::files_list(const QString& current, QWidget* parent)
    : QTableWidget{parent},
      dir_it{new directory_iterator{this}}{

    setSelectionBehavior(QTableWidget::SelectRows);
    setContextMenuPolicy(Qt::CustomContextMenu);

    setup_connections();
    enter_directory(current);

    verticalHeader()->setVisible(false);
    horizontalHeader()->setVisible(true);
    horizontalHeader()->setSectionsMovable(true);
    setHorizontalHeaderLabels({"File name"});

    setShowGrid(false);

    timer->setSingleShot(false);
    timer->start(1000);
}


files_list::~files_list(){
    if(dir_it->isRunning()){
        dir_it->cancel();

        do{
            QApplication::processEvents();
        } while(!dir_it->wait(0));
    }
}


void files_list::setup_connections(){
    connect(this, &QTableWidget::cellDoubleClicked, this, &files_list::row_double_clicked);
    connect(this, &QTableWidget::customContextMenuRequested, this, &files_list::show_context_menu);

    connect(dir_it, &directory_iterator::row_count, this, &QTableWidget::setRowCount, Qt::BlockingQueuedConnection);
    connect(dir_it, &directory_iterator::setup_columns, this, &files_list::setup_columns, Qt::BlockingQueuedConnection);

    connect(dir_it, &directory_iterator::new_item, this, &QTableWidget::setItem, Qt::BlockingQueuedConnection);
    connect(dir_it, &directory_iterator::resize_columns, this, &QTableWidget::resizeColumnsToContents, Qt::BlockingQueuedConnection);

    connect(timer, &QTimer::timeout, [this]{
        if(!dir_it->isRunning())
            setup_files_list();
    });

    connect(new QShortcut{Qt::Key_Delete, this}, &QShortcut::activated, this, &files_list::remove);
    connect(new QShortcut{Qt::CTRL +  Qt::Key_C, this}, &QShortcut::activated, this, &files_list::on_copy);
    connect(new QShortcut{Qt::CTRL +  Qt::Key_X, this}, &QShortcut::activated, this, &files_list::on_cut);
    connect(new QShortcut{Qt::CTRL +  Qt::Key_V, this}, &QShortcut::activated, this, &files_list::paste);
}


void files_list::show_context_menu(const QPoint& point){
    QMenu* menu = new QMenu{this};
    auto files = get_selected_files();

    if(!files.size()){
        connect(menu->addAction("Create file..."), &QAction::triggered, this, &files_list::create_file);
        connect(menu->addAction("Create directory..."), &QAction::triggered, this, &files_list::create_directory);

        if(clipboard_operation == "COPY" || clipboard_operation == "CUT"){
            menu->addSeparator();
            connect(menu->addAction("Paste"), &QAction::triggered, this, &files_list::paste);
        }

        menu->addSeparator();
        connect(menu->addAction("Select all"), &QAction::triggered, this, &QTableWidget::selectAll);

        connect(menu->addAction("Open Terminal"), &QAction::triggered,
                    std::bind((bool(*)(const QString&, const QStringList&))&QProcess::startDetached, "gnome-terminal", QStringList{"--working-directory=" + current_dir.absolutePath()}));
    }

    else if(files.size() == 1){
        connect(menu->addAction("Create symlink..."), &QAction::triggered, this, &files_list::create_symlinks);
        connect(menu->addAction("Rename..."), &QAction::triggered, this, &files_list::rename);

        menu->addSeparator();

        connect(menu->addAction("Copy"), &QAction::triggered, this, &files_list::on_copy);
        connect(menu->addAction("Cut"), &QAction::triggered, this, &files_list::on_cut);
        connect(menu->addAction("Remove"), &QAction::triggered, this, &files_list::remove);

        menu->addSeparator();

        if(QFileInfo{files[0]}.isDir())
            connect(menu->addAction("Go to directory"), &QAction::triggered, std::bind(&files_list::enter, this, files[0]));

        connect(menu->addAction("Open file..."), &QAction::triggered, std::bind(&files_list::open_file, this, QStringList{files[0]}));
    }

    else if(files.size() > 1){
        connect(menu->addAction("Create symlinks..."), &QAction::triggered, this, &files_list::create_symlinks);

        menu->addSeparator();
        connect(menu->addAction("Copy"), &QAction::triggered, this, &files_list::on_copy);
        connect(menu->addAction("Cut"), &QAction::triggered, this, &files_list::on_cut);
        connect(menu->addAction("Remove"), &QAction::triggered, this, &files_list::remove);

        menu->addSeparator();
        connect(menu->addAction("Open files..."), &QAction::triggered, std::bind(&files_list::open_file, this, files));
    }

    menu->exec(mapToGlobal(point));
    menu->deleteLater();
}


void files_list::rename(){
    auto list = get_selected_files();

    if(list.size() == 1){
        auto* dialog = new string_dialog("Rename", "Rename [" + list[0] + "]:", this);
        dialog->edit->setText(QFileInfo{list[0]}.fileName());

        if(dialog->exec() == QDialog::Accepted){
            fs::rename(list[0].toStdString(), fs::path{list[0].toStdString()}.parent_path() / dialog->edit->text().toStdString());
            setup_files_list();
        }

        dialog->deleteLater();
    }
}


void files_list::create_symlinks(){
    auto list = get_selected_files();

    if(list.size()){
        auto dirname = QFileDialog::getExistingDirectory(this, "Select directory", current_dir.absolutePath());

        if(dirname.size()){
            enter(dirname);

            auto* operation = new symlink_operation{list, current_dir.absolutePath(), this};
            auto* progress = new file_operation_progress{operation, list.size()};

            progress->setAttribute(Qt::WA_DeleteOnClose);
            connect(operation, &QThread::finished, operation, &QObject::deleteLater);

            progress->show();
        }
    }
}


void files_list::open_file(const QStringList& arguments){
    QString file = QFileDialog::getOpenFileName(this, "Select executable", QDir::rootPath());

    if(file.size())
        QProcess::startDetached(file, arguments);
}


void files_list::create_file(){
    auto* dialog = new string_dialog{"Create file", "File name:", this};

    if(dialog->exec() == QDialog::Accepted){
        std::ofstream{current_dir.absoluteFilePath(dialog->edit->text()).toStdString()};
        setup_files_list();
    }

    dialog->deleteLater();
}


void files_list::create_directory(){
    auto* dialog = new string_dialog{"Create directory", "Directory name:", this};

    if(dialog->exec() == QDialog::Accepted){
        current_dir.mkdir(dialog->edit->text());
        setup_files_list();
    }

    dialog->deleteLater();
}


void files_list::row_double_clicked(int row, int ){
    enter(current_dir.filePath(item(row, 0)->text()));
}

void files_list::enter_directory(const QString& path){
    auto prev = current_dir;
    current_dir.setPath(path);

    if(!current_dir.isReadable()){
        QMessageBox::critical(this, "Permission denied", "Cannot read directory");
        current_dir = std::move(prev);
        setup_files_list();
    }

    else{
        setup_files_list();
        emit new_path(path);
    }
}

void files_list::enter(const QString& path){
    QFileInfo info{path};

    if(info.isDir())
        enter_directory(info.absoluteFilePath());

    else
        QDesktopServices::openUrl(QUrl{info.absoluteFilePath()});

}

void files_list::go_back(){
    if(current_dir != QDir::root() && !current_dir.cdUp())
        QMessageBox::critical(this, "Permission denied", "Cannot read directory");

    else{
        setup_files_list();
        emit new_path(current_dir.absolutePath());
    }
}


void files_list::setup_columns(){
    QStringList header_names{"Name"};
    int count = 1;

    if(headers & headers_mask::modified){
        ++count;
        header_names.push_back("Modified");
    }

    if(headers & headers_mask::size){
        ++count;
        header_names.push_back("Size");
    }

    if(headers & headers_mask::type){
        ++count;
        header_names.push_back("Type");
    }

    if(headers & headers_mask::path){
        ++count;
        header_names.push_back("Path");
    }

    setColumnCount(count);
    setHorizontalHeaderLabels(header_names);
}


void files_list::hide_items(const QRegularExpression& regex){
    for(int row=0; row < rowCount(); ++row){
        if(regex.match(item(row, 0)->text()).hasMatch())
            showRow(row);
        else
            hideRow(row);
    }
}


void files_list::on_copy(){
    clipboard_operation = "COPY";
    clipboard->setProperty(clipboard_operation.c_str(), get_selected_files());
}


void files_list::on_cut(){
    clipboard_operation = "CUT";
    clipboard->setProperty(clipboard_operation.c_str(), get_selected_files());
}


void files_list::copy(){
    auto list = clipboard->property(clipboard_operation.c_str()).toStringList();

    auto* operation = new copy_operation{list, current_dir.absolutePath(), this};
    auto* progress = new file_operation_progress{operation, operation->get_nfiles()};

    progress->setAttribute(Qt::WA_DeleteOnClose);
    connect(operation, &QThread::finished, operation, &QObject::deleteLater);

    progress->show();
}


void files_list::cut(){
    auto list = clipboard->property(clipboard_operation.c_str()).toStringList();

    auto* operation = new move_operation{list, current_dir.absolutePath(), this};
    auto* progress = new file_operation_progress{operation, list.size()};

    progress->setAttribute(Qt::WA_DeleteOnClose);
    connect(operation, &QThread::finished, operation, &QObject::deleteLater);

    progress->show();
}


void files_list::remove(){
    auto list = get_selected_files();

    if(!list.size())
        return;

    int ret = list.size() > 1
                ? QMessageBox::warning(this, "Remove files", "Do you want to remove " + QString::number(list.size()) + " files?", QMessageBox::Yes | QMessageBox::No)
                : QMessageBox::Yes;

    if(ret == QMessageBox::Yes){
        auto* operation = new remove_operation{list, this};
        auto* progress = new file_operation_progress{operation, list.size()};

        progress->setAttribute(Qt::WA_DeleteOnClose);
        connect(operation, &QThread::finished, operation, &QObject::deleteLater);

        progress->show();
    }
}


void files_list::paste(){
    if(clipboard_operation == "COPY")
        copy();

    else if(clipboard_operation == "CUT"){
        cut();
        clipboard_operation.clear();
    }
}


QStringList files_list::get_selected_files() const{
    auto items = selectedItems();

    QStringList list;
    list.reserve(items.size());

    for(const auto* item : items){
        if(!column(item))
            list.push_back(current_dir.absoluteFilePath(item->text()));
    }

    return list;
}


void files_list::setup_files_list(){
    if(dir_it->isRunning()){
        dir_it->cancel();

        do{
            QApplication::processEvents();
        } while(!dir_it->wait(0));
    }

    dir_it->start();
}
