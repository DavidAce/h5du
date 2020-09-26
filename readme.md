# h5du

```
==========  h5du  ============
HDF5 Disk Usage Analyzer

Usage                       : ./h5du [-option <value>].

option <value>              : Description
----------------------------------------------------------------
-h                          : Help. Shows this text.
-A                          : Print Attributes
-D                          : Print Datasets
-d <max depth>              : Max level of recursion in search (default -1 (inf)) (WORK IN PROGRESS)
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
     ./h5du -f example.h5

Example 2 - Print the size of all groups and datasets recursively in ascending order of name:
    ./h5du -f example.h5 -D -K name
    
Example 3 - Print the size of all groups starting from group "grpA/grpB":
    ./h5du -f example.h5 -l grpA/grpB
```


## Example output

```
> ./h5du -f ../example.h5 -l xDMRG

[h5du][  info  ] Parsing input argument: -f ../example.h5
[h5du][  info  ] Parsing input argument: -l xDMRG
Type        Path                                                                Dataset size    Storage size           Ratio  Attribute size    Storage size           Ratio
Group       xDMRG                                                                   21511752        11620887            1.85           16990           16990            1.00
Group       xDMRG/state_0                                                           16104024         8344188            1.93            9840            9840            1.00
Group       xDMRG/state_0/finished                                                   7970072         4099817            1.94            2922            2922            1.00
Group       xDMRG/state_0/checkpoint                                                 8051688         4096915            1.97            2922            2922            1.00
Group       xDMRG/state_0/checkpoint/iter_last                                       8051688         4096915            1.97            2922            2922            1.00
Group       xDMRG/state_0/finished/mps                                               7964352         4031629            1.98            1176            1176            1.00
Group       xDMRG/state_0/checkpoint/iter_last/mps                                   8027120         4028711            1.99            1176            1176            1.00
Group       xDMRG/state_1                                                            5405328         3260379            1.66            6918            6918            1.00
Group       xDMRG/state_1/checkpoint                                                 5348584         3114723            1.72            2922            2922            1.00
Group       xDMRG/state_1/checkpoint/iter_last                                       5348584         3114723            1.72            2922            2922            1.00
Group       xDMRG/state_1/checkpoint/iter_last/mps                                   5338640         3046755            1.75            1176            1176            1.00
Group       xDMRG/state_0/projection                                                   62664           82408            0.76            2282            2282            1.00
Group       xDMRG/state_1/projection                                                   51256           80608            0.64            2282            2282            1.00
Group       xDMRG/state_1/tables                                                        5488           65048            0.08            1714            1714            1.00
Group       xDMRG/state_0/tables                                                       19600           65048            0.30            1714            1714            1.00
Group       xDMRG/model                                                                 2400           16320            0.15             232             232            1.00
Group       xDMRG/state_0/projection/mps                                               48480           14187            3.42             536             536            1.00
Group       xDMRG/state_1/projection/mps                                               41952           12616            3.33             536             536            1.00
 
```

