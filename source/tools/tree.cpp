#include "tree.h"

tools::tree::byteResult_t tools::tree::collectBytes(const groupTree_t &groupTree, std::string_view root) {
    if(groupTree.find(root) == groupTree.end()) {
        auto log = spdlog::get("h5du");
        log->info("groupTree:");
        for(const auto &group : groupTree) log->info("    {}", group.first);
        throw std::runtime_error(h5pp::format("Root {} not found in groupTree", root));
    }
    auto &groupMeta = groupTree.find(root)->second;
    if(groupMeta.done) return {groupMeta.dsetByte, groupMeta.dsetStrg, groupMeta.attrByte, groupMeta.attrStrg, groupMeta.headByte, groupMeta.headStrg};

    // Add up the attribute contribution to the datasets
    for(auto &dset : groupMeta.dsetMetas) {
        for(const auto &attr : dset.attrMetas) {
            dset.attrByte += attr.attrByte;
            dset.attrStrg += attr.attrStrg;
        }
        if(dset.attrStrg > 0) dset.attrRatio = static_cast<double>(dset.attrByte) / static_cast<double>(dset.attrStrg);
    }

    // Add up the header contribution to the datasets
    for(auto &dset : groupMeta.dsetMetas) {
        for(const auto &link : dset.headMetas) {
            dset.headByte += link.headByte;
            dset.headStrg += link.headStrg;
        }
        if(dset.headStrg > 0) dset.attrRatio = static_cast<double>(dset.headByte) / static_cast<double>(dset.headStrg);
    }

    // Add up the dataset contribution to the group
    for(const auto &dset : groupMeta.dsetMetas) {
        groupMeta.dsetByte += dset.dsetByte;
        groupMeta.dsetStrg += dset.dsetStrg;
        groupMeta.attrByte += dset.attrByte;
        groupMeta.attrStrg += dset.attrStrg;
        groupMeta.headByte += dset.headByte;
        groupMeta.headStrg += dset.headStrg;
    }
    // Add up the attribute contribution to this group
    for(const auto &attr : groupMeta.attrMetas) {
        groupMeta.attrByte += attr.attrByte;
        groupMeta.attrStrg += attr.attrStrg;
    }
    // Add up the header contribution to this group
    for(const auto &link : groupMeta.headMetas) {
        groupMeta.headByte += link.headByte;
        groupMeta.headStrg += link.headStrg;
    }

    // Add up the contribution from subgroups
    for(const auto &subg : groupMeta.subGroups) {
        auto res = collectBytes(groupTree, subg);
        groupMeta.dsetByte += res.dsetByte;
        groupMeta.dsetStrg += res.dsetStrg;
        groupMeta.attrByte += res.attrByte;
        groupMeta.attrStrg += res.attrStrg;
        groupMeta.headByte += res.headByte;
        groupMeta.headStrg += res.headStrg;
    }
    if(groupMeta.dsetStrg > 0) groupMeta.dsetRatio = static_cast<double>(groupMeta.dsetByte) / static_cast<double>(groupMeta.dsetStrg);
    if(groupMeta.attrStrg > 0) groupMeta.attrRatio = static_cast<double>(groupMeta.attrByte) / static_cast<double>(groupMeta.attrStrg);
    if(groupMeta.headStrg > 0) groupMeta.headRatio = static_cast<double>(groupMeta.headByte) / static_cast<double>(groupMeta.headStrg);
    groupMeta.done = true;
    return {groupMeta.dsetByte, groupMeta.dsetStrg, groupMeta.attrByte, groupMeta.attrStrg, groupMeta.headByte, groupMeta.headStrg};
}

tools::tree::groupTreeSorted_t tools::tree::sort(const groupTree_t &groupTree, SortKey key, SortOrder order, SortType type) {
    auto sorter = [key, order, type](auto &lhs, auto &rhs) {
        switch(key) {
            case SortKey::SIZE: {
                switch(order) {
                    case SortOrder::ASCENDING: {
                        switch(type) {
                            case SortType::DSET: return lhs.second.dsetStrg < rhs.second.dsetStrg; break;
                            case SortType::ATTR: return lhs.second.attrStrg < rhs.second.attrStrg; break;
                            case SortType::LINK: return lhs.second.headStrg < rhs.second.headStrg; break;
                        }
                    } break;
                    case SortOrder::DESCENDING: {
                        switch(type) {
                            case SortType::DSET: return lhs.second.dsetStrg > rhs.second.dsetStrg; break;
                            case SortType::ATTR: return lhs.second.attrStrg > rhs.second.attrStrg; break;
                            case SortType::LINK: return lhs.second.headStrg > rhs.second.headStrg; break;
                        }
                    } break;
                }
            } break;
            case SortKey::NAME: {
                switch(order) {
                    case SortOrder::DESCENDING: return lhs.second.path > rhs.second.path; break;
                    case SortOrder::ASCENDING: return lhs.second.path < rhs.second.path; break;
                }
            } break;
        }
        return lhs.second.dsetStrg > rhs.second.dsetStrg;
    };
    std::vector<std::pair<std::string, GroupMeta>> groupTreeVec;
    std::copy(groupTree.begin(), groupTree.end(), std::back_inserter(groupTreeVec));
    std::sort(groupTreeVec.begin(), groupTreeVec.end(), sorter);
    return groupTreeVec;
}

size_t tools::tree::getMaxPathSize(const groupTreeSorted_t &groupTreeSorted, long filterDepth, bool printDset, bool printAttr) {
    size_t maxPathSize = 0;
    for(const auto &[group, groupMeta] : groupTreeSorted) {
        // Do not print if user wants to filter off groups deeper than filterDepth
        if(filterDepth >= 0 and groupMeta.depth > filterDepth) continue;
        maxPathSize = std::max(maxPathSize, groupMeta.path.size());

        if(printDset) {
            // Print datasets
            for(auto &dset : groupMeta.dsetMetas) {
                maxPathSize = std::max(maxPathSize, 4 + dset.name.size());
                if(not printAttr) continue;
                // Print attributes on this dataset
                for(auto &attr : dset.attrMetas) { maxPathSize = std::max(maxPathSize, 8 + attr.name.size()); }
            }
        }
        if(printAttr) {
            // Print attributes on this group
            for(auto &attr : groupMeta.attrMetas) { maxPathSize = std::max(maxPathSize, 4 + attr.name.size()); }
        }
    }
    return maxPathSize;
}
