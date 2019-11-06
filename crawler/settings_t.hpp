#ifndef SETTINGS_T_HPP
#define SETTINGS_T_HPP


#include <QObject>
#include <QStandardPaths>
#include <QDir>

#include "filesystem.hpp"


enum headers_mask{
    modified =  1 << 0,
    size =      1 << 1,
    type =      1 << 2,
    path =      1 << 3,
    all = size | modified | type | path,
};


class settings_t : public QObject{
    Q_OBJECT

signals:
    void error(const QString& );

public:
    inline static const QString file_path{QStandardPaths::standardLocations(QStandardPaths::ConfigLocation)[0] + "/crawler/config.data"};

    explicit settings_t(QObject* parent) : QObject{parent}{
        QString config_dir{fs::path{file_path.toStdString()}.parent_path().c_str()};

        if(!QFileInfo{config_dir}.isDir()){
            std::error_code ec;
            fs::create_directories(config_dir.toStdString(), ec);

            if(ec)
                emit error('[' + config_dir + "]: Cannot create directory");
        }
    }

    QString trash_directory{(fs::path{file_path.toStdString()}.parent_path() / "trash").c_str()};
    QDir::Filters filter{QDir::NoDotAndDotDot | QDir::AllEntries};
    headers_mask headers{headers_mask::all};

    void read();
    void write();
};


#endif
