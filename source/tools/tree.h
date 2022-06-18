#pragma once
#include <map>
#include <meta/groupmeta.h>
#include <string>
#include <string_view>

namespace h5pp {
    class File;
}

namespace tools::tree {
    struct byteResult_t {
        size_t dsetByte = 0;
        size_t dsetStrg = 0;
        size_t attrByte = 0;
        size_t attrStrg = 0;
        size_t headByte = 0;
        size_t headStrg = 0;
    };
    using groupTree_t       = std::map<std::string, GroupMeta, std::less<void>>;
    using groupTreeSorted_t = std::vector<std::pair<std::string, GroupMeta>>;

    byteResult_t      collectBytes(const groupTree_t &groupTree, std::string_view root);
    groupTreeSorted_t sort(const groupTree_t &groupTree, SortKey key = SortKey::SIZE, SortOrder order = SortOrder::DESCENDING, SortType type = SortType::DSET);

    size_t getMaxPathSize(const groupTreeSorted_t &groupTreeSorted, long filterDepth, bool printDset, bool printAttr);
}