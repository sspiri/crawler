#ifndef MAIN_WINDOW_HPP
#define MAIN_WINDOW_HPP


#include <QMainWindow>
#include <QWidget>
#include <QSplitter>
#include <QDir>

#include "mounts_list.hpp"
#include "search_bar.hpp"
#include "files_list.hpp"
#include "path_widget.hpp"

#include "filesystem.hpp"


class main_window : public QMainWindow{
private slots:
    void enter_if_exists(){
        if(QFile::exists(path->edit->text()))
            files->enter(path->edit->text());
    }

public:
    explicit main_window(const QString& current);

private:
    QWidget* central{new QWidget{this}};
    search_bar* search{new search_bar{this}};
    QSplitter* splitter{new QSplitter{this}};
    mounts_list* mounts{new mounts_list{this}};
    path_widget* path;
    files_list* files;

    void setup_connections();
    void set_menu_bar();
    void setup_layout();
};


#endif
