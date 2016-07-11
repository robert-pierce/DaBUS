/**********************************************
****** 		DO NOT MODIFY THIS FILE		*******
***********************************************/

#ifndef CACHESIM_H
#define CACHESIM_H

#include <inttypes.h>

typedef struct {
    int valid;
    int dirty;
    uint64_t tag;
} block_t;

typedef struct {
    int lines;
    block_t* blocks;
} set_t;


typedef struct LRU_node_t {
    int set_index;
    struct  LRU_node_t* next;
} LRU_node_t;

typedef struct {
  LRU_node_t* head;
} LRU_head_t;

typedef struct {
    int associativity;   // 2^S
    int lines;           // 2^(C-B-S)
    set_t* sets;    
    LRU_head_t* LRU;
    uint64_t C;
    uint64_t B;
    uint64_t S;   
} cache_t;


struct cache_stats_t {
    uint64_t accesses;
    uint64_t reads;
    uint64_t read_misses;
    uint64_t writes;
    uint64_t write_misses;
    uint64_t misses;
    uint64_t write_backs;
    uint64_t access_time;
    uint64_t miss_penalty;
    double   miss_rate;
    double   avg_access_time;
};


void cache_init(uint64_t C, uint64_t S, uint64_t B);
void cache_access (char rw, uint64_t address, struct cache_stats_t *stats);
void cache_cleanup (struct cache_stats_t *stats, uint64_t S);
block_t * block_search(uint64_t address, cache_t *cache);
uint64_t index_decode(uint64_t address);
uint64_t tag_decode(uint64_t address);
void cache_hit(block_t* block, char rw, uint64_t address, struct cache_stats_t *stats);
void cache_miss(char rw, uint64_t address, struct cache_stats_t *stats);
block_t* find_invalid_block(uint64_t address);
block_t* find_LRU_block(uint64_t address);
void update_LRU(uint64_t address, int set_index);



static const uint64_t DEFAULT_C = 15;   /* 32KB Cache */
static const uint64_t DEFAULT_B = 5;    /* 32-byte blocks */
static const uint64_t DEFAULT_S = 3;    /* 8 blocks per set */
static const uint64_t ADDRESS_LENGTH = 64;  /* 64 bit addresses */

static const char READ = 'r';
static const char WRITE = 'w';

#endif
