#include "logger.hpp"
#include <olop.hpp>

extern MAIN OLOP;

Logger::Logger(MAIN nmain)
{
    OLOP=nmain;
    qDebug() << "Logger initialisé";
}
