#include "tree.h"

tools::tree::byteResult_t tools::tree::collectBytes(std::map<std::string, GroupMeta> &groupTree, const std::string &root) {
    if(groupTree.find(root) == groupTree.end()) throw std::runtime_error(h5pp::format("Root {} not found in tree", root));
    auto &groupMeta = groupTree[root];
    if(groupMeta.done) return {groupMeta.dsetByte, groupMeta.dsetStrg, groupMeta.attrByte, groupMeta.attrStrg, groupMeta.linkByte, groupMeta.linkStrg};

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
        for(const auto &link : dset.linkMetas) {
            dset.linkByte += link.linkByte;
            dset.linkStrg += link.linkStrg;
        }
        if(dset.linkStrg > 0) dset.attrRatio = static_cast<double>(dset.linkByte) / static_cast<double>(dset.linkStrg);
    }

    // Add up the dataset contribution to the group
    for(const auto &dset : groupMeta.dsetMetas) {
        groupMeta.dsetByte += dset.dsetByte;
        groupMeta.dsetStrg += dset.dsetStrg;
        groupMeta.attrByte += dset.attrByte;
        groupMeta.attrStrg += dset.attrStrg;
        groupMeta.linkByte += dset.linkByte;
        groupMeta.linkStrg += dset.linkStrg;
    }
    // Add up the attribute contribution to this group
    for(const auto &attr : groupMeta.attrMetas) {
        groupMeta.attrByte += attr.attrByte;
        groupMeta.attrStrg += attr.attrStrg;
    }
    // Add up the header contribution to this group
    for(const auto &link : groupMeta.linkMetas) {
        groupMeta.linkByte += link.linkByte;
        groupMeta.linkStrg += link.linkStrg;
    }

    // Add up the contribution from subgroups
    for(auto &subg : groupMeta.subGroups) {
        auto res = collectBytes(groupTree, subg);
        groupMeta.dsetByte += res.dsetByte;
        groupMeta.dsetStrg += res.dsetStrg;
        groupMeta.attrByte += res.attrByte;
        groupMeta.attrStrg += res.attrStrg;
        groupMeta.linkByte += res.linkByte;
        groupMeta.linkStrg += res.linkStrg;
    }
    if(groupMeta.dsetStrg > 0) groupMeta.dsetRatio = static_cast<double>(groupMeta.dsetByte) / static_cast<double>(groupMeta.dsetStrg);
    if(groupMeta.attrStrg > 0) groupMeta.attrRatio = static_cast<double>(groupMeta.attrByte) / static_cast<double>(groupMeta.attrStrg);
    if(groupMeta.linkStrg > 0) groupMeta.linkRatio = static_cast<double>(groupMeta.linkByte) / static_cast<double>(groupMeta.linkStrg);
    groupMeta.done = true;
    return {groupMeta.dsetByte, groupMeta.dsetStrg, groupMeta.attrByte, groupMeta.attrStrg, groupMeta.linkByte, groupMeta.linkStrg};
}

std::vector<std::pair<std::string,GroupMeta>> tools::tree::sort(std::map<std::string, GroupMeta> &groupTree, SortKey key, SortOrder order,
                       SortType type) {
    auto sorter = [key, order, type](auto &lhs, auto &rhs) {
        switch(key) {
            case SortKey::SIZE: {
                switch(order) {
                    case SortOrder::ASCENDING: {
                        switch(type) {
                            case SortType::DSET: return lhs.second.dsetStrg < rhs.second.dsetStrg; break;
                            case SortType::ATTR: return lhs.second.attrStrg < rhs.second.attrStrg; break;
                            case SortType::LINK: return lhs.second.linkStrg < rhs.second.linkStrg; break;
                        }
                    } break;
                    case SortOrder::DESCENDING: {
                        switch(type) {
                            case SortType::DSET: return lhs.second.dsetStrg > rhs.second.dsetStrg; break;
                            case SortType::ATTR: return lhs.second.attrStrg > rhs.second.attrStrg; break;
                            case SortType::LINK: return lhs.second.linkStrg > rhs.second.linkStrg; break;
                        }
                    } break;
                }
            } break;
            case SortKey::NAME: {
                switch(order) {
                    case SortOrder::DESCENDING: return lhs.second.path > rhs.second.path; break;
                    case SortOrder::ASCENDING:  return lhs.second.path < rhs.second.path; break;
                }
            } break;
        }
      return lhs.second.dsetStrg > rhs.second.dsetStrg;
    };
    std::vector<std::pair<std::string,GroupMeta>> groupTreeVec;
    std::copy(groupTree.begin(),groupTree.end(), std::back_inserter(groupTreeVec));
    std::sort(groupTreeVec.begin(),groupTreeVec.end(),sorter);
    return groupTreeVec;
}


size_t tools::tree::getMaxPathSize(const std::vector<std::pair<std::string, GroupMeta>> groupTreeSorted, long filterDepth, bool printDset, bool printAttr) {
    size_t maxPathSize = 0;
    for(const auto &[group, groupMeta] : groupTreeSorted) {
        // Do not print if user wants to filter off groups deeper than filterDepth
        if(filterDepth >= 0 and groupMeta.depth > filterDepth) continue;
        maxPathSize = std::max(maxPathSize, groupMeta.path.size());

        if(printDset) {
            // Print datasets
            for(auto &dset : groupMeta.dsetMetas) {
                maxPathSize = std::max(maxPathSize, 4+dset.name.size());
                if(not printAttr) continue;
                // Print attributes on this dataset
                for(auto &attr : dset.attrMetas) {
                    maxPathSize = std::max(maxPathSize, 8+attr.name.size());
                }
            }
        }
        if(printAttr) {
            // Print attributes on this group
            for(auto &attr : groupMeta.attrMetas) {
                maxPathSize = std::max(maxPathSize, 4+attr.name.size());
            }
        }
    }
    return maxPathSize;

}
