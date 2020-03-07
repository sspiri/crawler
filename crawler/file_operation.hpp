#ifndef QT_COLLECTION_FILE_OPERATION_HPP
#define QT_COLLECTION_FILE_OPERATION_HPP


#include <functional>
#include <system_error>
#include <filesystem>
#include <stack>

#include <QThread>

#include "detail.hpp"
#include "file_operation_dialog.hpp"



inline std::ptrdiff_t get_files_count(const std::filesystem::path& from){
    std::filesystem::recursive_directory_iterator it{from, std::filesystem::directory_options::skip_permission_denied}, end_it;
    return std::distance(it, end_it) + 1;
}


class file_operation : public QThread{
    Q_OBJECT

signals:
    void process(const QString& );
    void error(const std::filesystem::filesystem_error& );

public slots:
    void toggle_pause(){
        pause_op = !pause_op;
    }

    void cancel(){
        cancel_op = true;
    }

public:
    virtual const char* operation() const = 0;

    bool is_paused() const{
        return pause_op;
    }

protected:
    std::function<void()> callback;
    bool pause_op{}, cancel_op{};

    file_operation(QObject* parent = Q_NULLPTR)
        : QThread{parent} {}

    virtual ~file_operation() {}

    void run() override{
        pause_op = cancel_op = false;

        try{
            callback();
        }

        catch(const detail::quit_exception& ) {}

        catch(const std::filesystem::filesystem_error& err){
            emit error(err);
        }
    }

    void pause(){
        while(pause_op){
            if(cancel_op)
                throw detail::quit_exception{};

            msleep(100);
        }
    }

    void control(){
        if(cancel_op)
            throw detail::quit_exception{};

        if(pause_op)
            pause();
    }
};



class remove_operation final : public file_operation{
    Q_OBJECT

public:
    remove_operation(const QStringList& paths, QObject* parent = Q_NULLPTR)
        : file_operation{parent}{

        callback = std::bind(&remove_operation::remove_files, this, paths);
    }

    const char* operation() const override{
        return "Removing";
    }

private:
    void remove_files(const QStringList& paths){
        std::error_code ec;

        for(const auto& path : paths){
            emit process(path);
            control();

            std::filesystem::remove_all(path.toStdString(), ec);

            if(ec)
                emit error(std::filesystem::filesystem_error{ec.message(), path.toStdString(), ec});
        }
    }
};



class move_operation final : public file_operation{
    Q_OBJECT

public:
    move_operation(const QStringList& paths, const QString& dest, QObject* parent = Q_NULLPTR)
        : file_operation{parent}{

        callback = std::bind(&move_operation::move_files, this, paths, std::filesystem::path{dest.toStdString()});
    }

    const char * operation() const override{
        return "Moving";
    }

private:
    void move_files(const QStringList& paths, const std::filesystem::path& dest){
        std::error_code ec;
        std::filesystem::path destination;

        if(!std::filesystem::is_directory(dest))
            emit error(std::filesystem::filesystem_error{"Not a directory", dest, ec});

        for(const auto& path : paths){
            emit process(" to " + (dest.c_str() + (": [" + path + "]")));
            control();

            destination = dest / std::filesystem::path{path.toStdString()}.filename();
            std::filesystem::rename(path.toStdString(), destination, ec);

            if(ec.value() == (int)std::errc::cross_device_link){
                std::filesystem::copy(path.toStdString(), destination,
                                      std::filesystem::copy_options::recursive | std::filesystem::copy_options::overwrite_existing, ec);

                if(ec)
                    emit error(std::filesystem::filesystem_error{ec.message(), path.toStdString(), destination, ec});

                else{
                    std::filesystem::remove_all(path.toStdString(), ec);

                    if(ec)
                        emit error(std::filesystem::filesystem_error{ec.message(), path.toStdString(), destination, ec});
                }
            }

            else if(ec)
                emit error(std::filesystem::filesystem_error{ec.message(), path.toStdString(), destination, ec});
        }
    }
};



class symlink_operation final : public file_operation{
    Q_OBJECT

public:
    symlink_operation(const QStringList& paths, const QString& dest, QObject* parent = Q_NULLPTR)
        : file_operation{parent}{

        callback = std::bind(&symlink_operation::create_symlinks, this, paths, std::filesystem::path{dest.toStdString()});
    }

    const char * operation() const override{
        return "Moving";
    }

private:
    void create_symlinks(const QStringList& paths, const std::filesystem::path& dest){
        std::error_code ec;
        std::filesystem::path destination;

        if(!std::filesystem::is_directory(dest))
            emit error(std::filesystem::filesystem_error{"Not a directory", dest, ec});

        for(const auto& path : paths){
            emit process(" to " + (dest.c_str() + (": [" + path + "]")));
            control();

            destination = dest / std::filesystem::path{path.toStdString()}.filename();
            std::filesystem::create_symlink(path.toStdString(), destination, ec);

            if(ec)
                emit error(std::filesystem::filesystem_error{ec.message(), path.toStdString(), destination, ec});
        }
    }
};


class copy_operation final : public file_operation{
    Q_OBJECT

public:
    copy_operation(const QStringList& paths, const QString& dest, QObject* parent = Q_NULLPTR)
        : file_operation{parent}{

        for(const auto& path : paths){
            if(std::filesystem::is_directory(path.toStdString()))
                nfiles += get_files_count(path.toStdString());
            else
                ++nfiles;
        }

        callback = std::bind(&copy_operation::copy_files, this, paths, std::filesystem::path{dest.toStdString()});
    }

    const char * operation() const override{
        return "Copying";
    }

    std::ptrdiff_t get_nfiles() const{
        return nfiles;
    }

private:
    std::ptrdiff_t nfiles{};

    void copy_files(const QStringList& paths, const std::filesystem::path& dest){
        std::error_code ec;
        std::filesystem::path path;

        for(const auto& p : paths){
            path = p.toStdString();

            if(std::filesystem::is_directory(path)){
                if(!std::filesystem::is_directory(dest / path.filename()))
                    std::filesystem::create_directories(dest / path.filename());

                std::size_t n = path.parent_path().string().size();
                std::string sub;

                for(std::filesystem::recursive_directory_iterator it{path}, end_it; it != end_it; ){
                    emit process((" to " + (dest / it->path().filename()).string() + ": [" + it->path().string() + "]").c_str());
                    control();

                    sub = it->path().string().substr(n + 1);

                    if(std::filesystem::is_directory(it->path()))
                        std::filesystem::create_directories(dest / sub);
                    else
                        std::filesystem::copy_file(it->path(), dest / sub, std::filesystem::copy_options::overwrite_existing, ec);

                    if(ec)
                        emit error(std::filesystem::filesystem_error{ec.message(), it->path(), dest / sub, ec});

                    try{
                        ++it;
                    }

                    catch(const std::filesystem::filesystem_error& err){
                        emit error(err);
                    }
                }
            }

            else{
                emit process((" to " + (dest / path.filename()).string() + ": [" + path.string() + "]").c_str());
                control();

                std::filesystem::copy_file(path, dest / path.filename(), std::filesystem::copy_options::overwrite_existing, ec);

                if(ec)
                    emit error(std::filesystem::filesystem_error{ec.message(), path, dest / path.filename(), ec});
            }
        }
    }
};


#endif
