#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "QList"
#include <QFileDialog>
#include <QtMultimedia/QMediaPlayer>
#include <QAudioOutput>



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    // ui stuff
    ui->setupUi(this);
    setFixedSize(size());
    original_flags = this->windowFlags();
    ui->lb_update->setVisible(false);

    // check if the file_parser is good
    file_parser = new class file_parser();
    if (!file_parser->loaded()) {
        ui->lb_status->setText("Could not create file_parser, app disabled, please exit!");
        return;
    }

    // get data from file or default
    file_parser->fill_tp_list(tp_list);
    song_name = file_parser->get_value(file_parser->SNAME_KEY);
    song_path = file_parser->get_value(file_parser->SPATH_KEY);
    work_min  = std::chrono::minutes(std::stoi(file_parser->get_value(file_parser->WORK_KEY)));
    break_min = std::chrono::minutes(std::stoi(file_parser->get_value(file_parser->BREAK_KEY)));
    ui->lb_song->setText(QString::fromStdWString(song_name));

    // Set default work/break minutes
    QDateTime mins; mins.setSecsSinceEpoch(60 * work_min.count());
    ui->tm_work->setDateTime(mins); mins.setSecsSinceEpoch(60 * break_min.count());
    ui->tm_break->setDateTime(mins);

    // Set the table model
    ui->tbv_proc_list->setModel(&table_model);
    table_model.setHorizontalHeaderItem(0, new QStandardItem("Name"));
    ui->tbv_proc_list->setColumnWidth(0, ui->tbv_proc_list->size().width());
    build_process_table();

    // Connect the buttons
    connect(ui->btn_add_tp,         &QPushButton::clicked, this, &MainWindow::add_tp);
    connect(ui->btn_del_tp,         &QPushButton::clicked, this, &MainWindow::del_tp);
    connect(ui->btn_start,          &QPushButton::clicked, this, &MainWindow::change_app_state);
    connect(ui->btn_update,         &QPushButton::clicked, this, &MainWindow::update_minutes);
    connect(ui->btn_change_song,    &QPushButton::clicked, this, &MainWindow::select_song);
}


// Re-building the table from the ground
// todo try not to delete and rewrite every item if not necessary
void MainWindow::build_process_table() {
    table_model.removeRows(0, table_model.rowCount());

    for (const auto& item : tp_list.get_const_tp_ref()) {
        row_data << new QStandardItem(QString::fromStdWString(item.name));

        table_model.appendRow(row_data);
        row_data.clear();
    }
}

// Check if the string is valid and add it to the tp_list
void MainWindow::add_tp() {
    if (ui->tb_add_tp->toPlainText().trimmed() == "") {
        ui->lb_status->setText("Process name must not be empty");
        return;
    }

    // check if it has ".exe", if not, add it
    timed_process to_add; to_add.name = ui->tb_add_tp->toPlainText().toLower().toStdWString();
    size_t dot_idx = to_add.name.find_last_of('.');
    if (dot_idx == std::wstring::npos) {
        to_add.name += L".exe";
    }

    if (tp_list.add_timed_process(to_add)) {
        ui->tb_add_tp->clear();
        build_process_table();
        file_parser->update_plist(tp_list.get_const_tp_ref());
    } else {
        ui->lb_status->setText("Process already exists");
    }
}

// Delete processes from the tp_list by name
void MainWindow::del_tp() {
    auto selection = ui->tbv_proc_list->selectionModel()->selectedRows();
    if (selection.count() == 0) return;

    timed_process to_remove;
    for (size_t i=0; i < selection.count(); i++)
    {
        auto item = table_model.data(selection.at(i));
        to_remove.name = item.toString().toStdWString();
        tp_list.remove_timed_process(to_remove);
    }

    file_parser->update_plist(tp_list.get_const_tp_ref());
    build_process_table();
}

// Handles the start button actions
void MainWindow::change_app_state() {
    switch (app_state) {
    case APP_WAITING:
        resume_tp_thread();
        break;

    case APP_RUNNING:
        pause_tp_thread();
        break;

    case APP_PAUSED:
        resume_tp_thread();
        break;
    }
}

void MainWindow::update_minutes() {
    work_min    = std::chrono::minutes(ui->tm_work->dateTime().time().minute());
    break_min   = std::chrono::minutes(ui->tm_break->dateTime().time().minute());

    file_parser->update(file_parser->WORK_KEY,  std::to_wstring(work_min.count()));
    file_parser->update(file_parser->BREAK_KEY, std::to_wstring(break_min.count()));

    ui->lb_update->setVisible(true);
    std::thread hide_thread([&](){
        std::this_thread::sleep_for(std::chrono::seconds(3));
        ui->lb_update->setVisible(false);
    }); hide_thread.detach();
}

void MainWindow::select_song() {
    auto path{QFileDialog::getOpenFileName(
        nullptr,
        QObject::tr("Open song"),
        QDir::currentPath(),
        QObject::tr("MP3 music files (*.mp3);;WAV music files (*.wav)"))};

    if (!path.isNull()) {
        song_path = path.toStdWString();
        song_name = QString(path.split("/").last()).toStdWString();

        file_parser->update(file_parser->SNAME_KEY, song_name);
        file_parser->update(file_parser->SPATH_KEY, song_path);

        ui->lb_song->setText(QString::fromStdWString(song_name));
    }
}

// Creates a new thread to resume the work
void MainWindow::resume_tp_thread() {
    app_state = APP_RUNNING;
    tp_running = true;

    // clear window flags
    setWindowFlags(original_flags);

    ui->btn_start->setText("Running");

    // Process the timed processes in another thread
    auto tp_lambda = [&]() {
        const auto cwork_min = work_min, cbreak_min = break_min;
        ui->btn_add_tp->setEnabled(false);
        ui->btn_del_tp->setEnabled(false);
        ui->btn_change_song->setEnabled(false);
        ui->btn_update->setEnabled(false);

        while (tp_running) {
            switch (tp_list.update_tp_list()) { default: break; } // todo, it does not get every process!

            switch (tp_list.manage_tp_break(cwork_min, cbreak_min, song_path)) {
            case INVALID_INPUT_BLOCK: {
                ui->lb_status->setText("Could not block input, check if app is elevated");
                setWindowFlags(original_flags | Qt::WindowStaysOnTopHint);
                show();
                pause_tp_thread();
                break;
            }
            case INVALID_INPUT_UNBLOCK: {
                ui->lb_status->setText("Could not un-block input, use CTRL+ALT+DEL to forcefully un-block");
                setWindowFlags(original_flags | Qt::WindowStaysOnTopHint);
                show();
                pause_tp_thread();
                break;
            }

            default: break;
            }

            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    };

    tp_thread = std::thread(tp_lambda);
    tp_thread.detach();
}

// Kills the thread using the atomic bool
void MainWindow::pause_tp_thread() {
    app_state = APP_PAUSED;
    tp_running = false;

    ui->btn_add_tp->setEnabled(true);
    ui->btn_del_tp->setEnabled(true);
    ui->btn_change_song->setEnabled(true);
    ui->btn_update->setEnabled(true);

    ui->btn_start->setText("Paused");
}


MainWindow::~MainWindow()
{
    file_parser->write_file();
    delete file_parser;
    delete ui;
}
