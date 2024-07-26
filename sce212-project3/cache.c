#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#include "cache.h"

static int index_bits(int n){
  int cnt = 0;

  while(n) {
    cnt++;
    n = n >> 1;
  }

  return cnt-1;
}

/***************************************************************/
/*                                                             */
/* Procedure : build_cache                                     */
/*                                                             */
/* Purpose   : Initialize cache structure                      */
/*                                                             */
/***************************************************************/
void build_cache(struct sce212cache** cache, int block_size, int n_ways, int capacity)
{
  int set_num = capacity/(n_ways*block_size);
  *cache = (struct sce212cache*)malloc(sizeof(struct sce212cache));
  (*cache)->sets = (struct cache_set*)malloc(set_num * sizeof(struct cache_set));
  (*cache)->n_ways = n_ways;
  (*cache)->n_set_bits = index_bits(set_num);
  (*cache)->n_data_bits = index_bits(n_ways);

  for(int i=0; i<set_num; i++)
  {
    (*cache)->sets[i].lines = malloc(sizeof(struct cache_line)*n_ways);
    for(int j=0;j<n_ways;j++)
    {
      (*cache)->sets[i].lines[j].valid = 0;
    }
  }
  // /* Implement this function */
}

/***************************************************************/
/*                                                             */
/* Procedure : access_cache                                    */
/*                                                             */
/* Purpose   : Update cache stat and content                   */
/*                                                             */
/***************************************************************/
void access_cache(struct sce212cache* cache, int op, uint32_t addr, int* hit, int* miss, int* wb)
{
  // R : op=0, W : op=1
  /* Implement this function */
  int tag = (addr>>7) & 0x3fffff;
  int set_b = (addr>>3) & 0xf;

  for(int w=0; w<cache->n_ways; w++) // age +1
      {
        if(cache->sets[set_b].lines[w].valid == 1)
        {
          cache->sets[set_b].lines[w].age++;
        }
      }

  switch(op)
  {
    case 0: // R
      
      for(int w=0; w<cache->n_ways; w++) // start detec
      {
        if(cache->sets[set_b].lines[w].tag == tag) // read hit
        {
          (*hit)++;
          cache->sets[set_b].lines[w].age = 0;
          return;
        }
        else if(cache->sets[set_b].lines[w].tag == 0)  // read miss - set의 way중 valid를 차지하지 않은 걸 발견했을 때
        {
          cache->sets[set_b].lines[w].tag = tag;
          cache->sets[set_b].lines[w].valid = 1;
          cache->sets[set_b].lines[w].age = 0;
          cache->sets[set_b].lines[w].dirty = 0;
          (*miss)++;
          return;
        }
        else if(w == cache->n_ways-1) // 퇴출
        {
          int oldest = 0;
          for(int w = 1; w < cache->n_ways; w++) // oldest 인덱스 찾기
          {
            if(cache->sets[set_b].lines[w].age > cache->sets[set_b].lines[oldest].age)
            {
              oldest = w;
            }
          }
          if(cache->sets[set_b].lines[oldest].dirty)
          {
            (*wb)++;
          }
          cache->sets[set_b].lines[oldest].tag = tag;
          cache->sets[set_b].lines[oldest].valid = 1;
          cache->sets[set_b].lines[oldest].age = 0;
          cache->sets[set_b].lines[oldest].dirty = 0;
          (*miss)++;
          return;
        }

      }
      break;

    case 1: // W
      for(int w=0; w<cache->n_ways; w++)
      {
        if(cache->sets[set_b].lines[w].tag == tag) // write hit
        {
          (*hit)++;
          cache->sets[set_b].lines[w].age = 0;
          cache->sets[set_b].lines[w].dirty = 1;
          return;
        }
        else if(cache->sets[set_b].lines[w].tag == 0)  // write miss
        {
          cache->sets[set_b].lines[w].tag = tag;
          cache->sets[set_b].lines[w].valid = 1;
          cache->sets[set_b].lines[w].age = 0;
          cache->sets[set_b].lines[w].dirty = 1;
          (*miss)++;
          return;
        }
        else if(w == cache->n_ways-1) // 퇴출
        {
          int oldest = 0;
          for(int w = 1; w < cache->n_ways; w++) // oldest 인덱스 찾기
          {
            if(cache->sets[set_b].lines[w].age > cache->sets[set_b].lines[oldest].age)
            {
              oldest = w;
            }
          }
          if(cache->sets[set_b].lines[oldest].dirty)
          {
            (*wb)++;
          }
          cache->sets[set_b].lines[oldest].tag = tag;
          cache->sets[set_b].lines[oldest].valid = 1;
          cache->sets[set_b].lines[oldest].age = 0;
          cache->sets[set_b].lines[oldest].dirty = 1;
          (*miss)++;
          return;
        }
      }
      break;
  }
  
}