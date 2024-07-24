#include "tp_list.h"

#ifdef USING_QT_MULTIMEDIA
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QEventLoop>
#include <QTimer>
#else
#include <thread>
#endif

#include <bits/ranges_algo.h>
#include <windows.h>
#include <cwctype>
#include <algorithm>

#ifdef USING_QT_MULTIMEDIA
void play_sound_and_wait(const std::wstring& song_path, const std::chrono::minutes& break_minutes) {
    QMediaPlayer * player{nullptr};
    QAudioOutput * audio_output{nullptr};
    if (!song_path.empty()) {
        player = new QMediaPlayer;
        audio_output = new QAudioOutput;

        audio_output->setVolume(0.3);
        player->setAudioOutput(audio_output);
        player->setSource(QUrl::fromLocalFile(QString::fromStdWString(song_path)));

        QObject::connect(player, &QMediaPlayer::mediaStatusChanged, [player](QMediaPlayer::MediaStatus status){
            if (status == QMediaPlayer::EndOfMedia) {
                player->setPosition(0);  // Restart the media from the beginning
                player->play();
            }
        });

        player->play();
    }

    QEventLoop loop;
    QTimer timer;
    timer.setSingleShot(true);

    QObject::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    timer.start(static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(break_minutes).count()));

    loop.exec();
    delete player;
    delete audio_output;
}
#else
void play_sound_and_wait(const std::wstring& song_path, const std::chrono::minutes& break_minutes) {
    if (!song_path.empty()) PlaySoundW(song_path.c_str(), nullptr, SND_LOOP | SND_ASYNC);
    std::this_thread::sleep_for(break_minutes);
    PlaySoundW(nullptr, nullptr, SND_LOOP | SND_ASYNC);
}
#endif

bool tp_list::tp_exists_by_name(const timed_process& proc) const {
    return std::ranges::any_of(tp.cbegin(), tp.cend(),
                               [&proc](const auto& p) { return p.name == proc.name; });
}

bool tp_list::add_timed_process(const timed_process& proc) {
    if (tp_exists_by_name(proc)) return false;

    tp.push_back(proc);
    return true;
}
bool tp_list::add_timed_process_by_name(const std::wstring& name) {
    timed_process tp; tp.name = name;
    return add_timed_process(tp);
}

//todo make only by name
void tp_list::remove_timed_process(const timed_process& proc) {
    if (const auto& it{std::find_if(tp.cbegin(), tp.cend(), [&proc](const timed_process& x){ return x.name == proc.name; })}; it != tp.end()) tp.erase(it);
}

Code tp_list::update_tp_list() {
    proc_entry = {};
    proc_entry.dwSize = sizeof(PROCESSENTRY32W);

    snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) {
        return INVALID_SNAPSHOT;
    }

    if (Process32FirstW(snapshot, &proc_entry) == FALSE) {
        CloseHandle(snapshot);
        return OK;
    }

    std::wstring proc_name;
    while (Process32NextW(snapshot, &proc_entry)) {
        proc_name = proc_entry.szExeFile;
        std::transform(proc_name.begin(), proc_name.end(), proc_name.begin(), std::towlower);

        for (auto& [seconds, pid, name] : tp) {
            if (name == proc_name) {
                ++seconds;

                if (pid == 0) pid = proc_entry.th32ProcessID;
                else if (pid != proc_entry.th32ProcessID) {
                    pid = proc_entry.th32ProcessID;
                    seconds = std::chrono::seconds(0);
                }
            }
        }
    }

    CloseHandle(snapshot);
    return OK;
}

Code tp_list::manage_tp_break(const std::chrono::minutes& work_minutes,
                              const std::chrono::minutes& break_minutes, const std::wstring& song_path) {

    const size_t len{tp.size()};
    for (size_t i{0}; auto& [seconds, pid, name] : tp) {
        if (seconds >= std::chrono::seconds(work_minutes)) {
            while (i < len) tp[i++].seconds = std::chrono::seconds(0);

            if (!BlockInput(true)) return INVALID_INPUT_BLOCK;
            play_sound_and_wait(song_path, break_minutes);
            if (!BlockInput(false)) return INVALID_INPUT_UNBLOCK;

            break;
        }
    }

    return OK;
}


