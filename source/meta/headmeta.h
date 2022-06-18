#pragma once
#include <vector>
#include <string>
#include <h5pp/h5pp.h>
#include <tools/log.h>
#include "enums.h"
struct HeadMeta {
    std::string path;
    std::string name;
    std::size_t headByte = 0;
    std::size_t headStrg = 0;
    double ratio = 1.0;
    explicit HeadMeta(const h5pp::LinkInfo & info){
        if(not info.linkExists.value()) return;

        info.assertReadReady();
        path = info.linkPath.value();
        if(not path.empty()) name = path.substr(path.find_last_of('/') + 1);
        headByte = info.h5HdrByte.value();
        headStrg = headByte;
        // Is storage ever different?
        //        headStrg = H5Aget_storage_size(info.h5Link.value());
        if(headStrg > 0)
            ratio = static_cast<double>(headByte)/static_cast<double>(headStrg);
    }

    HeadMeta & operator = (const h5pp::LinkInfo & rhs){
        *this = HeadMeta(rhs);
        return *this;
    }

};