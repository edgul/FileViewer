#ifndef FILERECORD_H
#define FILERECORD_H

#include <QString>

class FileRecord
{
public:
    FileRecord();

    QString abs_path;
    int size;
};

#endif // FILERECORD_H
