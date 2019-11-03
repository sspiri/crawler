#include <QCommonStyle>
#include <QDateTime>
#include <QFile>

#include "settings_t.hpp"
#include "utility.hpp"
#include "files_list.hpp"
#include "directory_iterator.hpp"


directory_iterator::directory_iterator(QObject* parent)
    : QThread{parent} {}


void directory_iterator::populate_files_list(){
    auto* parent = (files_list*)this->parent();
    auto entries = parent->current_dir.entryInfoList(parent->settings.filter, QDir::DirsFirst);

    if(parent->files != entries){
        parent->files = std::move(entries);

        emit row_count(parent->files.size());
        emit setup_columns();

        QCommonStyle style;
        QTableWidgetItem* item;
        int row = 0, col = 0;

        for(const auto& info : parent->files){
            if(cancel_op){
                emit resize_columns();
                throw detail::quit_exception{};
            }

            item = new QTableWidgetItem{info.fileName()};
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);
            emit new_item(row, col++, item);

            if(info.isSymLink())
                item->setIcon(style.standardIcon(QStyle::SP_FileLinkIcon));

            else if(info.isFile())
                item->setIcon(style.standardIcon(QStyle::SP_FileIcon));

            else if(info.isDir())
                item->setIcon(style.standardIcon(QStyle::SP_DirIcon));

            else
                item->setIcon(style.standardIcon(QStyle::SP_FileIcon));

            if(parent->settings.headers & headers_mask::modified){
                auto datetime = info.fileTime(QFile::FileModificationTime);

                if(datetime >= QDateTime::currentDateTime().addDays(-7))
                    item = new QTableWidgetItem{datetime.toString("dddd, HH:mm")};
                else
                    item = new QTableWidgetItem{datetime.toString("dd/MM/yyyy HH:mm")};

                item->setFlags(item->flags() & ~Qt::ItemIsEditable);
                emit new_item(row, col++, item);
            }

            if(parent->settings.headers & headers_mask::size){
                item = new QTableWidgetItem{human_readable_file_size((std::uintmax_t)info.size())};
                item->setFlags(item->flags() & ~Qt::ItemIsEditable);
                emit new_item(row, col++, item);
            }

            if(parent->settings.headers & headers_mask::type){
                item = new QTableWidgetItem{cookie.string(info.absoluteFilePath())};
                item->setFlags(item->flags() & ~Qt::ItemIsEditable);
                emit new_item(row, col++, item);
            }

            if(parent->settings.headers & headers_mask::path){
                item = new QTableWidgetItem{info.absoluteFilePath()};
                item->setFlags(item->flags() & ~Qt::ItemIsEditable);
                emit new_item(row, col++, item);
            }

            ++row;
            col = 0;

            if(!(row % 100))
                emit resize_columns();
        }

        emit resize_columns();
    }
}
