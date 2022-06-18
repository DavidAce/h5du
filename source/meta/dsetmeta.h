#pragma once
#include "attrmeta.h"
#include "headmeta.h"
#include <h5pp/h5pp.h>
#include <string>
#include <vector>
struct DsetMeta {
    std::string           path;
    std::string           name;
    mutable std::size_t   dsetByte  = 0;
    mutable std::size_t   dsetStrg  = 0;
    mutable std::size_t   attrByte  = 0;
    mutable std::size_t   attrStrg  = 0;
    mutable std::size_t   headByte  = 0;
    mutable std::size_t   headStrg  = 0;
    mutable double        dsetRatio = 1.0;
    mutable double        attrRatio = 1.0;
    mutable double        headRatio = 1.0;
    std::vector<AttrMeta> attrMetas;
    std::vector<HeadMeta> headMetas;
    explicit DsetMeta(const h5pp::DsetInfo &info) {
        info.assertReadReady();
        path = info.dsetPath.value();
        if(not path.empty()) name = path.substr(path.find_last_of('/') + 1);
        dsetByte = info.dsetByte.value();
        if(info.h5Layout.value() == H5D_CHUNKED) dsetStrg = H5Dget_storage_size(info.h5Dset.value());
        else
            dsetStrg = dsetByte;
        dsetRatio = static_cast<double>(dsetByte) / static_cast<double>(dsetStrg);
    }

    void sort(SortKey sort = SortKey::SIZE, SortOrder order = SortOrder::DESCENDING) {
        switch(sort) {
            case SortKey::SIZE: {
                switch(order) {
                    case SortOrder::ASCENDING: {
                        auto attr_sort_byte = [](auto &left, auto &right) { return left.attrByte < right.attrByte; };
                        std::sort(attrMetas.begin(), attrMetas.end(), attr_sort_byte);
                        break;
                    }
                    case SortOrder::DESCENDING: {
                        auto attr_sort_byte = [](auto &left, auto &right) { return left.attrByte > right.attrByte; };
                        std::sort(attrMetas.begin(), attrMetas.end(), attr_sort_byte);
                        break;
                    }
                }
                break;
            }
            case SortKey::NAME: {
                switch(order) {
                    case SortOrder::ASCENDING: {
                        auto attr_sort_name = [](auto &left, auto &right) { return left.name < right.name; };
                        std::sort(attrMetas.begin(), attrMetas.end(), attr_sort_name);
                        break;
                    }
                    case SortOrder::DESCENDING: {
                        auto attr_sort_name = [](auto &left, auto &right) { return left.name > right.name; };
                        std::sort(attrMetas.begin(), attrMetas.end(), attr_sort_name);
                        break;
                    }
                }
                break;
            }
        }
    }
};