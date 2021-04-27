## PrefixFPM: A Parallel Framework for Mining Frequent Patterns
Following divide and conquer, this system treats each pattern to examine and extend as a task, and parallelizes all tasks as much as possible following the idea of divide and conquer. This allows it to use all CPU cores in a machine to mine frequent patterns over big data.

There are 7 applications on top of our framework. Here are the folder structures:

* system: the general-purpose programming interface
* app/prefixspan: the PrefixSpan parallel version developed on top for sequence mining
* app/clospan: the Clospan parallel version developed on top for close sequence mining
* app/gspan: the gSpan parallel version developed on top for subgraph mining
* app/gaston: the Gaston parallel version developed on top for subgraph mining
* sleuth: the Sleuth parallel version developed on top for unordered subtree mining
* treeminer: the TreeMiner parallel version developed on top for ordered subtree mining
* treespan: the PrefixTreeSpan parallel version developed on top for ordered subtree mining

The applicatons are adapted from:

* PrefixSpan: http://chasen.org/~taku/software/prefixspan/prefixspan-0.4.tar.gz
* Clospan: https://sites.cs.ucsb.edu/~xyan/software/Clospan.htm
* gSpan: https://github.com/rkwitt/gboost/tree/master/src-gspan
* Gaston: https://liacs.leidenuniv.nl/~nijssensgr/gaston/
* Sleuth: https://github.com/zakimjz/SLEUTH
* TreeMiner: https://github.com/zakimjz/TreeMiner
and **PrefixTreeSpan** is implemented by ourself. 

Note that OpenMP is used and should be enabled in your Makefile. There are 7 run_XXX.cpp files for the 7 applications above. To compile each one, you may run "cp run_XXX.cpp run.cpp" and then run "make". The compiled "run" program can be renamed to a meaningful name for later use.

A sample command to run each program is put at the end of each each run_XXX.cpp file, which reads data from the sample data that we put in the folder of each application.

The following is the complete command:

./run -s [support_threshold] -i [input_file] -n [thread_number] -t [max_pdbsize_parallelism_threshold] -e [max_time_parallelism_threshold]

### Contact
Da Yan: https://yanlab19870714.github.io/yanda

Email: yanda@uab.edu

### Contributors
Wenwen Qu (major co-contributer)

Da Yan (major co-contributer)

Guimu Guo

Xiaoling Wang
