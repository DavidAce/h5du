#pragma once
#include <map>
#include <meta/groupmeta.h>
#include <string>

namespace h5pp {
    class File;
}

namespace tools::tree {
    struct byteResult_t {
        size_t dsetByte = 0;
        size_t dsetStrg = 0;
        size_t attrByte = 0;
        size_t attrStrg = 0;
        size_t linkByte = 0;
        size_t linkStrg = 0;
    };

    byteResult_t collectBytes(std::map<std::string, GroupMeta> &groupTree, const std::string &root);

    std::vector<std::pair<std::string, GroupMeta>> sort(std::map<std::string, GroupMeta> &groupTree, SortKey key = SortKey::SIZE,
                                                        SortOrder order = SortOrder::DESCENDING, SortType type = SortType::DSET);

    size_t getMaxPathSize(const std::vector<std::pair<std::string, GroupMeta>> groupTreeSorted, long filterDepth, bool printDset, bool printAttr);

}