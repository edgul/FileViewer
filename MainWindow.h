#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
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

private slots:
    void on_button_browse_clicked();
    void on_button_watch_clicked();

    void apply_directory_changes();
    void file_changed(QString file_path);

private:
    Ui::MainWindow * ui;

    TableModel * table_model;

    QString watch_folder_path;
    QDir watch_folder_dir;

    void watch_folder();

    QList<QString> all_file_paths_in_dir();

};

#endif // MAINWINDOW_H
