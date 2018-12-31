#include "TableModel.h"

#include <QFileInfo>
#include "PrintHelper.h"

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

int TableModel::total_file_size()
{
    int size = 0;
    foreach (FileRecord * record, file_records )
    {
        size += record->size;
    }

    return size;
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

    QString msg = "";
    if (row_index == -1) // not found -- add new record
    {
        int row = file_records.size();
        if (insertRow(row))
        {
            file_records[row] = new_record;

            msg = QString("Added new record: %1").arg(new_record->abs_path);
        }
        else
        {
            msg = QString("(1) Failed to add new record: %1").arg(new_record->abs_path);
        }
    }
    else
    {
        msg = QString("(2) Failed to add new record: %1").arg(new_record->abs_path);
    }

    PrintHelper::print(msg);


}

void TableModel::remove_record(QString record_path)
{
    int r_index = record_index(record_path);
    QString msg = "";

    bool success = false;
    if (r_index != -1)
    {
        success = removeRow(r_index);
    }
    else
    {
        msg = QString("(1) Failed to remove from model: %1").arg(record_path);
    }

    msg = QString("Removed row: %1").arg(r_index);
    if (!success) msg = QString("(2) Failed to remove from model: %1").arg(record_path);

    PrintHelper::print(msg);

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

                PrintHelper::print("HeaderData: File Name called");
            }
            else if (section == COLUMN_INDEX_SIZE)
            {
                result = QString("Size (B)");

                PrintHelper::print("HeaderData: Size called");
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

        PrintHelper::print("Row Added");

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

        PrintHelper::print("Row removed.");

        endRemoveRows();
        return true;
    }

    return false;
}
