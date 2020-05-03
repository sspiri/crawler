#ifndef SEARCH_BAR_HPP
#define SEARCH_BAR_HPP


#include <QWidget>
#include <QLineEdit>
#include <QLabel>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QRegularExpression>


class search_bar : public QWidget{
    Q_OBJECT

signals:
    void changed_valid_regex(const QRegularExpression& , bool );

private slots:
    void on_changed();

public:
    QLineEdit* edit{new QLineEdit{this}};
    QCheckBox* recursive{new QCheckBox{"Recursive", this}};

    explicit search_bar(QWidget* parent) : QWidget{parent}{
        setup_layout();
        setup_connections();
        recursive->setChecked(false);
    }

private:
    void setup_layout(){
        auto* box = new QHBoxLayout{};
        box->addWidget(new QLabel{"Search:", this});
        box->addWidget(edit);
        box->addWidget(recursive);
        setLayout(box);
    }

    void setup_connections();
};


#endif
