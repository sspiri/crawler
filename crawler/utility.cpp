#include <iomanip>
#include <cmath>

#include "utility.hpp"



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
