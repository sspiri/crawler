#include <QMenuBar>
#include <QShortcut>

#include "settings_t.hpp"
#include "settings_dialog.hpp"
#include "main_window.hpp"


main_window::main_window(const QString& current)
    : path{new path_widget{current, this}},
      files{new files_list{current, this}}{

    resize(1200, 800);
    setCentralWidget(central);
    path->edit->setText(current);

    splitter->addWidget(mounts);
    splitter->addWidget(files);
    splitter->setOrientation(Qt::Horizontal);
    splitter->setStretchFactor(1, 2);

    setup_menu_bar();
    setup_connections();
    setup_layout();
}


void main_window::setup_menu_bar(){
    QMenu* menu = menuBar()->addMenu("File");

    connect(menu->addAction("Settings..."), &QAction::triggered, this, &main_window::open_settings);
    menu->addSeparator();
    connect(menu->addAction("Quit"), &QAction::triggered, this, &QMainWindow::close);
}


void main_window::setup_connections(){
    connect(files, &files_list::new_path, path->edit, &QLineEdit::setText);
    connect(path->back, &QPushButton::clicked, files, &files_list::go_back);

    connect(search, &search_bar::changed_valid_regex, files, &files_list::hide_items);

    connect(path->edit, &QLineEdit::returnPressed, this, &main_window::enter_if_exists);
    connect(path->go, &QPushButton::clicked, this, &main_window::enter_if_exists);

    connect(mounts->list, &QListWidget::itemDoubleClicked, [this](QListWidgetItem* item){
        files->enter_directory(item->text());
    });

    connect(new QShortcut{Qt::Key_Backspace, this}, &QShortcut::activated, files, &files_list::go_back);
}


void main_window::setup_layout(){
    auto* grid = new QGridLayout;

    grid->addWidget(search, 0, 0);

    grid->addWidget(splitter, 1, 0);
    grid->setRowStretch(1, 1);

    grid->addWidget(path, 2, 0);

    central->setLayout(grid);
}


void main_window::open_settings(){
    auto* dialog = new settings_dialog{files->settings, this};

    if(dialog->exec() == QDialog::Accepted){
        dialog->set_settings();
        files->setup_files_list(true);
    }

    dialog->deleteLater();
}
