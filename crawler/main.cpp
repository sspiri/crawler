#include <QApplication>
#include <iostream>

#include "main_window.hpp"


int main(int argc, char** argv) try{
    if(argc > 2){
        std::cerr << "Synopsis: " << argv[0] << " [directory file path]\n";
        return 1;
    }

    if(argc == 2 && !fs::is_directory(argv[1])){
        std::cerr << "Error: [" << argv[1] << "]: Not a directory\n";
        return 2;
    }

    QApplication crawler{argc, argv};

    main_window window(argc == 1 ? QDir::homePath() : argv[1]);
    window.resize(1200, 800);
    window.show();

    return crawler.exec();
}


catch(const std::exception& error){
    std::cerr << "Error: " << error.what() << '\n';
    return 3;
}
