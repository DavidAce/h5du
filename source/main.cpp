
#include <getopt.h>
#include <h5pp/h5pp.h>
#include <map>
#include <meta/attrmeta.h>
#include <meta/dsetmeta.h>
#include <meta/groupmeta.h>
#include <tools/human.h>
#include <tools/log.h>
#include <tools/text.h>
#include <tools/tree.h>
void print_usage() {
    std::string helpstr = R"(
==============================================  h5du  ==============================================
HDF5 Disk Usage Analyzer

Usage                                : ./h5du [-option <value>].
----------------------------------------------------------------------------------------------------
option  <value>            default   : Description
----------------------------------------------------------------------------------------------------
-h                                   : Help. Shows this text.
-A                                   : Print Attributes
-B      <1000|1024>        1024      : Base for converting bytes to human-readable units (kB/KiB)
-D                                   : Print Datasets
-d      <depth filter>     -1 (inf)  : Print objects in groups up to this depth
-f      <filepath>                   : Path to file to analyze
-H                                   : Print human-readable units (kB/KiB, MB/MiB...)
-K      <size|name>        size      : Sort results according to key "size" or "name"
-k      <filter key>                 : Filter links matching key
-g      <group root>       .         : Start analysis from group
-n      <num hits>         -1 (inf)  : Max number of search hits
-O      <desc|asc>         desc      : Sort results in ascending/descending order
-p      <num decimals>     2         : Number of decimals used when printing human-readable units
-T      <dset|attr|link>   dset      : Sort results according to dataset or attribute
-v      <level>            2         : Enables verbose logging at level 0 (max) to 5 (min)
-V      <level>            2         : As "-v" but for the underlying h5pp library (very verbose)
----------------------------------------------------------------------------------------------------

Example 1 - Print the size of all groups recursively in descending order of size:
     ./h5du -f example.h5

Example 2 - Print the size of all groups starting from group "grpA/grpB" in human-readable units:
    ./h5du -f example.h5 -g grpA/grpB -H

Example 3 - Print the size of all groups and datasets recursively in ascending order of name:
    ./h5du -f example.h5 -D -K name -O asc

)";
    fmt::print("{}\n", helpstr);
}

int main(int argc, char *argv[]) {
    // Here we use getopt to parse CLI input
    // Note that CLI input always override config-file values
    // wherever they are found (config file, h5 file)
    auto log = tools::Logger::setLogger("h5du", 2, false);

    std::string filepath;
    std::string filterKey;
    std::string groupRoot      = "/";
    int         filterHits     = -1;
    int         filterDepth    = -1;
    size_t      verbosity      = 2;
    size_t      h5pp_verbosity = 2;
    bool        printAttr      = false;
    bool        printDset      = false;
    bool        humanRead      = false;
    size_t      base           = 1024;
    size_t      decimals       = 2;
    SortKey     key            = SortKey::SIZE;
    SortOrder   order          = SortOrder::DESCENDING;
    SortType    type           = SortType::DSET;

    while(true) {
        char opt = static_cast<char>(getopt(argc, argv, "AB:Dd:f:HK:k:g:n:O:p:T:hv:V:"));
        if(opt == EOF) break;
        if(optarg == nullptr) log->debug("Parsing input argument: -{}", opt);
        else
            log->debug("Parsing input argument: -{} {}", opt, optarg);
        switch(opt) {
            case 'A': printAttr = true; continue;
            case 'B': {
                base = std::stoul(optarg, nullptr, 10);
                if(base != 1000 and base != 1024) throw std::runtime_error("Base must be either 1000 or 1024");
                continue;
            }
            case 'D': printDset = true; continue;
            case 'd': filterDepth = std::stoi(optarg, nullptr, 10); continue;
            case 'f': filepath = optarg; continue;
            case 'H': humanRead = true; continue;
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
            case 'p': decimals = std::stoul(optarg, nullptr, 10); continue;
            case 'T': {
                if(std::string_view(optarg).find("dset") != std::string::npos) type = SortType::DSET;
                else if(std::string_view(optarg).find("attr") != std::string::npos)
                    type = SortType::ATTR;
                else if(std::string_view(optarg).find("link") != std::string::npos)
                    type = SortType::LINK;
                continue;
            }
            case 'v':
                verbosity = std::strtoul(optarg, nullptr, 10);
                tools::Logger::setLogLevel(log, verbosity);
                continue;
            case 'V': h5pp_verbosity = std::strtoul(optarg, nullptr, 10); continue;
            case ':': log->error("Option -{} needs a value", opt); break;
            case 'h':
            case '?': print_usage(); exit(0);
            default:
                log->error("Option -{} is invalid", opt);
                print_usage();
                exit(0);
            case -1: break;
        }
        break;
    }

    // Sanitize inputs
    if(filepath.empty()) {
        print_usage();
        throw std::runtime_error(fmt::format(FMT_COMPILE("Invalid path to file: {}\n\tSet argument -f <valid filepath>."), filepath));
    }
    // Fix for users sometimes starting paths with "./", just remove the dot
    text::clipHead(groupRoot, ".");
    auto file = h5pp::File(filepath, h5pp::FilePermission::READONLY, h5pp_verbosity);

    // Collect a tree of groups,datasets and attributes
    std::map<std::string, GroupMeta> groupTree;
    for(auto &group : file.findGroups(filterKey, groupRoot, filterHits, -1)) {
        if(group == ".") group = groupRoot;
        else if (groupRoot != "." and groupRoot != "/")
            group = fmt::format(FMT_COMPILE("{}/{}"), groupRoot, group); // Cast group to full path
        if(groupTree.find(group) == groupTree.end()) groupTree[group] = GroupMeta(group);
        auto &groupMeta = groupTree[group];

        // For each group we scan it's dataset, attribute and group content
        // Note that we don't consider filterHits or filterDepth here, but later when printing

        // Add datasets inside this group
        for(auto &dset : file.findDatasets(filterKey, group, filterHits, 0)) {
            groupMeta.dsetMetas.emplace_back(file.getDatasetInfo(fmt::format(FMT_COMPILE("{}/{}"), group, dset)));
        }
        // Add attributes attached to this group
        std::vector<std::string> attrsFound;
        if(group.find('/') != std::string::npos) attrsFound = file.getAttributeNames(group);
        for(auto &attr : attrsFound) groupMeta.attrMetas.emplace_back(file.getAttributeInfo(group, attr));
        // Add link header of this group
        groupMeta.linkMetas.emplace_back(file.getLinkInfo(group));

        // To each dataset, add the attributes and header attached to that dataset
        for(auto &dset : groupMeta.dsetMetas){
            for(auto &attr : file.getAttributeNames(dset.path)) dset.attrMetas.emplace_back(file.getAttributeInfo(dset.path, attr));
            dset.linkMetas.emplace_back(file.getLinkInfo(dset.path));
        }


        // Add subgroups to this group.
        for(auto &subg : file.findGroups(filterKey, group, -1, 0)) {
            if(group == "." or group == "/") groupMeta.subGroups.emplace_back(subg);
            else
                groupMeta.subGroups.emplace_back(fmt::format(FMT_COMPILE("{}/{}"), groupMeta.path, subg));
        }
    }

    // Throw error if nothing found
    if(groupTree.empty())
        throw std::runtime_error(
            fmt::format(FMT_COMPILE("Nothing found in file [{}] starting from root [{}] matching key [{}]"), file.getFilePath(), groupRoot, filterKey));

    // Count up the bytes in the tree that was found
    tools::tree::collectBytes(groupTree, groupRoot);

    // Sort the tree. This can use some improvement, since we copy the whole
    // map into a vector to get it sorted. Perhaps one could use the vector from start
    // but then it's a mess to build the tree instead.
    for(auto &group : groupTree) group.second.sort(key, order, type);
    auto groupTreeSorted = tools::tree::sort(groupTree, key, order, type);

    // Find the longest path in the tree to set the width of the "Path" column
    size_t maxPathSize = tools::tree::getMaxPathSize(groupTreeSorted, filterDepth, printDset, printAttr);

    // Print report
    fmt::print(FMT_COMPILE("{1:<10}{2:<{0}}{3:>16}{4:>16}{5:>8}{6:>16}{7:>16}\n"), maxPathSize, "Type", "Path", "Dataset size", "Storage size", "Ratio", "Attribute size", "Header Size");
    for(const auto &[group, groupMeta] : groupTreeSorted) {
        // Do not print if user wants to filter off groups deeper than filterDepth
        if(filterDepth >= 0 and groupMeta.depth > filterDepth) continue;

        // Print the current group
        fmt::print(FMT_COMPILE("{1:<10}{2:<{0}}{3:>16}{4:>16}{5:>8.2f}{6:>16}{7:>16}\n"), maxPathSize, "Group", groupMeta.path, tools::fmtBytes(humanRead, groupMeta.dsetByte, base, decimals),
                    tools::fmtBytes(humanRead, groupMeta.dsetStrg, base, decimals), groupMeta.dsetRatio,
                    tools::fmtBytes(humanRead, groupMeta.attrByte, base, decimals), tools::fmtBytes(humanRead, groupMeta.linkByte, base, decimals));
        if(printDset) {
            // Print datasets
            for(const auto &dset : groupMeta.dsetMetas) {
                fmt::print(FMT_COMPILE("{1:<10}{2:<{0}}{3:>16}{4:>16}{5:>8.2f}{6:>16}{7:>16}\n"), maxPathSize, "Dataset", fmt::format(FMT_COMPILE("{:^{}}{}"), " ", 4, dset.name),
                            tools::fmtBytes(humanRead, dset.dsetByte, base, decimals), tools::fmtBytes(humanRead, dset.dsetStrg, base, decimals),
                            dset.dsetRatio, tools::fmtBytes(humanRead, dset.attrByte, base, decimals),
                            tools::fmtBytes(humanRead, dset.linkByte, base, decimals));
                if(not printAttr) continue;
                // Print attributes on this dataset
                for(const auto &attr : dset.attrMetas) {
                    fmt::print(FMT_COMPILE("{1:<10}{2:<{0}}{3:>16}{4:>16}{5:>8}{6:>16}\n"), maxPathSize, "Attribute", fmt::format(FMT_COMPILE("{:^{}}{}"), " ", 8, attr.name), " ", " ", " ",
                                tools::fmtBytes(humanRead, attr.attrByte, base, decimals));
                }
            }
        }
        if(printAttr) {
            // Print attributes on this group
            for(const auto &attr : groupMeta.attrMetas) {
                fmt::print(FMT_COMPILE("{1:<10}{2:<54}{3:>16}{4:>16}{5:>8}{6:>16}\n"), maxPathSize, "Attribute", fmt::format(FMT_COMPILE("{:^{}}{}"), " ", 4, attr.name), " ", " ", " ",
                            tools::fmtBytes(humanRead, attr.attrByte, base, decimals));
            }
        }
    }
}