#ifndef FILE_PARSER_H
#define FILE_PARSER_H

#include <string>
#include <unordered_map>
#include "tp_list.h"

class file_parser
{
public:
    // all possible keys for the parser_map
    const std::wstring PLIST_KEY = L"process_list";
    const std::wstring SNAME_KEY = L"song_name";
    const std::wstring SPATH_KEY = L"song_path";
    const std::wstring WORK_KEY  = L"work_min";
    const std::wstring BREAK_KEY = L"break_min";

    // directory path without filename
    std::string path;

    wchar_t delimiter{};

    file_parser(const wchar_t dlm = L'=');
    bool loaded();

    bool update(const std::wstring& key, const std::wstring& value);
    void update_plist(const std::vector<timed_process>& tp_vec);

    std::wstring get_value(const std::wstring& key);
    void fill_tp_list(tp_list& tp_vec);

    void write_file();
private:
    std::unordered_map<std::wstring, std::wstring> parser_map;

    void load_defaults();
    bool exception_thrown{false};

#define TRIM_STR(s) s.erase(s.find_last_not_of(L" \t\n\r\f\v") + 1); s.erase(0, s.find_first_not_of(L" \t\n\r\f\v"));
};

#endif // FILE_PARSER_H
