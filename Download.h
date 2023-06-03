#ifndef DOWNLOAD_H
#define DOWNLOAD_H

#include "qprocess.h"
#include "qurl.h"
class download{
public:
    void static launch(QUrl url, bool exi=false, bool exec=false, QString execname="", QString args=""){
        QString program = "OlopDownloader.exe";
            QStringList arguments;
            arguments << url.toDisplayString();
            QProcess *myProcess = new QProcess();
            myProcess->start(program, arguments);
            myProcess->waitForStarted();
            myProcess->waitForFinished();
            if(exec){
            QStringList arguments2;
            arguments << args;
            QProcess *myProcess2 = new QProcess();
            myProcess2->start(execname, arguments);
            }
            if(exi){
                exit(0);
            }
    }
};

#endif // DOWNLOAD_H
