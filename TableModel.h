#ifndef TABLEMODEL_H
#define TABLEMODEL_H

#include <QFileSystemWatcher>
#include <QAbstractTableModel>
#include <QList>
#include <QTimer>
#include "FileRecord.h"

class TableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit TableModel(QObject *parent = 0);

    void clear();
    QList<QString> files_under_watch();

    void update(QString record_path, int size);

    int total_file_size();
    void clear_files();

    void add_file(QString file_path);
    void remove_file(QString file_path);
    void remove_record(QString record_path);

    bool add_dir(QString dir_path);

    int record_index(QString record_path);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Add data:
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    // Remove data:
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    void clear_dirs_from_watchlist();

signals:
    void directory_changed();
    void file_changed();

private slots:
    void directory_changed(QString dir_path);
    void file_change_detected(QString file_path);

    void directory_changed_delay_timer_tick();
    void file_changed_delay_timer_tick();

private:

    // Use two file system watchers as a workaround for apparent bug when removing paths
    QFileSystemWatcher file_system_watcher_dir;
    QFileSystemWatcher file_system_watcher_files;

    QList<FileRecord *> file_records;

    QList<QString> files_that_have_changed;

    QTimer directory_changed_delay_timer;
    QTimer files_changed_delay_timer;

    QList<QString> files_under_watch_cache; // file_system_watcher.files() introduces race conditions

};

#endif // TABLEMODEL_H
