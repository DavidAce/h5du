#pragma once
#include "attrmeta.h"
#include "dsetmeta.h"
#include "enums.h"
#include "headmeta.h"
#include <h5pp/h5pp.h>
#include <string>
#include <vector>

struct GroupMeta {
    std::string              path;
    std::string              name;
    long                     depth     = 0;
    mutable bool             done      = false;
    mutable std::size_t      dsetByte  = 0;
    mutable std::size_t      dsetStrg  = 0;
    mutable std::size_t      attrByte  = 0;
    mutable std::size_t      attrStrg  = 0;
    mutable std::size_t      headByte  = 0;
    mutable std::size_t      headStrg  = 0;
    mutable double           dsetRatio = 1.0;
    mutable double           attrRatio = 1.0;
    mutable double           headRatio = 1.0;
    std::vector<std::string> subGroups;
    std::vector<DsetMeta>    dsetMetas;
    std::vector<AttrMeta>    attrMetas;
    std::vector<HeadMeta>    headMetas;
    GroupMeta() = default;
    explicit GroupMeta(const std::string &path_) : path(path_) {
        name  = path.substr(path.find_last_of('/') + 1);
        depth = std::count_if(path.begin(), path.end(), [](char c) { return c == '/'; });



    }

    void sort(SortKey key = SortKey::SIZE, SortOrder order = SortOrder::DESCENDING, SortType type = SortType::DSET) {
        for(auto &dset : dsetMetas) dset.sort(key, order);
        auto dset_sorter = [key, order, type](auto &lhs, auto &rhs) {
            switch(key) {
                case SortKey::SIZE: {
                    switch(order) {
                        case SortOrder::ASCENDING: {
                            switch(type) {
                                case SortType::DSET: return lhs.dsetStrg < rhs.dsetStrg; break;
                                case SortType::ATTR: return lhs.attrStrg < rhs.attrStrg; break;
                                case SortType::LINK: return lhs.headStrg < rhs.headStrg; break;
                            }
                        } break;
                        case SortOrder::DESCENDING: {
                            switch(type) {
                                case SortType::DSET: return lhs.dsetStrg > rhs.dsetStrg; break;
                                case SortType::ATTR: return lhs.attrStrg > rhs.attrStrg; break;
                                case SortType::LINK: return lhs.headStrg > rhs.headStrg; break;
                            }
                        } break;
                    }
                } break;
                case SortKey::NAME: {
                    switch(order) {
                        case SortOrder::DESCENDING: return lhs.name > rhs.name; break;
                        case SortOrder::ASCENDING: return lhs.name < rhs.name; break;
                    }
                } break;
            }
            return lhs.dsetStrg > rhs.dsetStrg;
        };

        auto attr_sorter = [key, order](auto &lhs, auto &rhs) {
            switch(key) {
                case SortKey::SIZE: {
                    switch(order) {
                        case SortOrder::ASCENDING: return lhs.attrStrg < rhs.attrStrg; break;
                        case SortOrder::DESCENDING: return lhs.attrStrg > rhs.attrStrg; break;
                    }
                } break;
                case SortKey::NAME: {
                    switch(order) {
                        case SortOrder::DESCENDING: return lhs.name > rhs.name; break;
                        case SortOrder::ASCENDING: return lhs.name < rhs.name; break;
                    }
                } break;
            }
            return lhs.attrStrg > rhs.attrStrg;
        };

        auto link_sorter = [key, order](auto &lhs, auto &rhs) {
            switch(key) {
                case SortKey::SIZE: {
                    switch(order) {
                        case SortOrder::ASCENDING: return lhs.headStrg < rhs.headStrg; break;
                        case SortOrder::DESCENDING: return lhs.headStrg > rhs.headStrg; break;
                    }
                } break;
                case SortKey::NAME: {
                    switch(order) {
                        case SortOrder::DESCENDING: return lhs.name > rhs.name; break;
                        case SortOrder::ASCENDING: return lhs.name < rhs.name; break;
                    }
                } break;
            }
            return lhs.headStrg > rhs.headStrg;
        };

        std::sort(dsetMetas.begin(), dsetMetas.end(), dset_sorter);
        std::sort(attrMetas.begin(), attrMetas.end(), attr_sorter);
        std::sort(headMetas.begin(), headMetas.end(), link_sorter);
    }
};