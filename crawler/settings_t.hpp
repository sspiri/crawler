#ifndef SETTINGS_T_HPP
#define SETTINGS_T_HPP


#include <QDir>


enum headers_mask{
    modified =  1 << 0,
    size =      1 << 1,
    type =      1 << 2,
    path =      1 << 3,
    all = size | modified | type | path,
};


struct settings_t{
    QString trash_directory{QDir::home().absoluteFilePath(".crawler_trash")};
    QDir::Filters filter{QDir::NoDotAndDotDot | QDir::AllEntries};
    headers_mask headers{headers_mask::all};
};


#endif
