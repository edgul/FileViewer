#include "TableModel.h"

#include <iostream>
#include <QFileInfo>

#define NUM_COLUMNS (2)
#define COLUMN_INDEX_PATH (0)
#define COLUMN_INDEX_SIZE (1)

TableModel::TableModel(QObject *parent)
    : QAbstractTableModel(parent)
{

}

QList<FileRecord *> TableModel::files()
{
    return file_records;
}

void TableModel::update(QString record_path, int size)
{
    int row_index = record_index(record_path);

    foreach (FileRecord * file_record, file_records)
    {
        if (file_record->abs_path == record_path)
        {
            file_record->size = size;
            break;
        }
    }

    // only update cell that needs it
    emit dataChanged(index(row_index, 1), index(row_index,1));

}

void TableModel::clear_files()
{
    foreach (auto i, file_records)
    {
        removeRow(0);
    }

    file_records.clear(); // TODO: leak?
}

void TableModel::add_record(FileRecord * new_record)
{
    int row_index = record_index(new_record->abs_path);

    if (row_index == -1) // not found -- add new record
    {
        int row = file_records.size();
        if (insertRow(row))
        {
            file_records[row] = new_record;
        }
        else
        {
            std::cout << QString("(1) Failed to add new record: %1").arg(new_record->abs_path).toStdString();
        }
    }
    else
    {
        std::cout << QString("(2) Failed to add new record: %1").arg(new_record->abs_path).toStdString();
    }

}

void TableModel::remove_record(QString record_path)
{
    int r_index = record_index(record_path);

    bool success = false;
    if (r_index != -1)
    {
        success = removeRow(r_index);
    }
    else
    {
      std::cout << QString("(1) Failed to remove from model: %1").arg(record_path).toStdString();
    }

    if (!success) std::cout << QString("(2) Failed to remove from model: %1").arg(record_path).toStdString();

}

int TableModel::record_index(QString record_path)
{
    int index = -1;

    for (int i = 0; i < file_records.size(); i++)
    {
        FileRecord * record = file_records[i];

        if (record->abs_path == record_path)
        {
            index = i;
            break;
        }
    }

    return index;
}


QVariant TableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    QVariant result = QVariant();

    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal)
        {
            if (section == COLUMN_INDEX_PATH)
            {
                result = QString("File Name");
            }
            else if (section == COLUMN_INDEX_SIZE)
            {
                result = QString("Size (B)");
            }
        }
    }

    return result;
}

int TableModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) return 0;

    return file_records.size();
}

int TableModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid()) return 0;

    return NUM_COLUMNS;
}

QVariant TableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) return QVariant();

    int row = index.row();
    int column = index.column();

    QVariant result = QVariant();

    switch (role)
    {
        case Qt::DisplayRole:
        {
            if (row >= 0 && row < file_records.size())
            {
                FileRecord * record_at_row = file_records[row];

                if (column == COLUMN_INDEX_PATH)
                {
                    result = QFileInfo(record_at_row->abs_path).fileName();
                }
                else if (column == COLUMN_INDEX_SIZE)
                {
                    result = record_at_row->size;
                }
            }
        }break;
    }


    return result;
}

bool TableModel::insertRows(int row, int count, const QModelIndex &parent)
{
    if (row >= 0 && row <= file_records.size())
    {
        beginInsertRows(parent, row, row + count - 1);

        file_records.append(0);

        endInsertRows();

        return true;
    }

    return false;
}

bool TableModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (row >= 0 && row < file_records.size())
    {
        beginRemoveRows(parent, row, row + count - 1);

        FileRecord * record = file_records.takeAt(row);
        delete record;

        endRemoveRows();
        return true;
    }

    return false;
}
