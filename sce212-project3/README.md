# Project #3: Simulating Cache Behavior

### Goal
This project is intended to help you understand the principle of caching by implementing a data cache. You will implement a simple cache simulator running for memory trace files. Each input file consists of a sequence of read (load) or write (store) operations with memory addresses. For each step, the cache simulator reads a line of the input file and simulates the internal operation of the cache. 

### Background of the skeleton code

This repository provides the skeleton codes to simulate the cache behavior. First, we recommend you to be familar with the code structure.
The `cache.h` file defines the `struct sce212cache` to model a data cache and you can find the `struct cache_set` type that is comprised of
a set of `struct cache_line`. Each cache line (or block) contains the valid bit, dirty bit, tag, and age fields. In this project, you do not
need to cache the actual data for the memory address because the goal of this project is to understand the cache behavior instead of caching
the actual data. 

Second, you may need to understand how the `main()` function works in `sce212cache.c`. It prepares the cache to simulate a given trace file through 
the `build_cache()` function. Your first task is to implement the cache as intended. Then, the main loop is to read a line of the trace and extract
the access type and memory address to be cached. Then, your second task is to implement accessing the cache (`access_cache()`) with the operation from the trace.


### 1. Task #1: __build_cache()__  
To simulate the data cache, we need to define the capacity, associativity, and block size. These three parameters must be configurable as a command line
interface. The skeleton code already implemented the routine handling of the options. So, you are supposed to complete the `build_cache()` function in the
`cache.c` file. In `cache.h`, we define the cache data structure, called struct `sce212cache`, which contains sets (`struct cache_set`), ways (`n_ways`),
and configurations (`n_set_bits` and `n_data_bits`). The first task is to understand the structure of `sce212cache` and then complete the `build_cache()`
function. 

### 2. Task #2: __access_cache()__
If you allocate the memory of cache structures properly, the next step would be reading a file and applying address lines in the file to the cache
structures. Given those addresses, the cache which was implemented should be  used and contains proper addresses in the cache line. In the function,
you need to write codes looking up the cache for a given memory address and then figure out whether the requested memory can be found in the cache or
not. The tag matching procedure should be implemented. If you can find the requested cache block (as known as cache hit), you need to update the age
field (which is used for LRU replacement) of the cache. Also, you may update the dirty bit depending on the access type. 
    
If the requested memory block is not in your cache, you need to allocate a cache block by either finding an empty space or evicting a cache block that
is least recently used in the given cache set. After that, you need to update the tag information and other fields (valid, age, and dirty). Since we
assume that our cache is write-allocate, you need to properly set the relevant field according to the access type.

Finally, our simulator counts total number of hits, misses, and write-backs for a given trace file. So, we need to update the values through call-by-reference in `access_cache()`.

__Sample files__  
Each sample file in `sample_input` directory has two columns. The first column stands for 
a type of operations such as "R" for Read and "W" for Write. The second column is an address value.
So, you should interpret properly and use them for their own function.

## ETC
The functions above can be modified if the application produces same output in the answer.
So, feel free to change and add any functions if student desires. If you have any question, 
please post your question in the Ajou BB.
