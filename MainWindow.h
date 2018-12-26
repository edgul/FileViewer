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

    void refresh();

private slots:
    void on_button_browse_clicked();
    void on_button_watch_clicked();

private:
    Ui::MainWindow * ui;

    QStandardItemModel * model;

    QList<FileRecord> files;

    QFileSystemWatcher file_system_watcher;

    QString watch_folder_path;
    QDir watch_folder_dir;

    void add_headers_to_model();

    void watch_folder();

    void clear();




};

#endif // MAINWINDOW_H
