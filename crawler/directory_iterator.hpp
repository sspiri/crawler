#ifndef DIRECTORY_ITERATOR_HPP
#define DIRECTORY_ITERATOR_HPP


#include <QApplication>
#include <QMessageBox>
#include <QTableWidgetItem>
#include <QThread>
#include <QDir>

#include "detail.hpp"
#include "magic_cookie.hpp"


class directory_iterator : public QThread{
    Q_OBJECT

signals:
    void show_row(int );
    void hide_row(int );

    void row_count(int );
    void setup_columns();

    void resize_columns();
    void new_item(int, int, QTableWidgetItem* );

public:
    explicit directory_iterator(QObject* parent);

public slots:
    void cancel(){
        cancel_op = true;
    }

    void set_force(bool force){
        force_op = force;
    }

    void set_recursive(bool recursive){
        recursive_op = recursive;
    }

    void set_search(bool s, const QRegularExpression& reg = {}){
        regex = reg;
        search_op = s;
    }

    void join(){
        if(isRunning()){
            do{
                QApplication::processEvents();
            } while(!wait(0));
        }
    }

protected:
    void run() override{
        cancel_op = false;

        try{
            populate_files_list();

            if(search_op){
                search();
                search_op = false;
            }
        }

        catch(const std::exception& err){
            QMessageBox::critical(nullptr, "Error", err.what());
        }

        catch(const detail::quit_exception& ) {}
    }

private:
    magic_cookie cookie;
    QRegularExpression regex;
    bool cancel_op, force_op{false}, recursive_op{false}, search_op{false};

    QList<QFileInfo> get_file_infos();
    void populate_files_list();
    void search();
};


#endif
