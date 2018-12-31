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

    QString default_folder = DEFAULT_SELECTED_FOLDER;
    ui->lineedit_folder_path->setText(default_folder);

    // stretch header widths
    QHeaderView * header_view = ui->tableview->horizontalHeader();
    header_view->setSectionResizeMode(QHeaderView::Stretch);

    reset_watch_folder_dir();

    connect(&file_system_watcher_dir, SIGNAL(directoryChanged(QString)), this, SLOT(directory_changed(QString)));
    connect(&file_system_watcher_files, SIGNAL(fileChanged(QString)), this, SLOT(file_changed(QString)));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::update_totals()
{
    reset_watch_folder_dir(); // need fresh dir so entryInfoList up to date
    QFileInfoList file_info_list = watch_folder_dir.entryInfoList();

    // count file sizes
    int total_file_sizes = 0;
    foreach (QFileInfo file_info, file_info_list)
    {
        total_file_sizes += file_info.size();
    }

    ui->label_total_size->setText(QString::number(total_file_sizes));

    // num files
    uint num_files = watch_folder_dir.count();
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

void MainWindow::reset_watch_folder_dir()
{
    watch_folder_dir = QDir(); // need to remake the qdir so that latest file can be seen
    watch_folder_dir.setFilter(QDir::Files | QDir::Hidden); // show hidden files and no directories
    watch_folder_dir.setPath(watch_folder_path);
}

void MainWindow::directory_changed(QString dir_path)
{
    Q_UNUSED(dir_path);

    QList<QString> files_under_watch = file_system_watcher_files.files();

    PrintHelper::print("\nFiles Under watch:");
    PrintHelper::print(files_under_watch);

    reset_watch_folder_dir();

    QList<QFileInfo> all_files_in_dir = watch_folder_dir.entryInfoList();

    QList<QString> all_file_in_dir_paths;

    foreach (QFileInfo file_info, all_files_in_dir)
    {
        all_file_in_dir_paths.append( file_info.filePath() );
    }

    PrintHelper::print("\nFiles currently in dir:");
    PrintHelper::print(all_file_in_dir_paths);

    // Add new files to watch list
    QList<QString> added_files;
    foreach (QString existing_file, all_file_in_dir_paths)
    {
        if (!files_under_watch.contains(existing_file))
        {
            added_files.append(existing_file);
        }
    }

    foreach( QString added_file, added_files)
    {
        add_file_to_watchlist(added_file);
    }

    // Remove deleted files from watchlist
    QList<QString> removed_files;
    foreach (QString watched_file, files_under_watch)
    {
        if (!all_file_in_dir_paths.contains(watched_file))
        {
            removed_files.append(watched_file);
        }
    }

    foreach (QString removed_file, removed_files)
    {
        remove_file_from_watchlist(removed_file, false);
    }

    update_totals();

}

void MainWindow::file_changed(QString file_path)
{
    QList<QString> files_under_watch = file_system_watcher_files.files();

    QFileInfo file_info(file_path);

    table_model->update(file_path, file_info.size());

    update_totals();
}


void MainWindow::watch_folder()
{
    clear_watchlist();

    // set path and dir
    watch_folder_path = ui->lineedit_folder_path->text();
    reset_watch_folder_dir();

    bool success = file_system_watcher_dir.addPath(watch_folder_path); //  directoryChanged() should fire on success

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
            add_file_to_watchlist(file_info.filePath());
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

void MainWindow::clear_watchlist()
{
    QList<QString> dirs_under_watch = file_system_watcher_dir.directories();
    foreach (QString dir, dirs_under_watch)
    {
        remove_file_from_watchlist(dir, true);
    }

    QList<QString> files_under_watch = file_system_watcher_files.files();
    foreach (QString file, files_under_watch)
    {
        remove_file_from_watchlist(file, false);
    }

    table_model->clear_files();
}

void MainWindow::add_file_to_watchlist(QString file_path)
{
    bool success = file_system_watcher_files.addPath(file_path);

    QString msg = "" ;

    if (success)
    {
        msg = QString("Added to watchlist: %1").arg(file_path);
    }
    else
    {
        msg = QString("Failed to add to watchlist: %1").arg(file_path);
    }
    PrintHelper::print(msg);

    QFileInfo file_info(file_path);

    FileRecord * file_record = new FileRecord();
    file_record->abs_path = file_info.absoluteFilePath();
    file_record->size = file_info.size();

    table_model->add_record(file_record);

}

void MainWindow::remove_file_from_watchlist(QString file_path, bool dir)
{
    // watcher will remove deleted files automatically

    // update model
    table_model->remove_record(file_path);

}
