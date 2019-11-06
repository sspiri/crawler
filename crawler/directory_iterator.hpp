#ifndef DIRECTORY_ITERATOR_HPP
#define DIRECTORY_ITERATOR_HPP


#include <QMessageBox>
#include <QTableWidgetItem>
#include <QThread>
#include <QDir>

#include "detail.hpp"
#include "magic_cookie.hpp"


class directory_iterator : public QThread{
    Q_OBJECT

signals:
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

protected:
    void run() override{
        cancel_op = false;

        try{
            populate_files_list();
        }

        catch(const std::exception& err){
            QMessageBox::critical(nullptr, "Error", err.what());
        }

        catch(const detail::quit_exception& ) {}
    }

private:
    magic_cookie cookie;
    bool cancel_op, force_op{false};

    void populate_files_list();
};


#endif
