#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QFileDialog>
#include <QDir>
#include <QFileInfoList>
#include <QFileInfo>
#include <iostream>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::refresh()
{
    QString folder_path = ui->lineedit_folder_path->text();

    QDir folder_dir(folder_path);

    // Feedback for valid folder path
    QString message = "";
    QString message_color;

    if (folder_dir.exists())
    {
        message = "Displaying folder contents:";
        message_color = "QLabel { color : black; }";
    }
    else
    {
        message = "Folder does not exist!";
        message_color = "QLabel { color : red; }";
    }

    ui->label_message->setText(message);
    ui->label_message->setStyleSheet(message_color);


    folder_dir.setFilter(QDir::Files | QDir::Hidden);

    QFileInfoList file_info_list = folder_dir.entryInfoList();

    foreach (QFileInfo file_info, file_info_list)
    {
        std::cout << file_info.fileName().toStdString() << "\t";
        std::cout << file_info.size() << std::endl;
        std::flush(std::cout);
    }

}

void MainWindow::on_button_browse_clicked()
{
    // TODO: add windows and MAC support

    QString dir = QFileDialog::getExistingDirectory(this, tr("Select Directory"),
                                                 "/home/ed",
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);

    ui->lineedit_folder_path->setText(dir);

    // TODO: check how spaces in path/folder handled?

    refresh();

}

void MainWindow::on_button_refresh_clicked()
{
    refresh();
}
