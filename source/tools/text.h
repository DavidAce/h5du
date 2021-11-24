#pragma once

#include <string_view>
#include <string>
namespace text {
    extern bool endsWith(std::string_view str, std::string_view suffix);
    extern bool startsWith(std::string_view str, std::string_view prefix);
    extern std::string replace(std::string_view str, std::string_view from, std::string_view to);
    extern std::string prepend(std::string_view str, std::string_view pre);
    extern void removeSubstring(std::string &s, std::string_view p);
    extern void clipHead(std::string &s, std::string_view p);
    extern void clipTail(std::string &s, std::string_view p);
}
