
#include <getopt.h>
#include <h5pp/h5pp.h>
#include <map>
#include <meta/attrmeta.h>
#include <meta/dsetmeta.h>
#include <meta/groupmeta.h>
#include <tools/class_tic_toc.h>
#include <tools/log.h>
#include <tools/prof.h>
#include <tools/tree.h>
void print_usage() {
    std::cout <<
        R"(
==========  cpp_merger  ============
Usage                       : ./cpp_merger [-option <value>].
-h                          : Help. Shows this text.
-A                          : Print Attributes
-D                          : Print Datasets
-d <max depth>              : Max level of recursion in search (default 1 | inf: -1)
-f <filepath>               : Path to file to analyze
-K sort <size|name>         : Sort results according to key (default size)
-k <filter key>             : Filter links matching key (default "")
-l <root link>              : Start analysis from (default "/")
-n <num hits>               : Max number of search hits (default inf)
-O sort <desc|asc>          : Sort results in ascending/descending order (default DESC)
-T sort <dset|attr>         : Sort results according to dataset or attribute (default dataset)
-v <level>                  : Enables trace-level verbosity
)";
}

int main(int argc, char *argv[]) {
    // Here we use getopt to parse CLI input
    // Note that CLI input always override config-file values
    // wherever they are found (config file, h5 file)
    auto        log = tools::Logger::setLogger("h5du", 2);
    std::string filepath;
    std::string searchKey;
    std::string searchRoot = ".";
    int         maxHits    = -1;
    int         maxDepth   = 1;
    size_t      verbosity  = 2;
    bool        printAttr  = false;
    bool        printDset  = false;
    SortKey     key        = SortKey::SIZE;
    SortOrder   order      = SortOrder::DESCENDING;
    SortType    type       = SortType::DSET;

    while(true) {
        char opt = static_cast<char>(getopt(argc, argv, "ADd:f:K:k:l:n:O:T:hv:"));
        if(opt == EOF) break;
        if(optarg == nullptr) log->info("Parsing input argument: -{}", opt);
        else
            log->info("Parsing input argument: -{} {}", opt, optarg);
        switch(opt) {
            case 'A': printAttr = true; continue;
            case 'D': printDset = true; continue;
            case 'd': maxDepth = std::stoi(optarg, nullptr, 10); continue;
            case 'f': filepath = optarg; continue;
            case 'K': {
                if(std::string_view(optarg).find("size") != std::string::npos) key = SortKey::SIZE;
                else if(std::string_view(optarg).find("name") != std::string::npos) key = SortKey::NAME;
                continue;
            }
            case 'k': searchKey = optarg; continue;
            case 'l': searchRoot = optarg; continue;
            case 'n': maxHits = std::stoi(optarg, nullptr, 10); continue;
            case 'O': {
                if(std::string_view(optarg).find("asc") != std::string::npos) order = SortOrder::ASCENDING;
                else if(std::string_view(optarg).find("desc") != std::string::npos) order = SortOrder::DESCENDING;
                continue;
            }
            case 'T': {
                if(std::string_view(optarg).find("dset") != std::string::npos) type = SortType::DSET;
                else if(std::string_view(optarg).find("attr") != std::string::npos) type = SortType::ATTR;
                continue;
            }
            case 'v': verbosity = std::strtoul(optarg, nullptr, 10); continue;
            case ':': log->error("Option -{} needs a value", opt); break;
            case 'h':
            case '?':
            default: print_usage(); exit(0);
            case -1: break;
        }
        break;
    }
    if(filepath.empty()) {
        print_usage();
        throw std::runtime_error(h5pp::format("Invalid path to file: {}\n\tSet argument -f <valid filepath>.", filepath));
    }
    tools::Logger::setLogLevel(log, verbosity);
    tools::Logger::disableTimestamp(log);

    auto file = h5pp::File(filepath, h5pp::FilePermission::READONLY, 2);

    // Collect a tree of groups,datasets and attributes
    std::map<std::string, GroupMeta> groupTree;
    for(auto &group : file.findGroups(searchKey, searchRoot, maxHits, maxDepth)) {
        // For each group we scan it's dataset, attribute and group content
        if(group == ".") group = searchRoot;
        else group = h5pp::format("{}/{}",searchRoot,group);
        auto                     groupFound = file.findGroups(searchKey, group, maxHits, 0);
        auto                     dsetsFound = file.findDatasets(searchKey, group, maxHits, 0);
        std::vector<std::string> attrsFound;
        if(group.find('/') != std::string::npos) attrsFound = file.getAttributeNames(group);

        if(groupTree.find(group) == groupTree.end()) groupTree[group] = GroupMeta(group);
        auto &groupMeta = groupTree[group];
        for(auto &subg : groupFound) {
            if(group == ".") groupMeta.subGroups.emplace_back(subg);
            else
                groupMeta.subGroups.emplace_back(groupMeta.path + "/" + subg);
        }
        for(auto &dset : dsetsFound) groupMeta.dsetMetas.emplace_back(file.getDatasetInfo(h5pp::format("{}/{}", group, dset)));
        for(auto &attr : attrsFound) groupMeta.attrMetas.emplace_back(file.getAttributeInfo(h5pp::format("{}", group), attr));
        // On each dataset, we populate its attribute details
        for(auto &dset : groupMeta.dsetMetas) {
            for(auto &attr : file.getAttributeNames(dset.path)) { dset.attrMetas.emplace_back(file.getAttributeInfo(h5pp::format("{}", dset.path), attr)); }
        }
    }
    if(groupTree.empty()) throw std::runtime_error(h5pp::format("Nothing found in file [{}] starting from root [{}] matching key [{}]",file.getFilePath(),searchRoot,searchKey));
    tools::tree::collectBytes(groupTree, searchRoot);
    for(auto &group : groupTree) group.second.sort(key,order,type);
    auto groupTreeSorted = tools::tree::sort(groupTree,key,order,type);

    h5pp::print("{:<12}{:64}{:>16}{:>16}{:>16}{:>16}{:>16}{:>16}\n", "Type", "Path", "Dataset size", "Storage size", "Ratio", "Attribute size", "Storage size",
                "Ratio");
    for(auto &[group, groupMeta] : groupTreeSorted) {
        h5pp::print("{:<12}{:<64}{:>16}{:>16}{:>16.2f}{:>16}{:>16}{:>16.2f}\n", "Group", groupMeta.path, groupMeta.dsetByte, groupMeta.dsetStrg,
                    groupMeta.dsetRatio, groupMeta.attrByte, groupMeta.attrStrg, groupMeta.attrRatio);
        if(printDset){
            for(auto &dset : groupMeta.dsetMetas) {
                h5pp::print("{:<12}{:<64}{:>16}{:>16}{:>16.2f}{:>16}{:>16}{:>16.2f}\n", "Dataset", h5pp::format("  {}", dset.name), dset.dsetByte, dset.dsetStrg,
                            dset.dsetRatio, dset.attrByte, dset.attrStrg, dset.attrRatio);
                if(not printAttr) continue;
                for(auto &attr : dset.attrMetas) {
                    h5pp::print("{:<12}{:<64}{:>16}{:>16}{:>16}{:>16}{:>16}{:>16.2f}\n", "Attribute", h5pp::format("   {}", attr.name), " ", " ", " ",
                                attr.attrByte, attr.attrStrg, attr.ratio);
                }
            }
        }
        if(printAttr){
            for(auto &attr : groupMeta.attrMetas) {
                h5pp::print("{:<12}{:<64}{:>16}{:>16}{:>16}{:>16}{:>16}{:>16.2f}\n", "Attribute", h5pp::format("   {}", attr.name), " ", " ", " ",
                            attr.attrByte, attr.attrStrg, attr.ratio);
            }
        }
    }
}