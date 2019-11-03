#ifndef SEARCH_BAR_HPP
#define SEARCH_BAR_HPP


#include <QWidget>
#include <QLineEdit>
#include <QLabel>
#include <QHBoxLayout>
#include <QRegularExpression>


class search_bar : public QWidget{
    Q_OBJECT

signals:
    void changed_valid_regex(const QRegularExpression& );

private slots:
    void on_changed();

public:
    QLineEdit* edit{new QLineEdit{this}};

    explicit search_bar(QWidget* parent) : QWidget{parent}{
        setup_layout();
        setup_connections();
    }

private:
    void setup_layout(){
        auto* box = new QHBoxLayout{};
        box->addWidget(new QLabel{"Search:", this});
        box->addWidget(edit);
        setLayout(box);
    }

    void setup_connections();
};


#endif
