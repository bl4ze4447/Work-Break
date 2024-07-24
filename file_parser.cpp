#include "file_parser.h"

#include <cstring>
#include <fstream>
#include <filesystem>
#include <cstdlib>
#include <cstdio>

file_parser::file_parser(const wchar_t dlm) {
    // initialize it here so we can free it in case of an exception
    char * letter{nullptr};
    bool dupped{false};

    delimiter = dlm;

    try {
        letter = std::getenv("SystemDrive");
        if (letter == nullptr) {
            letter = strdup("C:");
            dupped = true;
        }

        path = std::string(letter) + "\\ProgramData\\Work-Break";
        if (dupped) free(letter);

        std::filesystem::create_directories(path);
    } catch (...) {
        exception_thrown = true;
        return;
    }

    if (!std::filesystem::exists(path + "\\conf.ini")) {
        load_defaults();
        write_file();
        return;
    }

    std::wifstream fin(path + "\\conf.ini");
    fin.imbue(std::locale(fin.getloc(), new std::codecvt_utf8<wchar_t>));

    std::wstring line, key, value;
    size_t delimiter_pos;

    if (!std::getline(fin, line)) {
        load_defaults();
        write_file();
        return;
    }

    do {
        delimiter_pos = line.find_first_of(delimiter);
        if (delimiter_pos == std::wstring::npos) {
            // repair file
            load_defaults();
            write_file();
            break;
        }

        key = line.substr(0, delimiter_pos);
        TRIM_STR(key);
        value = line.substr(delimiter_pos+1);
        TRIM_STR(value);

        if (key != PLIST_KEY &&
            key != SNAME_KEY &&
            key != SPATH_KEY &&
            key != WORK_KEY  &&
            key != BREAK_KEY) {

            // repair file
            load_defaults();
            write_file();
            break;
        }

        // test if it is a number
        if (key == WORK_KEY || key == BREAK_KEY) {
            try {
                std::stoi(value);
            } catch (...) {
                load_defaults();
                write_file();
                break;
            }
        }

        parser_map[key] = value;
    } while (std::getline(fin, line));

    fin.close();
}
bool file_parser::loaded() { return !exception_thrown; }

std::wstring file_parser::get_value(const std::wstring& key) {
    return parser_map[key];
}
void file_parser::fill_tp_list(tp_list& tp_list) {
    std::wstringstream wss(parser_map[PLIST_KEY]);
    std::wstring word;
    while (std::getline(wss, word, L',')) {
        TRIM_STR(word);
        if (word.empty()) continue;

        tp_list.add_timed_process_by_name(word);
    }
}

bool file_parser::update(const std::wstring& key, const std::wstring& value) {
    if (key == WORK_KEY || key == BREAK_KEY) {
        try {
            std::stoi(value);
        } catch (...) {
            return false;
        }
    }

    parser_map[key] = value;
    return true;
}
void file_parser::update_plist(const std::vector<timed_process>& tp_vec) {
    parser_map[PLIST_KEY].clear();
    for (const auto& tp : tp_vec) {
        parser_map[PLIST_KEY] = parser_map[PLIST_KEY] + tp.name + L",";
    }
}

void file_parser::write_file() {
    std::wofstream fout(path + "\\conf.ini");
    fout.imbue(std::locale(fout.getloc(), new std::codecvt_utf8<wchar_t>));

    fout << PLIST_KEY << L' ' << delimiter << L' ' << parser_map[PLIST_KEY] << L'\n'
         << SNAME_KEY << L' ' << delimiter << L' ' << parser_map[SNAME_KEY] << L'\n'
         << SPATH_KEY << L' ' << delimiter << L' ' << parser_map[SPATH_KEY] << L'\n'
         << WORK_KEY  << L' ' << delimiter << L' ' << parser_map[WORK_KEY]  << L'\n'
         << BREAK_KEY << L' ' << delimiter << L' ' << parser_map[BREAK_KEY] << L'\n';

    fout.close();
}

void file_parser::load_defaults() {
    parser_map[PLIST_KEY]     = L"random_process.exe, space_before_process_comma.exe, unicode_with_space_after_✅.exe ,";
    parser_map[SNAME_KEY]     = L"none";
    parser_map[SPATH_KEY]     = L"none";
    parser_map[WORK_KEY]      = L"45";
    parser_map[BREAK_KEY]     = L"15";
}
