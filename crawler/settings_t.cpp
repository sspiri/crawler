#include <fstream>
#include <iomanip>

#include "settings_t.hpp"


void settings_t::read(){
    std::ifstream in{file_path.toStdString(), std::ios::binary};

    if(!in.is_open()){
        emit error(('[' + file_path.toStdString() + "]: Cannot open for reading app data").c_str());
        return;
    }

    {
        std::string trash_dir;
        in >> std::quoted(trash_dir);
        trash_directory = trash_dir.c_str();
    }

    in.read((char*)&filter, sizeof(filter));
    in.read((char*)&headers, sizeof(headers));

    if(!in)
        emit error(('[' + file_path.toStdString() + "]: Cannot open for reading app data").c_str());
}


void settings_t::write(){
    std::ofstream out{file_path.toStdString(), std::ios::binary};

    if(!out){
        emit error(('[' + file_path.toStdString() + "]: Cannot open for saving app data").c_str());
        return;
    }

    out << std::quoted(trash_directory.toLocal8Bit().data());
    out.write((char*)&filter, sizeof(filter));
    out.write((char*)&headers, sizeof(headers));
}
