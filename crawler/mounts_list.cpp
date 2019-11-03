#include "mounts_list.hpp"

#include <QLabel>
#include <QVBoxLayout>


mounts_list::mounts_list(QWidget* parent) : QWidget{parent}{
    setup_connections();
    setup_layout();
    setup_mounts_list();

    timer->setSingleShot(false);
    timer->start(500);
}


void mounts_list::setup_layout(){
    auto* box = new QVBoxLayout{};

    auto* label = new QLabel{"Mount points:", this};

    QFont font;
    font.setBold(true);
    label->setFont(font);

    box->addWidget(label);
    box->addWidget(list);

    setLayout(box);
}


void mounts_list::setup_connections(){
    connect(timer, &QTimer::timeout, this, &mounts_list::setup_mounts_list);
}


void mounts_list::setup_mounts_list(){
    auto storage = QStorageInfo::mountedVolumes();

    if(mount_points != storage){
        mount_points = std::move(storage);

        list->clear();

        for(const auto& point : mount_points)
            list->addItem(point.rootPath());
    }
}
