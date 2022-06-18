#include "text.h"

bool text::endsWith(std::string_view str, std::string_view suffix) {
    return str.size() >= suffix.size() and 0 == str.compare(str.size() - suffix.size(), suffix.size(), suffix);
}

bool text::startsWith(std::string_view str, std::string_view prefix) { return str.size() >= prefix.size() and 0 == str.compare(0, prefix.size(), prefix); }

std::string text::replace(std::string_view str, std::string_view from, std::string_view to) {
    std::string res(str);
    if(from.empty()) return res;
    size_t start_pos = 0;
    while((start_pos = res.find(from, start_pos)) != std::string::npos) {
        res.replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
    return res;
}

std::string text::prepend(std::string_view str, std::string_view pre){
    return std::string(pre) + std::string(str);
}

void text::removeSubstring(std::string &s, std::string_view p) {
    std::string::size_type n = p.length();
    for(std::string::size_type i = s.find(p); i != std::string::npos; i = s.find(p)) s.erase(i, n);
}

void text::clipHead(std::string &s, std::string_view p) {
    if(s.size() > 1 and startsWith(s, p)) { s.erase(0, p.length()); }
}
void text::clipTail(std::string &s, std::string_view p) {
    if(s.size() > 1 and endsWith(s, p)) { s.erase(s.length() - p.length()); }
}