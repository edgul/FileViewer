#include "PrintHelper.h"

PrintHelper::PrintHelper()
{

}

void PrintHelper::print(QString message)
{
    if (message != "")
    {
        std::cout << QString(message).toStdString() << std::endl;
        std::flush(std::cout);
    }
}

void PrintHelper::print(QList<QString> list)
{
    foreach (QString item, list)
    {
        print(item);
    }
}
