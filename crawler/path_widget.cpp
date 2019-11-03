#include <QHBoxLayout>
#include <QCommonStyle>
#include <QShortcut>
#include <QFile>

#include "path_widget.hpp"


path_widget::path_widget(const QString& current, QWidget* parent) : QWidget{parent}{
    QCommonStyle style;
    back->setIcon(style.standardIcon(QStyle::SP_ArrowUp));

    edit->setText(current);
    add_to_history(current);

    setup_layout();
    setup_connections();
}


void path_widget::setup_layout(){
    auto* box = new QHBoxLayout;

    box->addWidget(back);
    box->addWidget(edit);
    box->addWidget(go);

    setLayout(box);
}


void path_widget::setup_connections(){
    connect(edit, &QLineEdit::textChanged, this, &path_widget::validate_line);

    connect(edit, &QLineEdit::returnPressed, [this]{
        add_to_history(edit->text());
    });

    connect(go, &QPushButton::clicked, [this]{
        add_to_history(edit->text());
    });

    connect(new QShortcut{Qt::Key_Up, this}, &QShortcut::activated, this, &path_widget::go_up);
    connect(new QShortcut{Qt::Key_Down, this}, &QShortcut::activated, this, &path_widget::go_down);
}


void path_widget::validate_line(){
    if(!QFile::exists(edit->text())){
        QPalette palette;
        palette.setColor(QPalette::Text, Qt::red);
        edit->setPalette(palette);
    }

    else{
        QPalette palette;
        palette.setColor(QPalette::Text, Qt::black);
        edit->setPalette(palette);
    }
}


void path_widget::add_to_history(const QString& current){
    if(!QFile::exists(current))
        return;

    auto it = std::find(history.begin(), history.end(), current);

    if(it != history.end())
        history.erase(it);

    history.push_back(current);
    current_entry = history.size() - 1;
}


void path_widget::go_up(){
    if(current_entry > 0)
        edit->setText(history[--current_entry]);
}


void path_widget::go_down(){
    if(current_entry < history.size() - 1)
        edit->setText(history[++current_entry]);
}
