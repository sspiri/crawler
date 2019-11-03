#ifndef STRING_DIALOG_HPP
#define STRING_DIALOG_HPP


#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QGridLayout>


class string_dialog : public QDialog{
public:
    QLineEdit* edit{new QLineEdit{this}};
    QPushButton* ok{new QPushButton{"Ok", this}},
               * cancel{new QPushButton{"Cancel", this}};

    string_dialog(const QString& title, const QString& label, QWidget* parent) : QDialog{parent} {
        edit->setMinimumWidth(500);

        setup_layout(title, label);
        setup_connections();
    }

private:
    void setup_layout(const QString& title, const QString& label){
        setWindowTitle(title);

        auto* grid = new QGridLayout{};

        grid->addWidget(new QLabel{label, this}, 0, 0);
        grid->addWidget(edit, 0, 1);

        auto* box = new QHBoxLayout{};

        box->addStretch();
        box->addWidget(ok);
        box->addWidget(cancel);

        grid->addLayout(box, 1, 0, 1, 2);

        setLayout(grid);
    }

    void setup_connections(){
        connect(ok, &QPushButton::clicked, this, &QDialog::accept);
        connect(cancel, &QPushButton::clicked, this, &QDialog::reject);
    }
};


#endif
