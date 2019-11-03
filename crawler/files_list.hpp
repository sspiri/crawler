#ifndef FILES_LIST_HPP
#define FILES_LIST_HPP


#include <QGuiApplication>
#include <QTableWidget>
#include <QHeaderView>
#include <QTimer>
#include <QFileInfo>
#include <QDir>
#include <QClipboard>


class directory_iterator;

class files_list : public QTableWidget{
    Q_OBJECT

    friend class directory_iterator;

signals:
    void new_path(const QString& );

public slots:
    void setup_files_list();

    void enter_directory(const QString& path);
    void enter(const QString& path);
    void go_back();

    void hide_items(const QRegularExpression& regex);

    void copy();
    void cut();
    void paste();
    void remove();

private slots:
    void open_file(const QStringList& arguments);

    void row_double_clicked(int row, int );
    void show_context_menu(const QPoint& point);

    void create_file();
    void create_directory();

    void on_copy();
    void on_cut();

    void create_symlinks();
    void rename();

public:
    enum headers_mask{
        modified =  1 << 0,
        size =      1 << 1,
        type =      1 << 2,
        path =      1 << 3,
        all = size | modified | type | path,
    };

    QDir::Filters filter{QDir::NoDotAndDotDot | QDir::AllEntries};
    headers_mask headers{headers_mask::all};

    files_list(const QString& current, QWidget* parent);
    ~files_list();

private:
    QDir current_dir;
    QFileInfoList files;
    std::string clipboard_operation;

    QTimer* timer = new QTimer{this};
    QClipboard* clipboard{QGuiApplication::clipboard()};
    directory_iterator* dir_it;

    void setup_connections();
    void setup_columns();

    QStringList get_selected_files() const;
};


#endif
