#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QFileDialog>
#include <QDir>
#include <QFileInfoList>
#include <QFileInfo>
#include <iostream>
#include <QList>
#include <QString>

#ifdef Q_OS_LINUX
#define DEFAULT_SELECTED_FOLDER "/home/ed/git/Test_Folder"
#elif Q_OS_WIN
#define DEFAULT_SELECTED_FOLDER "C:\\"
#elif Q_OS_MAC
#define DEFAULT_SELECTED_FOLDER "/home"
#endif

#define SCANNING_TIMER_INTERVAL (1000)

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    model =  new QStandardItemModel(0, 0, this);

    ui->tableview->setModel(model);

    QString default_folder = DEFAULT_SELECTED_FOLDER;
    ui->lineedit_folder_path->setText(default_folder);

    add_headers_to_model();

    // stretch header widths
    QHeaderView * header_view = ui->tableview->horizontalHeader();
    header_view->setSectionResizeMode(QHeaderView::Stretch);

    watch_folder_dir.setFilter(QDir::Files | QDir::Hidden); // show hidden files and no directories

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::refresh()
{

    QFileInfoList file_info_list = watch_folder_dir.entryInfoList();

    clear();

    // add files to model
    foreach (QFileInfo file_info, file_info_list)
    {
        FileRecord file;

        file.filename = file_info.fileName();
        file.size = file_info.size();

        QStandardItem * item_file_name = new QStandardItem(0,0);
        QStandardItem * item_file_size = new QStandardItem(0,1);

        item_file_name->setText(file.filename);
        item_file_size->setText(QString::number(file.size));

        QList<QStandardItem *> items;
        items.append(item_file_name);
        items.append(item_file_size);

        model->appendRow(items);
    }

    // count files and sizes
    int count = 0;
    int total_file_sizes = 0;
    foreach (QFileInfo file_info, file_info_list)
    {
        count++;
        total_file_sizes += file_info.size();
    }

    ui->label_total_files->setText(QString::number(count));
    ui->label_total_size->setText(QString::number(total_file_sizes));

}

void MainWindow::on_button_browse_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select Directory"),
                                                 DEFAULT_SELECTED_FOLDER,
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);

    ui->lineedit_folder_path->setText(dir);

    watch_folder();

}

void MainWindow::on_button_watch_clicked()
{
    watch_folder();
}

void MainWindow::add_headers_to_model()
{
    QList<QString> headers;
    headers.append("File Name");
    headers.append("File Size");

    model->setHorizontalHeaderLabels(headers);
}

void MainWindow::watch_folder()
{
    // set path and dir
    watch_folder_path = ui->lineedit_folder_path->text();
    watch_folder_dir.setPath(watch_folder_path);

    // UI Feedback
    QString message = "";
    QString message_color;

    if (watch_folder_dir.exists())
    {
        message = "Watching " + watch_folder_path;
        message_color = "QLabel { color : black; }";
    }
    else
    {
        message = "Folder does not exist!";
        message_color = "QLabel { color : red; }";
    }

    ui->label_message->setText(message);
    ui->label_message->setStyleSheet(message_color);

    refresh();
}

void MainWindow::clear()
{
    model->clear();

    add_headers_to_model();
}

