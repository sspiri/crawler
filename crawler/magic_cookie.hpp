#ifndef MAGIC_COOKIE_HPP
#define MAGIC_COOKIE_HPP


#include <QString>
#include <memory>
#include <magic.h>


class magic_cookie{
public:
    explicit magic_cookie(int flags = MAGIC_MIME_TYPE)
        : cookie{magic_open(flags)}{

        magic_load(cookie.get(), NULL);
    }

    QString string(const QString& fp) const{
        return magic_file(cookie.get(), fp.toStdString().c_str());
    }

private:
    struct magic_t_deleter{
        void operator()(magic_t magic){
            magic_close(magic);
        }
    };

    std::unique_ptr<std::remove_pointer<magic_t>::type, magic_t_deleter> cookie;
};



#endif
