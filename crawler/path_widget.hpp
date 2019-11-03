#ifndef PATH_WIDGET_HPP
#define PATH_WIDGET_HPP


#include <QPushButton>
#include <QLineEdit>
#include <QTimer>


class path_widget : public QWidget{
private slots:
    void validate_line();
    void add_to_history(const QString& current);

    void go_up();
    void go_down();

public:
    QPushButton* back{new QPushButton{this}};
    QLineEdit* edit{new QLineEdit{this}};
    QPushButton* go{new QPushButton{"Go", this}};

    path_widget(const QString& current, QWidget* parent);

private:
    std::size_t current_entry;
    std::vector<QString> history;

    void setup_layout();
    void setup_connections();

};

#endif
