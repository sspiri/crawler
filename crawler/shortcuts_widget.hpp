#ifndef SHORTCUTS_WIDGET_HPP
#define SHORTCUTS_WIDGET_HPP


#include <QWidget>
#include <QVBoxLayout>
#include <QSplitter>
#include <QGroupBox>

#include "locations_list.hpp"
#include "mounts_list.hpp"


class shortcuts_widget : public QWidget{
public:
    QSplitter* splitter{new QSplitter{this}};
    locations_list* locations{new locations_list{this}};
    mounts_list* mounts{new mounts_list{this}};

    explicit shortcuts_widget(QWidget* parent) : QWidget{parent}{
        splitter->addWidget(locations);
        splitter->addWidget(mounts);
        splitter->setOrientation(Qt::Vertical);
        splitter->setStretchFactor(1, 2);

        auto* box = new QVBoxLayout{};
        box->addWidget(splitter);
        setLayout(box);
    }
};


#endif
