/********************************************************************************
** Form generated from reading UI file 'newtab.ui'
**
** Created by: Qt User Interface Compiler version 6.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_NEWTAB_H
#define UI_NEWTAB_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_NewTab
{
public:
    QLineEdit *lineEdit;

    void setupUi(QWidget *NewTab)
    {
        if (NewTab->objectName().isEmpty())
            NewTab->setObjectName("NewTab");
        NewTab->resize(571, 551);
        lineEdit = new QLineEdit(NewTab);
        lineEdit->setObjectName("lineEdit");
        lineEdit->setGeometry(QRect(0, 0, 571, 25));

        retranslateUi(NewTab);

        QMetaObject::connectSlotsByName(NewTab);
    } // setupUi

    void retranslateUi(QWidget *NewTab)
    {
        NewTab->setWindowTitle(QCoreApplication::translate("NewTab", "Form", nullptr));
    } // retranslateUi

};

namespace Ui {
    class NewTab: public Ui_NewTab {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_NEWTAB_H
