
#include <getopt.h>
#include <h5pp/h5pp.h>
#include <map>
#include <meta/attrmeta.h>
#include <meta/dsetmeta.h>
#include <meta/groupmeta.h>
#include <tools/log.h>
#include <tools/tree.h>
void print_usage() {
    std::cout <<
        R"(
==============================================  h5du  ==============================================
HDF5 Disk Usage Analyzer

Usage                                : ./h5du [-option <value>].
----------------------------------------------------------------------------------------------------
option  <value>           default    : Description
----------------------------------------------------------------------------------------------------
-h                                   : Help. Shows this text.
-A                                   : Print Attributes
-D                                   : Print Datasets
-d      <depth filter>    -1 (inf)   : Print objects in groups up to this depth
-f      <filepath>                   : Path to file to analyze
-K      <size|name>        size      : Sort results according to key "size" or "name"
-k      <filter key>                 : Filter links matching key
-g      <group root>       .         : Start analysis from group
-n      <num hits>         -1 (inf)  : Max number of search hits
-O      <desc|asc>         desc      : Sort results in ascending/descending order
-T      <dset|attr>        dset      : Sort results according to dataset or attribute
-v      <level>            2         : Enables verbose logging at level 0 (max) to 5 (min)
-V      <level>            2         : As "-v" but for the underlying h5pp library (very verbose)
----------------------------------------------------------------------------------------------------

Example 1 - Print the size of all groups recursively in descending order of size:
     ./h5du -f example.h5

Example 2 - Print the size of all groups starting from group "grpA/grpB":
    ./h5du -f example.h5 -g grpA/grpB

Example 3 - Print the size of all groups and datasets recursively in ascending order of name:
    ./h5du -f example.h5 -D -K name -O asc

)";
}
void removeSubstring(std::string& s, std::string_view p) {
    std::string::size_type n = p.length();
    for (std::string::size_type i = s.find(p); i != std::string::npos; i = s.find(p))
        s.erase(i, n);
}

int main(int argc, char *argv[]) {
    // Here we use getopt to parse CLI input
    // Note that CLI input always override config-file values
    // wherever they are found (config file, h5 file)
    auto        log = tools::Logger::setLogger("h5du", 2,false);

    std::string filepath;
    std::string filterKey;
    std::string groupRoot  = ".";
    int         filterHits = -1;
    int         filterDepth = -1;
    size_t      verbosity  = 2;
    size_t      h5pp_verbosity  = 2;
    bool        printAttr  = false;
    bool        printDset  = false;
    SortKey     key        = SortKey::SIZE;
    SortOrder   order      = SortOrder::DESCENDING;
    SortType    type       = SortType::DSET;

    while(true) {
        char opt = static_cast<char>(getopt(argc, argv, "ADd:f:K:k:g:n:O:T:hv:V:"));
        if(opt == EOF) break;
        if(optarg == nullptr) log->debug("Parsing input argument: -{}", opt);
        else
            log->debug("Parsing input argument: -{} {}", opt, optarg);
        switch(opt) {
            case 'A': printAttr = true; continue;
            case 'D': printDset = true; continue;
            case 'd': filterDepth = std::stoi(optarg, nullptr, 10); continue;
            case 'f': filepath = optarg; continue;
            case 'K': {
                if(std::string_view(optarg).find("size") != std::string::npos) key = SortKey::SIZE;
                else if(std::string_view(optarg).find("name") != std::string::npos)
                    key = SortKey::NAME;
                continue;
            }
            case 'k': filterKey = optarg; continue;
            case 'g': groupRoot = optarg; continue;
            case 'n': filterHits = std::stoi(optarg, nullptr, 10); continue;
            case 'O': {
                if(std::string_view(optarg).find("asc") != std::string::npos) order = SortOrder::ASCENDING;
                else if(std::string_view(optarg).find("desc") != std::string::npos)
                    order = SortOrder::DESCENDING;
                continue;
            }
            case 'T': {
                if(std::string_view(optarg).find("dset") != std::string::npos) type = SortType::DSET;
                else if(std::string_view(optarg).find("attr") != std::string::npos)
                    type = SortType::ATTR;
                continue;
            }
            case 'v': verbosity = std::strtoul(optarg, nullptr, 10); tools::Logger::setLogLevel(log, verbosity); continue;
            case 'V': h5pp_verbosity = std::strtoul(optarg, nullptr, 10); continue;
            case ':': log->error("Option -{} needs a value", opt); break;
            case 'h':
            case '?': print_usage(); exit(0);
            default: log->error("Option -{} is invalid", opt); print_usage(); exit(0);
            case -1: break;
        }
        break;
    }

    // Sanitize inputs
    if(filepath.empty()) {
        print_usage();
        throw std::runtime_error(h5pp::format("Invalid path to file: {}\n\tSet argument -f <valid filepath>.", filepath));
    }
    // Fix for users sometimes starting paths with "./", just remove it
    removeSubstring(groupRoot,"./");





    auto file = h5pp::File(filepath, h5pp::FilePermission::READONLY, h5pp_verbosity);

    // Collect a tree of groups,datasets and attributes
    std::map<std::string, GroupMeta> groupTree;
    for(auto &group : file.findGroups(filterKey, groupRoot, filterHits, -1)) {
        if(group == ".") group = groupRoot;
        else if(groupRoot != ".")
            group = h5pp::format("{}/{}", groupRoot, group);
        if(groupTree.find(group) == groupTree.end()) groupTree[group] = GroupMeta(group);
        auto &groupMeta = groupTree[group];

        // For each group we scan it's dataset, attribute and group content
        // Note that we don't consider filterHits or filterDepth here, but later when printing

        // Add datasets inside this group
        for(auto &dset : file.findDatasets(filterKey, group, filterHits, 0))
            groupMeta.dsetMetas.emplace_back(file.getDatasetInfo(h5pp::format("{}/{}", group, dset)));
        // Add attributes attached to this group
        std::vector<std::string> attrsFound;
        if(group.find('/') != std::string::npos) attrsFound = file.getAttributeNames(group);
        for(auto &attr : attrsFound) groupMeta.attrMetas.emplace_back(file.getAttributeInfo(group, attr));
        // To each dataset, add the attributes attached to that dataset
        for(auto &dset : groupMeta.dsetMetas)
            for(auto &attr : file.getAttributeNames(dset.path)) dset.attrMetas.emplace_back(file.getAttributeInfo(dset.path, attr));

        // Add subgroups to this group.
        for(auto &subg : file.findGroups(filterKey, group, -1, 0)) {
            if(group == ".") groupMeta.subGroups.emplace_back(subg);
            else
                groupMeta.subGroups.emplace_back(h5pp::format("{}/{}",groupMeta.path, subg));
        }
    }

    // Throw error if nothing found
    if(groupTree.empty())
        throw std::runtime_error(
            h5pp::format("Nothing found in file [{}] starting from root [{}] matching key [{}]", file.getFilePath(), groupRoot, filterKey));

    //Count up the bytes in the tree that was found
    tools::tree::collectBytes(groupTree, groupRoot);

    // Sort the tree. This can use some improvement, since we copy the whole
    // map into a vector to get it sorted. Perhaps one could use the vector from start
    // but then it's a mess to build the tree instead.
    for(auto &group : groupTree) group.second.sort(key, order, type);
    auto groupTreeSorted = tools::tree::sort(groupTree, key, order, type);

    // Print report
    h5pp::print("{:<10}{:54}{:>16}{:>16}{:>8}{:>16}\n", "Type", "Path", "Dataset size", "Storage size", "Ratio", "Attribute size");
    for(auto &[group, groupMeta] : groupTreeSorted) {
        // Do not print if user wants to filter off groups deeper than filterDepth
        if(filterDepth >= 0 and groupMeta.depth > filterDepth) continue;

        // Print the current group
        h5pp::print("{:<10}{:<54}{:>16}{:>16}{:>8.2f}{:>16}\n", "Group", groupMeta.path, groupMeta.dsetByte, groupMeta.dsetStrg, groupMeta.dsetRatio,
                    groupMeta.attrByte);
        if(printDset) {
            // Print datasets
            for(auto &dset : groupMeta.dsetMetas) {
                h5pp::print("{:<10}{:<54}{:>16}{:>16}{:>8.2f}{:>16}\n", "Dataset", h5pp::format("{:^{}}{}", " ", 4, dset.name), dset.dsetByte, dset.dsetStrg,
                            dset.dsetRatio, dset.attrByte);
                if(not printAttr) continue;
                // Print attributes on this dataset
                for(auto &attr : dset.attrMetas) {
                    h5pp::print("{:<10}{:<54}{:>16}{:>16}{:>8}{:>16}\n", "Attribute", h5pp::format("{:^{}}{}", " ", 8, attr.name), " ", " ", " ",
                                attr.attrByte);
                }
            }
        }
        if(printAttr) {
            // Print attributes on this group
            for(auto &attr : groupMeta.attrMetas) {
                h5pp::print("{:<10}{:<54}{:>16}{:>16}{:>8}{:>16}\n", "Attribute", h5pp::format("{:^{}}{}", " ", 4, attr.name), " ", " ", " ", attr.attrByte);
            }
        }
    }
}