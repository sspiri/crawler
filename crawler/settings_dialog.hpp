#ifndef SETTINGS_DIALOG_HPP
#define SETTINGS_DIALOG_HPP


#include <QDialog>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>

#include "settings_t.hpp"


class settings_dialog : public QDialog{
public:
    QLineEdit* trash{new QLineEdit{this}};
    QCheckBox* hidden{new QCheckBox{"Show hidden files", this}};

    QCheckBox* modified{new QCheckBox{"Modified", this}},
             * size{new QCheckBox{"Size", this}},
             * type{new QCheckBox{"Type", this}},
             * path{new QCheckBox{"Path", this}};

    QPushButton* ok{new QPushButton{"Ok", this}},
               * cancel{new QPushButton{"Cancel", this}};


    settings_dialog(const settings_t& settings, QWidget* parent);

    settings_t get_settings() const;

private:
    void apply_settings(const settings_t& settings);

    void setup_layout();
    void setup_connections();
};


#endif
