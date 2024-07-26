#pragma once

#include <stdint.h>
#include <stdbool.h>

/*
 * Cache structures
 */
struct cache_line
{
  uint32_t age;
  bool valid;
  bool dirty;
  uint32_t tag;
};

struct cache_set
{
  struct cache_line *lines;
};

struct sce212cache
{
  int n_set_bits;  // The number of bits that are used to index Set
  int n_data_bits; // The number of bits that are used to index Data
  int n_ways;
  struct cache_set *sets;
};

void build_cache(struct sce212cache** cache, int block_size, int n_ways, int capacity);

void access_cache(struct sce212cache* cache, int op, uint32_t addr, int* hit, int* miss, int* wb);
