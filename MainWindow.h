#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include "FileRecord.h"

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

    void clear();

private slots:
    void on_button_browse_clicked();
    void on_button_refresh_clicked();

private:
    Ui::MainWindow *ui;

    QStandardItemModel * model;

    QList<FileRecord> files;

    void add_headers_to_model();

};

#endif // MAINWINDOW_H
