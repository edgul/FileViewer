#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QFileDialog>
#include <QDir>
#include <QFileInfoList>
#include <QFileInfo>
#include <QList>
#include <QString>
#include "PrintHelper.h"

#ifdef Q_OS_LINUX
#define DEFAULT_SELECTED_FOLDER "/home/ed/git/Test_Folder"
#endif

#ifdef Q_OS_WIN
#define DEFAULT_SELECTED_FOLDER "C:\\TestFolder"
#endif

#ifdef Q_OS_MAC
#define DEFAULT_SELECTED_FOLDER "/home"
#endif

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    table_model =  new TableModel(this);
    ui->tableview->setModel(table_model);
    connect(table_model, SIGNAL(directory_changed()), this, SLOT(apply_directory_changes()));
    connect(table_model, SIGNAL(file_changed()), this, SLOT(file_changed()));

    QString default_folder = DEFAULT_SELECTED_FOLDER;
    ui->lineedit_folder_path->setText(default_folder);

    // stretch header widths
    QHeaderView * header_view = ui->tableview->horizontalHeader();
    header_view->setSectionResizeMode(QHeaderView::Stretch);

    watch_folder_dir.setFilter(QDir::Files | QDir::Hidden); // show hidden files and no directories
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::update_totals()
{
    // total file size
    int total_file_size = table_model->total_file_size();
    ui->label_total_size->setText(QString::number(total_file_size));

    // total num files
    int num_files = table_model->rowCount();
    ui->label_total_files->setText(QString::number(num_files));

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

void MainWindow::apply_directory_changes()
{
    QList<QString> files_under_watch = table_model->files_under_watch();

    PrintHelper::print("\nFiles Under watch:");
    PrintHelper::print(files_under_watch);

    QList<QString> all_file_in_dir_paths = all_file_paths_in_dir();

    PrintHelper::print("\nFiles currently in dir:");
    PrintHelper::print(all_file_in_dir_paths);

    // Add new files to watch list
    QList<QString> files_to_add;
    foreach (QString existing_file, all_file_in_dir_paths)
    {
        if (!files_under_watch.contains(existing_file))
        {
            files_to_add.append(existing_file);
        }
    }

    foreach(QString file_to_add, files_to_add)
    {
        table_model->add_file(file_to_add);
    }

    // Remove deleted files from watchlist
    QList<QString> files_to_remove;
    foreach (QString watched_file, files_under_watch)
    {
        if (!all_file_in_dir_paths.contains(watched_file))
        {
            files_to_remove.append(watched_file);
        }
    }

    foreach (QString file_to_remove, files_to_remove)
    {
        table_model->remove_file(file_to_remove);
    }

    update_totals();
}

void MainWindow::file_changed(QString file_path)
{
    Q_UNUSED(file_path);

    update_totals();
}

void MainWindow::watch_folder()
{
    table_model->clear();

    // set path and dir
    watch_folder_path = ui->lineedit_folder_path->text();

    watch_folder_dir.setPath(watch_folder_path);
    watch_folder_dir.refresh();

    bool success = table_model->add_dir(watch_folder_path); //  directoryChanged() should fire on success

    // UI Feedback
    QString message = "";
    QString message_color;

    if (success)
    {
        message = "Watching " + watch_folder_path;
        message_color = "QLabel { color : black; }";

        PrintHelper::print("Watch Clicked");

        // Add files to watchlist
        QFileInfoList file_infos = watch_folder_dir.entryInfoList();
        foreach (QFileInfo file_info, file_infos)
        {
            table_model->add_file(file_info.filePath());
        }
    }
    else
    {
        message = "Failed to add path. ";
        message_color = "QLabel { color : red; }";

        if (!watch_folder_dir.exists())
        {
            message += "Folder does not exist.";
        }
        else
        {
            message += "System may not have enough resources";
        }
    }

    ui->label_message->setText(message);
    ui->label_message->setStyleSheet(message_color);

    update_totals();
}

QList<QString> MainWindow::all_file_paths_in_dir()
{
    watch_folder_dir.refresh(); // make sure dir is fresh
    QList<QFileInfo> all_files_in_dir = watch_folder_dir.entryInfoList();

    QList<QString> all_file_in_dir_paths;
    foreach (QFileInfo file_info, all_files_in_dir)
    {
        all_file_in_dir_paths.append( file_info.filePath() );
    }

    return all_file_in_dir_paths;
}
