#pragma once
#include <vector>
#include <string>
#include <h5pp/h5pp.h>
#include <tools/log.h>
#include "enums.h"
struct LinkMeta{
    std::string path;
    std::string name;
    std::size_t linkByte = 0;
    std::size_t linkStrg = 0;
    double ratio = 1.0;
    explicit LinkMeta(const h5pp::LinkInfo & info){
        if(not info.linkExists.value()){
            return;
        }
        info.assertReadReady();
        path = info.linkPath.value();
        if(not path.empty()) name = path.substr(path.find_last_of('/') + 1);
        linkByte = info.h5HdrByte.value();
        linkStrg = linkByte;
        // Is storage ever different?
        //        linkStrg = H5Aget_storage_size(info.h5Link.value());
        if(linkStrg > 0)
            ratio = static_cast<double>(linkByte)/static_cast<double>(linkStrg);
    }

    LinkMeta & operator = (const h5pp::LinkInfo & rhs){
        *this = LinkMeta(rhs);
        return *this;
    }

};