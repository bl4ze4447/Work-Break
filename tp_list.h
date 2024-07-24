#ifndef TIMED_PROCESS_H
#define TIMED_PROCESS_H

#define USING_QT_MULTIMEDIA

#include <bemapiset.h>
#include <chrono>
#include <cstdint>
#include <string>
#include <tlhelp32.h>
#include <vector>

#include "app_codes.h"

class timed_process {
public:
    std::chrono::seconds seconds{0};
    uint32_t pid{0};
    std::wstring name;

    bool operator==(const timed_process& other) const {
        return name == other.name && pid == other.pid &&
               seconds == other.seconds;
    }
};

class tp_list {
    std::vector<timed_process> tp{};

    HANDLE snapshot{};
    PROCESSENTRY32W proc_entry{};

    [[nodiscard]] bool tp_exists_by_name(const timed_process& proc) const;
public:
    explicit tp_list() = default;
    explicit tp_list(const std::vector<timed_process>& list) : tp(list) {}
    explicit tp_list(const timed_process& proc) { tp.push_back(proc); }
    explicit tp_list(const std::chrono::seconds& seconds, const uint32_t& pid,
                     const std::wstring& name) { tp.emplace_back(seconds, pid, name); }

    const std::vector<timed_process>& get_const_tp_ref() { return tp; }

    bool add_timed_process(const timed_process& proc);
    bool add_timed_process_by_name(const std::wstring& name);
    void remove_timed_process(const timed_process& proc);

    [[nodiscard]] Code update_tp_list();
    [[nodiscard]] Code manage_tp_break(const std::chrono::minutes& work_minutes,
                                       const std::chrono::minutes& break_minutes, const std::wstring& song_path = L"");
};

#endif //TIMED_PROCESS_H
