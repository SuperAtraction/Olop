#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <QtCore>
#include <iostream>

class Logger
{
public:
    static void setupLogging(const QString &fileName, int mode);
    static void logMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg);
    static int modev;
};

#endif // LOGGER_HPP
