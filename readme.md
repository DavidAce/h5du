# h5du

```bash
==========  h5du  ============
HDF5 Disk Usage Analyzer

Usage                       : ./h5du [-option <value>].

option <value>              : Description
----------------------------------------------------------------
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
-v <level>                  : Enables verbosity at level 0(max) to 5 (min)
----------------------------------------------------------------

Example 1 - Print the size of all groups recursively in descending order of size:
     ./h5du -f myfile.h5

Example 2 - Print the size of all groups and datasets recursively in ascending order of name:
    ./h5du -f myfile.h5 -D -K name
    
Example 3 - Print the size of all groups starting from group "grpA/grpB":
    ./h5du -f myfile.h5 -l grpA/grpB
```