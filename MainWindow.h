#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include "FileRecord.h"
#include <QFileSystemWatcher>
#include <QDir>

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

private slots:
    void on_button_browse_clicked();
    void on_button_watch_clicked();

    void directory_changed(QString dir_path);
    void file_changed(QString file_path);

private:
    Ui::MainWindow * ui;

    // TODO: improve model -- currently doesn't update view correctly
    QStandardItemModel * model;

    QList<FileRecord> files;

    QFileSystemWatcher file_system_watcher;

    QString watch_folder_path;
    QDir watch_folder_dir;

    void add_headers_to_model();

    void watch_folder();

    void clear();

    void clear_watchlist();
    void add_file_to_watchlist(QString file_path);
    void remove_file_from_watchlist(QString file_path);

};

#endif // MAINWINDOW_H
