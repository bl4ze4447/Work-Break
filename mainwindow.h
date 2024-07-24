#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <thread>

#include "tp_list.h"
#include "file_parser.h"

#define APP_WAITING 0
#define APP_PAUSED 1
#define APP_RUNNING 2

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private slots:
    void add_tp();
    void del_tp();
    void change_app_state();
    void update_minutes();
    void select_song();
private:
    tp_list tp_list{};
    file_parser * file_parser{nullptr};

    short int app_state{APP_WAITING};
    double tbv_half_width{};

    std::thread tp_thread{};
    std::atomic_bool tp_running{false};

    std::wstring song_name{};
    std::wstring song_path{};

    std::chrono::minutes work_min{std::chrono::minutes(45)};
    std::chrono::minutes break_min{std::chrono::minutes(15)};

    Ui::MainWindow *ui;
    QStandardItemModel table_model{};
    QList<QStandardItem*> row_data{};
    Qt::WindowFlags original_flags{};

    void build_process_table();
    void resume_tp_thread();
    void pause_tp_thread();
};
#endif // MAINWINDOW_H
