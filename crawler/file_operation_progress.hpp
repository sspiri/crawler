#ifndef QT_COLLECTION_FILE_OPERATION_PROGRESS_HPP
#define QT_COLLECTION_FILE_OPERATION_PROGRESS_HPP


#include <map>

#include <QApplication>
#include <QWidget>
#include <QProgressBar>
#include <QPushButton>
#include <QLabel>
#include <QGridLayout>
#include <QCommonStyle>

#include "file_operation.hpp"


class file_operation_progress : public QWidget{
public:
    file_operation* operation;
    std::ptrdiff_t nfiles;

    QLabel* message{new QLabel{this}};
    QProgressBar* progress{new QProgressBar{this}};
    QPushButton* cancel{new QPushButton{this}},
               * pause{new QPushButton{this}};

    file_operation_progress(file_operation* op, std::ptrdiff_t nitems, QWidget* parent = Q_NULLPTR)
        : QWidget{parent}, operation{op}, nfiles{nitems}{

        setWindowTitle(op->operation() + QString{"..."});
        setFixedSize(600, minimumHeight());

        cancel->setIcon(style.standardIcon(QStyle::SP_MediaStop));
        pause->setIcon(style.standardIcon(QStyle::SP_MediaPause));

        set_layout();
        set_connections();

        progress->setRange(0, 100);
        operation->start();
    }

    ~file_operation_progress(){
        if(operation && operation->isRunning()){
            operation->cancel();

            do{
                QApplication::processEvents();
            } while(!operation->wait(0));
        }
    }

private slots:
    void evaluate_process(const QString& path){
        message->setText((operation->operation() + (' ' + path.toStdString())).c_str());
        progress->setValue((int)(100. / nfiles * ++nth));
    }

    void toggle_pause(){
        operation->toggle_pause();
        pause->setIcon(style.standardIcon(operation->is_paused() ? QStyle::SP_MediaPlay : QStyle::SP_MediaPause));
    }

    void handle_error(const std::filesystem::filesystem_error& err){
        auto it = errors.find(err.code().value());

        if(it == errors.end() || !it->second){
            auto* dialog = new file_operation_dialog{err.what(), this};

            dialog->exec();

            if(dialog->get_response_code() == file_operation_dialog::response_code::cancel)
                operation->cancel();

            errors[err.code().value()] = dialog->get_response_code() == file_operation_dialog::response_code::skip_all;

            dialog->deleteLater();
        }
    }

private:
    std::map<int, bool> errors;
    QCommonStyle style;
    std::ptrdiff_t nth{};

    void set_layout(){
        auto* layout = new QGridLayout;

        layout->addWidget(message, 0, 0);
        layout->addWidget(progress, 1, 0);

        auto* box = new QHBoxLayout;
        box->addStretch();
        box->addWidget(cancel);
        box->addWidget(pause);

        layout->addLayout(box, 2, 0);

        setLayout(layout);
    }

    void set_connections(){
        connect(operation, &file_operation::error, this, &file_operation_progress::handle_error, Qt::BlockingQueuedConnection);
        connect(operation, &file_operation::process, this, &file_operation_progress::evaluate_process, Qt::BlockingQueuedConnection);
        connect(operation, &QThread::finished, this, &QWidget::close);

        connect(pause, &QPushButton::clicked, this, &file_operation_progress::toggle_pause);
        connect(cancel, &QPushButton::clicked, operation, &file_operation::cancel);

    }
};

#endif
