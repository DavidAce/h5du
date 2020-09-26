#include "tree.h"

std::tuple<size_t, size_t, size_t, size_t> tools::tree::collectBytes(std::map<std::string, GroupMeta> &groupTree, const std::string &root) {
    if(groupTree.find(root) == groupTree.end()) throw std::runtime_error(h5pp::format("Root {} not found in tree", root));
    auto &groupMeta = groupTree[root];
    if(groupMeta.done) return std::make_tuple(groupMeta.dsetByte, groupMeta.dsetStrg, groupMeta.attrByte, groupMeta.attrStrg);

    // Add up the attribute contribution to the datasets
    for(auto &dset : groupMeta.dsetMetas) {
        for(const auto &attr : dset.attrMetas) {
            dset.attrByte += attr.attrByte;
            dset.attrStrg += attr.attrStrg;
        }
        if(dset.attrStrg > 0) dset.attrRatio = static_cast<double>(dset.attrByte) / static_cast<double>(dset.attrStrg);
    }
    // Add up the dataset contribution to the group
    for(const auto &dset : groupMeta.dsetMetas) {
        groupMeta.dsetByte += dset.dsetByte;
        groupMeta.dsetStrg += dset.dsetStrg;
        groupMeta.attrByte += dset.attrByte;
        groupMeta.attrStrg += dset.attrStrg;
    }
    // Add up the attribute contribution to this group
    for(const auto &attr : groupMeta.attrMetas) {
        groupMeta.attrByte += attr.attrByte;
        groupMeta.attrStrg += attr.attrStrg;
    }
    // Add up the contribution from subgroups
    for(auto &subg : groupMeta.subGroups) {
        auto [dsetByte, dsetStrg, attrByte, attrStrg] = collectBytes(groupTree, subg);
        groupMeta.dsetByte += dsetByte;
        groupMeta.dsetStrg += dsetStrg;
        groupMeta.attrByte += attrByte;
        groupMeta.attrStrg += attrStrg;
    }
    if(groupMeta.dsetStrg > 0) groupMeta.dsetRatio = static_cast<double>(groupMeta.dsetByte) / static_cast<double>(groupMeta.dsetStrg);
    if(groupMeta.attrStrg > 0) groupMeta.attrRatio = static_cast<double>(groupMeta.attrByte) / static_cast<double>(groupMeta.attrStrg);
    groupMeta.done = true;
    return std::make_tuple(groupMeta.dsetByte, groupMeta.dsetStrg, groupMeta.attrByte, groupMeta.attrStrg);
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
                        }
                    } break;
                    case SortOrder::DESCENDING: {
                        switch(type) {
                            case SortType::DSET: return lhs.second.dsetStrg > rhs.second.dsetStrg; break;
                            case SortType::ATTR: return lhs.second.attrStrg > rhs.second.attrStrg; break;
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