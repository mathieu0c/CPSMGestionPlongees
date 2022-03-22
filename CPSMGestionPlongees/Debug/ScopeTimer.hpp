#ifndef DEBUG_SCOPETIMER_HPP
#define DEBUG_SCOPETIMER_HPP

#include <chrono>
#include <QString>

#ifndef __FILE__
#error "File macro used for debugging purpose is not defined"
#else
#define _LINE_ QString::number(__LINE__)
#endif

#pragma clang diagnostic ignored "-Wstring-plus-int"
#define __FILENAME_PRIVATE__ (__FILE__ + SOURCE_PATH_SIZE)
#define __CURRENT_PLACE__ QString{"%0 : <%1> : l.%2"}.arg(__FILENAME_PRIVATE__,__func__,_LINE_)

namespace debug {

class ScopeTimer
{
private:
    using clk = std::chrono::high_resolution_clock;

public:
    explicit ScopeTimer(QString place = {});

    ~ScopeTimer();

private:
    std::chrono::time_point<clk> m_t_startingPoint;
    QString m_callPlace;
};

} // namespace debug

#endif // DEBUG_SCOPETIMER_HPP
