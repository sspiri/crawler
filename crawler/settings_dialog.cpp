#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QButtonGroup>

#include "settings_dialog.hpp"


settings_dialog::settings_dialog(settings_t* options, QWidget* parent) : QDialog{parent}, settings{options}{
    apply_settings();

    setup_layout();
    setup_connections();
}


void settings_dialog::apply_settings(){
    trash->setText(settings->trash_directory);
    hidden->setChecked(settings->filter & QDir::Hidden);

    modified->setChecked(settings->headers & headers_mask::modified);
    size->setChecked(settings->headers & headers_mask::size);
    type->setChecked(settings->headers & headers_mask::type);
    path->setChecked(settings->headers & headers_mask::path);
}


void settings_dialog::setup_layout(){
    auto* grid = new QGridLayout{};

    grid->addWidget(hidden, 0, 0, 1, 2);

    grid->addWidget(new QLabel{"Trash directory:", this}, 1, 0, Qt::AlignRight);
    grid->addWidget(trash, 1, 1);

    grid->addWidget(new QLabel{"Show columns:", this}, 2, 0, Qt::AlignRight);

    auto* box = new QHBoxLayout{};
    box->addWidget(modified);
    box->addWidget(size);
    box->addWidget(type);
    box->addWidget(path);

    grid->addLayout(box, 2, 1);

    box = new QHBoxLayout{};
    box->addStretch();
    box->addWidget(ok);
    box->addWidget(cancel);

    grid->addLayout(box, 3, 0, 1, 2);

    setLayout(grid);
}


void settings_dialog::setup_connections(){
    connect(ok, &QPushButton::clicked, this, &QDialog::accept);
    connect(cancel, &QPushButton::clicked, this, &QDialog::reject);
}


void settings_dialog::set_settings(){
    settings->trash_directory = trash->text();

    if(hidden->isChecked())
        settings->filter |= QDir::Hidden;
    else
        settings->filter &= ~QDir::Hidden;

    settings->headers = (headers_mask)0;

    if(modified->isChecked())
        settings->headers = (headers_mask)(settings->headers | headers_mask::modified);

    if(size->isChecked())
        settings->headers = (headers_mask)(settings->headers | headers_mask::size);

    if(type->isChecked())
        settings->headers = (headers_mask)(settings->headers | headers_mask::type);

    if(path->isChecked())
        settings->headers = (headers_mask)(settings->headers | headers_mask::path);


    settings->write();
}
