QT += quick widgets
CONFIG += c++17

QMAKE_CXXFLAGS += -std=c++17

LIBS += -lmagic -lstdc++fs -lstdc++


# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Refer to the documentation for the
# deprecated API to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        directory_iterator.cpp \
        files_list.cpp \
        locations_list.cpp \
        main.cpp \
        main_window.cpp \
        mounts_list.cpp \
        path_widget.cpp \
        search_bar.cpp \
        settings_dialog.cpp \
        settings_t.cpp \
        utility.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    columns_list.hpp \
    detail.hpp \
    directory_iterator.hpp \
    file_operation.hpp \
    file_operation_dialog.hpp \
    file_operation_progress.hpp \
    files_list.hpp \
    filesystem.hpp \
    locations_list.hpp \
    magic_cookie.hpp \
    main_window.hpp \
    mounts_list.hpp \
    path_widget.hpp \
    search_bar.hpp \
    settings_dialog.hpp \
    settings_t.hpp \
    shortcuts_widget.hpp \
    string_dialog.hpp \
    utility.hpp
