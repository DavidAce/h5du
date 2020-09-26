#pragma once
#include <vector>
#include <string>
#include <h5pp/h5pp.h>
#include "enums.h"
struct AttrMeta{
    std::string path;
    std::string name;
    std::size_t attrByte = 0;
    std::size_t attrStrg = 0;
    double ratio = 1.0;
    explicit AttrMeta(const h5pp::AttrInfo & info){
        info.assertReadReady();
        path = info.linkPath.value();
        name = info.attrName.value();
        attrByte = info.attrByte.value();
        attrStrg = attrByte;
        // Is storage ever different?
//        attrStrg = H5Aget_storage_size(info.h5Attr.value());
        if(attrStrg > 0)
            ratio = static_cast<double>(attrByte)/static_cast<double>(attrStrg);
    }
};