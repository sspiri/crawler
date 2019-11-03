#ifndef MOUNTS_LIST_HPP
#define MOUNTS_LIST_HPP


#include <QListWidget>
#include <QStorageInfo>
#include <QTimer>


class mounts_list : public QWidget{
private slots:
    void setup_mounts_list();

public:
    QListWidget* list{new QListWidget{this}};

    explicit mounts_list(QWidget* parent);

private:
    QList<QStorageInfo> mount_points;
    QTimer* timer{new QTimer{this}};

    void setup_layout();
    void setup_connections();
};


#endif
