#include "ScopeTimer.hpp"

#include <cmath>

#include <chrono>
#include <QDebug>

namespace debug {

ScopeTimer::ScopeTimer(QString place) : m_t_startingPoint{clk::now()},
    m_callPlace{std::move(place)}
{

}

ScopeTimer::~ScopeTimer()
{
    auto end{clk::now()};
    std::chrono::duration<double> diff{end-m_t_startingPoint};
    auto cnt{diff.count()};

//    if(cnt > 0.)
//    {
//    }
//    else if(cnt/1000 > 0.)
//    {
//        cnt =
//    }
    int power{};
    for(power = {}; power <= 6;power += 3)
    {
        double divideBy(pow(10,power));
        cnt *= divideBy;
        if(cnt < 1.)//if we are not in the right "size order"
        {
            continue;
        }
        //if we are in the right "size order"
        break;
    }
    QString suffix{};
    switch(power){
    case 0:
        suffix = "s";
        break;
    case 3:
        suffix = "ms";
        break;
    case 6:
        suffix = "µs";
        break;
    default:
        suffix = "Unknown dimension";
    }

    qDebug() << QString("-> Timer from '%0' : %1 %2").arg(m_callPlace).arg(QString::number(cnt,'f',6)).arg(suffix);
}

} // namespace debug
