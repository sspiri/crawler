#ifndef LOCATIONS_LIST_HPP
#define LOCATIONS_LIST_HPP


#include <QWidget>
#include <QListWidget>
#include <QTimer>
#include <QMap>


class locations_list : public QWidget{
private slots:
    void setup_locations();

public:
    QListWidget* list{new QListWidget{this}};

    explicit locations_list(QWidget* parent);

    const QMap<QString, QString>& get_locations() const{
        return locations;
    }

private:
    QMap<QString, QString> locations;
    QTimer* timer{new QTimer{this}};

    void setup_layout();
    void setup_connections();
};


#endif
