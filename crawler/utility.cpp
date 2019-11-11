#include <filesystem>
#include <iomanip>
#include <cmath>

#include <QStandardPaths>

#include "utility.hpp"


namespace fs = std::filesystem;


QString human_readable_file_size(std::uintmax_t bytes){
    static constexpr const char* units[] = {
        "B", "KiB", "MiB", "GiB", "TiB", "PiB", "EiB", "ZiB"
    };

    static constexpr std::size_t max_units = 8;

    std::size_t counter = 0;
    long double amount = (long double)bytes;

    while(counter < max_units && amount >= (long double)1024.){
        amount /= (long double)1024.;
        ++counter;
    }

    thread_local static std::stringstream stream;
    stream << std::fixed << std::setprecision((unsigned long)std::floor(amount) == (unsigned long)amount ? 0 : 1) << amount << ' ' << units[counter];

    QString result{stream.str().c_str()};

    stream.clear();
    stream.str("");

    return result;
}


QPair<QString, QStringList> get_terminal_command(const QString& file_path){
#ifndef Q_WINDOWS
    if(fs::is_regular_file("/usr/bin/gnome-terminal"))
        return {"gnome-terminal", {"--working-directory=" + file_path}};

    if(fs::is_regular_file("/usr/bin/xfce4-terminal"))
        return {"xfce4-terminal", {"--working-directory=" + file_path}};

    if(fs::is_regular_file("/usr/bin/konsole"))
        return {"konsole", {"--workdir", file_path}};

    if(fs::is_regular_file("/usr/bin/xterm"))
        return {"xterm", {"-e", "cd '" + file_path + "' && { bash -i || sh -i; }"}};

    return {};
#else
    return {"START", {file_path}};
#endif
}


QMap<QString, QString> get_standard_locations(){
    QMap<QString, QString> results;
    QStringList paths;

    paths = QStandardPaths::standardLocations(QStandardPaths::DesktopLocation);
    if(paths.size()) results.insert("Desktop", paths[0]);

    paths = QStandardPaths::standardLocations(QStandardPaths::HomeLocation);
    if(paths.size()) results.insert("Home", paths[0]);

    paths = QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation);
    if(paths.size()) results.insert("Documents", paths[0]);

    paths = QStandardPaths::standardLocations(QStandardPaths::MusicLocation);
    if(paths.size()) results.insert("Music", paths[0]);

    paths = QStandardPaths::standardLocations(QStandardPaths::MoviesLocation);
    if(paths.size()) results.insert("Videos", paths[0]);

    paths = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
    if(paths.size()) results.insert("Pictures", paths[0]);

    paths = QStandardPaths::standardLocations(QStandardPaths::DownloadLocation);
    if(paths.size()) results.insert("Downloads", paths[0]);

    return results;
}
