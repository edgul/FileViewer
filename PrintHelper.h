#ifndef PRINTHELPER_H
#define PRINTHELPER_H

#include <QString>
#include <QList>
#include <iostream>

class PrintHelper
{
public:
    PrintHelper();

    static void print(QString message);

    static void print(QList<QString> list);

    static bool printing_on;
};

#endif // PRINTHELPER_H
