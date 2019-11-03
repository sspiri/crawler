#include "search_bar.hpp"
#include <QRegularExpression>


void search_bar::setup_connections(){
    connect(edit, &QLineEdit::textChanged, this, &search_bar::on_changed);
}


void search_bar::on_changed(){
    QRegularExpression regex(edit->text(), QRegularExpression::CaseInsensitiveOption);

    if(regex.isValid()){
        QPalette palette;
        palette.setColor(QPalette::Text, Qt::black);
        edit->setPalette(palette);

        emit changed_valid_regex(regex);
    }

    else{
        QPalette palette;
        palette.setColor(QPalette::Text, Qt::red);
        edit->setPalette(palette);
    }
}
