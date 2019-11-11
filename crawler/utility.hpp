#ifndef UTILITY_HPP
#define UTILITY_HPP


#include <QStringList>
#include <QString>
#include <QPair>


QString human_readable_file_size(std::uintmax_t bytes);

QPair<QString, QStringList> get_terminal_command(const QString& file_path);

#endif
