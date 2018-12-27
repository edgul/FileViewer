#include "TableModel.h"

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

    file_records.clear();
}

void TableModel::add_record(FileRecord * new_record)
{

    int row_index = record_index(new_record->abs_path);

    if (row_index == -1) // not found -- add new record
    {
        insertRow(0);
        file_records.append(new_record);
        row_index = file_records.size();

    }
    else // found -- update an existing record
    {
        if (file_records[row_index]->abs_path == new_record->abs_path)
        {
            file_records[row_index]->size = new_record->size;
        }
    }
}

void TableModel::remove_record(QString record_path)
{
    for (int i = 0; i < file_records.size(); i++)
    {
        FileRecord * record = file_records[i];

        if (record->abs_path == record_path)
        {
            file_records.removeAt(i);
            delete record;
            break;
        }
    }
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

    if (row >= 0 && row < file_records.size())
    {
        FileRecord * record_at_row = file_records[row];

        if (column == COLUMN_INDEX_PATH)
        {
            result = record_at_row->abs_path;
        }
        else if (column == COLUMN_INDEX_SIZE)
        {
            result = record_at_row->size;
        }
    }

    return result;
}

bool TableModel::insertRows(int row, int count, const QModelIndex &parent)
{
    beginInsertRows(parent, row, row + count - 1);

    endInsertRows();

    return true;
}

bool TableModel::removeRows(int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row + count - 1);

    endRemoveRows();
}
