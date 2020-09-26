#pragma once
#include <map>
#include <meta/groupmeta.h>
#include <string>

namespace h5pp {
    class File;
}

namespace tools::tree {
    std::tuple<size_t, size_t, size_t, size_t> collectBytes(std::map<std::string, GroupMeta> &groupTree, const std::string &root);
    std::vector<std::pair<std::string,GroupMeta>> sort(std::map<std::string, GroupMeta> &groupTree, SortKey key = SortKey::SIZE, SortOrder order = SortOrder::DESCENDING,
              SortType type = SortType::DSET);
}