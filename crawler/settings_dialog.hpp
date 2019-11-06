#ifndef SETTINGS_DIALOG_HPP
#define SETTINGS_DIALOG_HPP


#include <QDialog>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>

#include "settings_t.hpp"


class settings_dialog : public QDialog{
public:
    settings_t* settings;

    QLineEdit* trash{new QLineEdit{this}};
    QCheckBox* hidden{new QCheckBox{"Show hidden files", this}};

    QCheckBox* modified{new QCheckBox{"Modified", this}},
             * size{new QCheckBox{"Size", this}},
             * type{new QCheckBox{"Type", this}},
             * path{new QCheckBox{"Path", this}};

    QPushButton* ok{new QPushButton{"Ok", this}},
               * cancel{new QPushButton{"Cancel", this}};


    settings_dialog(settings_t* settings, QWidget* parent);

    void set_settings();

private:
    void apply_settings();

    void setup_layout();
    void setup_connections();
};


#endif
