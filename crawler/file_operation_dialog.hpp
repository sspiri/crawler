#ifndef QT_COLLECTION_FILE_OPERATION_DIALOG_HPP
#define QT_COLLECTION_FILE_OPERATION_DIALOG_HPP


#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>


class file_operation_dialog : public QDialog{
public:
    enum class response_code : char{
        none, skip, skip_all, cancel
    };

    QLabel* message;
    QPushButton* skip{new QPushButton{"Skip", this}},
               * skip_all{new QPushButton{"Skip all", this}},
               * cancel{new QPushButton{"Cancel operation", this}};

    file_operation_dialog(const QString& msg, QWidget* parent = Q_NULLPTR)
        : QDialog{parent}, message{new QLabel{msg, this}}{

        message->setEnabled(false);
        message->setFixedWidth(800);

        set_layout();
        set_connections();
    }

    response_code get_response_code() const{
        return code;
    }

private slots:
    void button_clicked(response_code rc){
        code = rc;
        close();
    }

private:
    response_code code{response_code::none};

    void set_layout(){
       auto* layout = new QGridLayout;

       layout->addWidget(message, 0, 0);

       auto* box = new QHBoxLayout;
       box->addStretch();
       box->addWidget(skip);
       box->addWidget(skip_all);
       box->addWidget(cancel);

       layout->addLayout(box, 1, 0);

       setLayout(layout);
    }

    void set_connections(){
        connect(skip, &QPushButton::clicked, std::bind(&file_operation_dialog::button_clicked, this, response_code::skip));
        connect(skip_all, &QPushButton::clicked, std::bind(&file_operation_dialog::button_clicked, this, response_code::skip_all));
        connect(cancel, &QPushButton::clicked, std::bind(&file_operation_dialog::button_clicked, this, response_code::cancel));
    }
};


#endif
