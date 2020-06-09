#include "locations_list.hpp"
#include "utility.hpp"

#include <QLabel>
#include <QVBoxLayout>


locations_list::locations_list(QWidget* parent) : QWidget{parent}{
    setup_connections();
    setup_layout();
    setup_locations();

    timer->setSingleShot(false);
    timer->start(500);
}


void locations_list::setup_layout(){
    auto* box = new QVBoxLayout{};

    auto* label = new QLabel{"Default Locations:", this};

    QFont font;
    font.setBold(true);
    label->setFont(font);

    box->addWidget(label);
    box->addWidget(list);

    setLayout(box);
}


void locations_list::setup_connections(){
    connect(timer, &QTimer::timeout, this, &locations_list::setup_locations);
}


void locations_list::setup_locations(){
    auto shortcuts = get_standard_locations();

    if(locations != shortcuts){
        locations = std::move(shortcuts);

        list->clear();

        for(const auto& key : locations.keys())
            list->addItem(key);
    }
}
