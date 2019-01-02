#include "TableModel.h"

#include <QFileInfo>
#include "PrintHelper.h"

#define NUM_COLUMNS (2)
#define COLUMN_INDEX_PATH (0)
#define COLUMN_INDEX_SIZE (1)

TableModel::TableModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    connect(&file_system_watcher_dir, SIGNAL(directoryChanged(QString)), this, SLOT(directory_changed(QString)));
    connect(&file_system_watcher_files, SIGNAL(fileChanged(QString)), this, SLOT(file_change_detected(QString)));


    directory_changed_delay_timer.setSingleShot(true);
    connect(&directory_changed_delay_timer, SIGNAL(timeout()), this, SLOT(directory_changed_delay_timer_tick()));

    files_changed_delay_timer.setSingleShot(true);
    connect(&files_changed_delay_timer, SIGNAL(timeout()), this, SLOT(file_changed_delay_timer_tick()));
}

void TableModel::clear_dirs_from_watchlist()
{
    QList<QString> dirs_under_watch = file_system_watcher_dir.directories();
    foreach (QString dir, dirs_under_watch)
    {
        file_system_watcher_dir.removePath(dir);
    }
}

void TableModel::clear()
{
    clear_dirs_from_watchlist();

    // clear files from watchlist and records
    QList<QString> files = files_under_watch();
    foreach (QString file, files)
    {
        file_system_watcher_files.removePath(file);
        remove_record(file);
    }

    clear_files();
}

QList<QString> TableModel::files_under_watch()
{
    return files_under_watch_cache;
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
}

void TableModel::add_file(QString file_path)
{
    // Add to file_system_watcher
    bool success = file_system_watcher_files.addPath(file_path);
    if (success) files_under_watch_cache.append(file_path);

    QString msg = QString("Added to watchlist: %1").arg(file_path);
    if (!success) msg = QString("Failed to add to watchlist: %1").arg(file_path);
    PrintHelper::print(msg);

    QFileInfo file_info(file_path);

    // create the new FileRecord
    FileRecord * file_record = new FileRecord();
    file_record->abs_path = file_info.absoluteFilePath();
    file_record->size = file_info.size();

    int row_index = record_index(file_record->abs_path);

    msg = "";
    if (row_index == -1) // not found -- add new record
    {
        int row = file_records.size();
        if (insertRow(row))
        {
            file_records[row] = file_record;

            msg = QString("Added new record: %1").arg(file_record->abs_path);
        }
        else
        {
            msg = QString("(1) Failed to add new record: %1").arg(file_record->abs_path);
        }
    }
    else
    {
        msg = QString("(2) Failed to add new record: %1").arg(file_record->abs_path);
    }

    PrintHelper::print(msg);
}

void TableModel::remove_file(QString file_path)
{
    // watcher will remove deleted files automatically

    files_under_watch_cache.removeAll(file_path);

    remove_record(file_path);
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

bool TableModel::add_dir(QString dir_path)
{
    clear_dirs_from_watchlist(); // only 1 dir under watch

    bool result = file_system_watcher_dir.addPath(dir_path);

    return result;
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

        file_records.append(0); // add null pointer

        endInsertRows();

        PrintHelper::print("Row Added");

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

        endRemoveRows();

        delete record;

        PrintHelper::print("Row removed.");

        return true;
    }

    return false;
}

void TableModel::directory_changed(QString dir_path)
{
    Q_UNUSED(dir_path);

    // When many changes are happening at once table update is slow because processing happens per file change
    // Attempt to only run processing once per set of changes by adding a delay
    directory_changed_delay_timer.start(1000);

}

void TableModel::file_change_detected(QString file_path)
{
    files_changed_delay_timer.start(1000);

    files_that_have_changed.append(file_path);
}

void TableModel::directory_changed_delay_timer_tick()
{
    emit directory_changed();
}

void TableModel::file_changed_delay_timer_tick()
{
    foreach (QString file_path, files_that_have_changed)
    {
        QFileInfo file_info(file_path);
        update(file_path, file_info.size());
    }
    files_that_have_changed.clear();

    emit file_changed();
}
