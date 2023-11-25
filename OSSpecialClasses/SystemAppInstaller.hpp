#ifndef SYSTEMAPPINSTALLER_HPP
#define SYSTEMAPPINSTALLER_HPP

#include <QProcess>
#include "olop.hpp"

class SystemAppInstaller
{
public:
    SystemAppInstaller();
    static int installsystemapp(int type, QString packagename, QString Dest);
};

#endif // SYSTEMAPPINSTALLER_HPP
