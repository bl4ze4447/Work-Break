#include <windows.h>
#include <tlhelp32.h>
#include <cinttypes>
#include <chrono>
#include <thread>
#include <iostream>
#include <fstream>
#include <vector>

struct TimedProcess {
    uint64_t seconds;
    std::string name;
    DWORD * pid;
};

auto BREAK_TIME_MINUTES = std::chrono::minutes(10);
auto WORK_TIME_MINUTES = std::chrono::minutes(50);

void GetFormattedTime(time_t& now, tm& tm_struct, char buf[9], const size_t& buf_size);
void PrintInfo(const std::string_view& message);
void UpdateTimedProcesses(const HWND& wnd_handle, PROCESSENTRY32& p_entry, std::vector<TimedProcess>& t_process);

DWORD HandleTimedProcesses(const HWND& wnd_handle, std::vector<TimedProcess>& t_process);
DWORD TerminateRunningProcesses(std::string proc_name);

int main(int argc, char** argv) {
    // Before doing anything at all, verify if the app is already running
    if (TerminateRunningProcesses(argv[0]) != 0) return 1;

    {
        std::ifstream file("settings.ini");
        std::string work_m, break_m;
        std::getline(file, work_m);
        std::getline(file, break_m);

        size_t pos{work_m.find_last_of('=')};
        if (pos != std::string::npos) WORK_TIME_MINUTES = std::chrono::minutes(std::stoi(work_m.substr(pos+1)));
        pos = break_m.find_last_of('=');
        if (pos != std::string::npos) BREAK_TIME_MINUTES = std::chrono::minutes(std::stoi(break_m.substr(pos+1)));
    }

    // Window related
    HWND wnd_handle = GetConsoleWindow();
    ShowWindow(wnd_handle, SW_HIDE);
    int return_value{0};

    // Productivty processes
    std::vector<TimedProcess> t_process{
        {0, "WINWORD.EXE", nullptr},
        {0, "POWERPNT.EXE", nullptr},
        {0, "EXCEL.EXE", nullptr},
        {0, "MSACCESS.EXE", nullptr}
    };

    // All data about the running processes will be stored here one by one
    PROCESSENTRY32 p_entry{};

    while (true) {
        // Reset
        p_entry = {};
        p_entry.dwSize = sizeof(PROCESSENTRY32);

        UpdateTimedProcesses(wnd_handle, p_entry, t_process);
        if (HandleTimedProcesses(wnd_handle, t_process) != 0) {
            for (auto& [_x, _y, pid] : t_process) {
                delete pid;
            }

            return_value = 1;
            break;
        }
    }

    return return_value;
}


// Func Implementation
void GetFormattedTime(time_t& now, tm& tm_struct, char buf[9], const size_t& buf_size) {
    now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    tm_struct = *localtime(&now);
    strftime(buf, buf_size, "%X", &tm_struct);
}
void PrintInfo(const std::string_view& message) {
    // Used for displaying time
    static tm _tm_struct{};
    static time_t _now{};
    static char _buf[9]{};

    GetFormattedTime(_now, _tm_struct, _buf, sizeof(_buf));
    std::cout << "[" << _buf << "] >> " << message << '\n';
}
void UpdateTimedProcesses(const HWND& wnd_handle, PROCESSENTRY32& p_entry, std::vector<TimedProcess>& t_process) {
    HANDLE snapshot{CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0)};

    if (snapshot == INVALID_HANDLE_VALUE) {
        ShowWindow(wnd_handle, SW_SHOW);
        SetWindowPos(wnd_handle, HWND_TOPMOST, -1, -1, -1, -1,
        SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);

        PrintInfo("Incorrect snapshot value in UpdateTimedProcesses, continuing in 10 seconds, report this to the developer: " + std::to_string(GetLastError()));
        std::this_thread::sleep_for(std::chrono::seconds(10));
        ShowWindow(wnd_handle, SW_HIDE);
        return;
    }

    if (Process32First(snapshot, &p_entry) == FALSE) {
        CloseHandle(snapshot);
        return;
    };

    // Increment the second timer or reset it and set the PID, all if necessary
    while (Process32Next(snapshot, &p_entry) == TRUE) {
        for (auto& [seconds, name, pid] : t_process) {
            if (name == p_entry.szExeFile) {
                if (pid == nullptr) {
                    pid = new DWORD;
                    *pid = p_entry.th32ProcessID;
                }

                ++seconds;

                if (*pid != p_entry.th32ProcessID) {
                    seconds = 0;
                    *pid = p_entry.th32ProcessID;
                }
            }
        }
    }

    CloseHandle(snapshot);
}

DWORD HandleTimedProcesses(const HWND& wnd_handle, std::vector<TimedProcess>& t_process) {
    for (bool reset = false; auto& [seconds, name, pid] : t_process) {
        if (reset) {
            seconds = 0;
            continue;
        }

        if (std::chrono::seconds(seconds) == WORK_TIME_MINUTES) {
            seconds = 0;
            reset = true;

            // Bring window on top of all of the other
            ShowWindow(wnd_handle, SW_SHOW);
            SetWindowPos(wnd_handle, HWND_TOPMOST, -1, -1, -1, -1,
            SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);

            PrintInfo("Taking a break for " + std::to_string(BREAK_TIME_MINUTES.count()) + " minutes. (\"" + name + "\")");

            if (!BlockInput(true)) {
                PrintInfo("Could not block input, app closing in 2 minutes, report this to the developer: " + std::to_string(GetLastError()));

                std::this_thread::sleep_for(std::chrono::minutes(2));
                return 1;
            }

            PlaySoundA(TEXT("BreakSound.wav"), nullptr, SND_LOOP | SND_ASYNC);
            std::this_thread::sleep_for(BREAK_TIME_MINUTES);
            PlaySoundA(nullptr, nullptr, SND_LOOP | SND_ASYNC);

            if (!BlockInput(false)) {
                PrintInfo("Could not block input, app closing in 2 minutes, report this to the developer: " + std::to_string(GetLastError()));

                std::this_thread::sleep_for(std::chrono::minutes(2));
                return 1;
            }

            PrintInfo("Break is over.");
            std::this_thread::sleep_for(std::chrono::seconds(3));
            ShowWindow(wnd_handle, SW_HIDE);
        }
    }

    std::this_thread::sleep_for(std::chrono::seconds(1));
    return 0;
}
DWORD TerminateRunningProcesses(std::string proc_name) {
    if (const size_t idx = proc_name.find_last_of('\\'); idx != std::string::npos) proc_name = proc_name.substr(idx + 1);
    const DWORD proc_pid = GetCurrentProcessId();
    PROCESSENTRY32 p_entry{}; p_entry.dwSize = sizeof(PROCESSENTRY32);

    HANDLE snapshot{CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0)};
    if (snapshot == INVALID_HANDLE_VALUE) {
        PrintInfo("Incorrect snapshot value in TerminateRunningProcesses, closing in 2 minutes, report this to the developer: " + std::to_string(GetLastError()));
        std::this_thread::sleep_for(std::chrono::minutes(2));

        return 1;
    }

    if (HANDLE proc; Process32First(snapshot, &p_entry) == TRUE) {
        while (Process32Next(snapshot, &p_entry) == TRUE) {
            if (proc_pid != p_entry.th32ProcessID && p_entry.szExeFile == proc_name) {
                proc = OpenProcess(PROCESS_TERMINATE, FALSE, p_entry.th32ProcessID);
                if (proc == nullptr) {
                    PrintInfo("Incorrect proc value in TerminateRunningProcesses, closing in 2 minutes, report this to the developer: " + std::to_string(GetLastError()));
                    CloseHandle(snapshot);

                    std::this_thread::sleep_for(std::chrono::minutes(2));
                    return 1;
                }

                if (!TerminateProcess(proc, 0)) {
                    PrintInfo("The app is already opened but couldn't be terminated, closing in 2 minutes, report this to the developer: " + std::to_string(GetLastError()));
                    CloseHandle(proc);
                    CloseHandle(snapshot);

                    std::this_thread::sleep_for(std::chrono::minutes(2));
                    return 1;
                }

                CloseHandle(proc);
            }
        }
    }

    CloseHandle(snapshot);

    return 0;
}