#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "cache.h"

/***************************************************************/
/*                                                             */
/* Procedure : cdump                                           */
/*                                                             */
/* Purpose   : Dump cache configuration                        */
/*                                                             */
/***************************************************************/
static void cdump(int capacity, int assoc, int block_size)
{

  printf("Cache Configuration:\n");
  printf("-------------------------------------\n");
  printf("Capacity: %dB\n", capacity);
  printf("Associativity: %dway\n", assoc);
  printf("Block Size: %dB\n", block_size);
  printf("\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : sdump                                           */
/*                                                             */
/* Purpose   : Dump cache stat		                       */
/*                                                             */
/***************************************************************/
static void sdump(int total_reads, int total_writes, int write_backs,
    int reads_hits, int write_hits, int reads_misses, int write_misses)
{
  printf("Cache Stat:\n");
  printf("-------------------------------------\n");
  printf("Total reads: %d\n", total_reads);
  printf("Total writes: %d\n", total_writes);
  printf("Write-backs: %d\n", write_backs);
  printf("Read hits: %d\n", reads_hits);
  printf("Write hits: %d\n", write_hits);
  printf("Read misses: %d\n", reads_misses);
  printf("Write misses: %d\n", write_misses);
  printf("\n");
}


/***************************************************************/
/*                                                             */
/* Procedure : xdump                                           */
/*                                                             */
/* Purpose   : Dump current cache state                        */
/* 					                       */
/* Cache Design						       */
/*  							       */
/* 	    cache[set][assoc][word per block]		       */
/*                                			       */
/*      				                       */
/*       ----------------------------------------	       */
/*       I        I  way0  I  way1  I  way2  I                 */
/*       ----------------------------------------              */
/*       I        I  word0 I  word0 I  word0 I                 */
/*       I  set0  I  word1 I  word1 I  work1 I                 */
/*       I        I  word2 I  word2 I  word2 I                 */
/*       I        I  word3 I  word3 I  word3 I                 */
/*       ----------------------------------------              */
/*       I        I  word0 I  word0 I  word0 I                 */
/*       I  set1  I  word1 I  word1 I  work1 I                 */
/*       I        I  word2 I  word2 I  word2 I                 */
/*       I        I  word3 I  word3 I  word3 I                 */
/*       ----------------------------------------              */
/*                              			       */
/*                                                             */
/***************************************************************/
static void xdump(struct sce212cache* cache)
{
  int data_b, set_b;
  int n_ways, n_sets;
  uint32_t line;

  data_b = cache->n_data_bits;
  set_b = cache->n_set_bits;
  n_ways = cache->n_ways;
  n_sets = 1 << set_b;

  printf("Cache Tags:\n");
  printf("-------------------------------------\n");

  for (int w = 0; w < n_ways; w++) {
    if (w == 0) {
      printf("    ");
    }
    printf("      WAY[%d]", w);
  }
  printf("\n");
  for (int s = 0; s < n_sets; s++) {
    printf("SET[%d]:   ", s);

    for (int w = 0; w < n_ways; w++) {
      if (cache->sets[s].lines[w].valid) {
        
        line = cache->sets[s].lines[w].tag << (set_b + data_b);
        line = line | (s << data_b);  // tag 주소에 s로 index한 set_b로 or해서 line복원
      }
      else {
        line = 0;
      }
      printf("0x%08x  ", line);
    }
    printf("\n");
  }
  printf("\n");
}

int main(int argc, char *argv[])
{
  int capacity = 1024;
  int n_ways = 8;
  int block_size = 8;

  // Cache
  struct sce212cache *simCache;

  // Counts
  int read = 0, write = 0, writeback = 0;
  int readhit = 0, writehit = 0;
  int readmiss = 0, writemiss = 0;

  // Input option
  int opt = 0;
  char* token;
  int xflag = 0;

  // Parse file
  char *trace_name;
  FILE *fp;
  char line[16];
  char *op;
  uint32_t addr;

  /* You can define any variables that you want */
  trace_name = argv[argc-1];

  while((opt = getopt(argc, argv, "c:x")) != -1) {
    switch(opt) {
      case 'c':
        token = strtok(optarg, ":");
        capacity = (int)strtol(token, NULL, 10);
        token = strtok(NULL, ":");
        n_ways = (int)strtol(token, NULL, 10);
        token = strtok(NULL, ":");
        block_size = (int)strtol(token, NULL, 10);
        break;

      case 'x':
        xflag = 1;
        break;
    }
  }

  if (optind+1 > argc) {
      fprintf(stderr,  "Usage: %s [-c cap:assoc:block_size] [-x] input_trace \n", argv[0]);
      exit(EXIT_FAILURE);
  }


  /* TODO: Define a cache based on the struct declaration */
  build_cache(&simCache, block_size, n_ways, capacity);

  // Simulate
  fp = fopen(trace_name, "r"); // read trace file
  if (fp == NULL) {
    printf("Failed to read  trace file: %s\n", trace_name);
    return 1;
  }

  cdump(capacity, n_ways, block_size);

  /* TODO: Build an access function to load and store data from the file */
  while (fgets(line, sizeof(line), fp) != NULL) {
    // printf("line = %s",line);
    op = strtok(line, " ");
    addr = strtoull(strtok(NULL, ","), NULL, 16);

#ifdef DEBUG
    // You can use #define DEBUG above for seeing traces of the file.
    fprintf(stderr, "op: %s\n", op);
    fprintf(stderr, "addr: %x\n", addr);
#endif

    if (strcmp(op, "R") == 0){
      read++;
      access_cache(simCache, 0, addr, &readhit, &readmiss, &writeback);
    }
    else if (strcmp(op, "W") == 0){
      write++;
      access_cache(simCache, 1, addr, &writehit, &writemiss, &writeback);
    }
    else
      continue;
  }

  // test example
  sdump(read, write, writeback, readhit, writehit, readmiss, writemiss);
  if (xflag) {
    xdump(simCache);
  }

  return 0;
}
