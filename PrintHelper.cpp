#include "PrintHelper.h"

bool PrintHelper::printing_on = false;

PrintHelper::PrintHelper()
{

}

void PrintHelper::print(QString message)
{
    if (PrintHelper::printing_on)
    {
        if (message != "")
        {
            std::cout << QString(message).toStdString() << std::endl;
            std::flush(std::cout);
        }
    }
}

void PrintHelper::print(QList<QString> list)
{
    if (PrintHelper::printing_on)
    {
        foreach (QString item, list)
        {
            print(item);
        }
    }
}
