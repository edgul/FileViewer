#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QFileSystemWatcher>
#include <QDir>
#include <QTimer>
#include "FileRecord.h"
#include "TableModel.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void update_totals();

    void reset_watch_folder_dir();

private slots:
    void on_button_browse_clicked();
    void on_button_watch_clicked();

    void directory_changed(QString dir_path);
    void file_changed(QString file_path);

private:
    Ui::MainWindow * ui;

    TableModel * table_model;

	// Use two file system watchers as a workaround for apparent bug when removing paths
    QFileSystemWatcher file_system_watcher_dir;
    QFileSystemWatcher file_system_watcher_files;

    QString watch_folder_path;
    QDir watch_folder_dir;

    void watch_folder();

    void clear_watchlist();
    void add_file_to_watchlist(QString file_path);
    void remove_file_from_watchlist(QString file_path, bool dir);

};

#endif // MAINWINDOW_H
