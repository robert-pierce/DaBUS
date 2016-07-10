#include "cachesim.h"
#include <stdlib.h>


//-----------Global Varaibles---------------------- 
cache_t* cache;
//--------------------------------------------------


/**
 * Subroutine for initializing your cache with the paramters.
 * You may add and initialize any global variables that you might need
 *
 * @param C The total size of your cache is 2^C bytes
 * @param S The total number of sets in your cache are 2^S
 * @param B The size of your block is 2^B bytes
 */
void cache_init(uint64_t C, uint64_t S, uint64_t B) {
  int i, j;
  set_t* set;
  LRU_head_t* curr_LRU_head;
  LRU_node_t* curr_LRU_node;

  /*Allocate space for the cache structure */
  cache = malloc(sizeof(cache_t));
  
  /*Init cache */
  cache->C = C;
  cache->B = B;
  cache->S = S;
  cache->associativity = 1 << S; // 2^S
  cache->lines = 1 << (C-B-S);   // 2^(C-B-S)
  cache->sets = calloc(cache->associativity, sizeof(set_t));
  cache->LRU = calloc(cache->lines, sizeof(LRU_head_t));


  /*Init Sets */
  for(i = 0; i < (cache->associativity); i++) {
    set = cache->sets + i;
    set->lines = 1 << (C-B-S); // 2^(C-B-S)
    set->blocks = calloc(set->lines, sizeof(block_t));  // allocate the blocks in each set
  }

  /*Init LRU*/
  for(i = 0; i < (cache->lines); i++){  // set up head sentinels for each LRU list (one per line)
    curr_LRU_head = cache->LRU + i;
    curr_LRU_head->head = calloc(sizeof(LRU_node_t));  // init the first node in each list
    curr_LRU_node = curr_LRU_head->head;
    for(j = 0; j < (cache->associativity); j++) {   // create each node after the first in each LRU list
      curr_LRU_node->set_index = j;                      // init the LRU_node set_index
      if(j != cache->associativity - 1) {                // special case when at tail 
        curr_LRU_node->next = calloc(sizeof(LRU_node_t)); // init the next member of the list
      curr_LRU_node = curr_LRU_node->next;              // walk to next node
      }  else {
        curr_LRU_node->next = NULL;                       // at the tail, set next to NULL
      }
    }
  }
}

/**
 * Subroutine that simulates one cache event at a time.
 * @param rw The type of access, READ or WRITE
 * @param address The address that is being accessed
 * @param stats The struct that you are supposed to store the stats in
 */
void cache_access (char rw, uint64_t address, struct cache_stats_t *stats) {
  block_t* curr_block;
  
  stats->accesses++;                           // update stats
  curr_block = block_search(address, cache);  // search the cache for a hit

  if(curr_block == NULL) {          
    cache_miss(rw, address, stats); // cache miss :(
  } else {
    cache_hit(curr_block, rw, address, stats);  // cache hit!
  }


}

/**
 * Subroutine for cleaning up memory operations and doing any calculations
 *
 */
void cache_cleanup (struct cache_stats_t *stats, uint64_t S) {

}

/**
 * Subroutine for searching for a matching block (cache hit)
 *
 * @param address The memory address you are looking up
 * @param cache Pointer to the cache
 * @return Pointer to the matching block or NULL if cache miss
 */
block_t* block_search(uint64_t address, cache_t *cache) {
  int i, j;
  set_t* curr_set;
  block_t* curr_block;
  int index = index_decode(address);
  int tag = tag_decode(address);
  
  for(i = 0; i < (cache->associativity), i++) {      // iterate across all sets
    curr_set = cache->sets + i;
    curr_block = curr_set->blocks + index;           // get the current block
    if(curr_block->valid && curr_block->tag == tag){ // cache hit!
      
      update_LRU(address, i);                        // update the LRU

      return curr_block;
    }
  }
  return NULL;  // cache miss :(
}

/**
 * Subroutine for search for decoding the index from the address
 *
 * @param address The memory address you are looking up
 * @param cache Pointer to the cache
 * @return The index (int)
 */
int index_decode(uint64_t address) {
  int index_bits = (cache->C) - (cache->B) - (cache->S);
  int offset_bits = cache->B;

  // create mask, shift it to the right spot, AND with address, shift back
  return (address & (((1 << index_bits) - 1) << offset_bits)) >> offset_bits; 
}

/**
 * Subroutine for decoding the tag from the address
 *
 * @param address The memory address you are looking up
 * @param cache Pointer to the cache
 * @return The tag (int)
 */
int tag_decode(uint64_t address) {
  int index_bits = (cache->C) - (cache->B) - (cache->S);
  int offset_bits = cache->B;
  int tag_bits = ADDRESS_LENGTH - index_bits - offset_bits;

  // create mask, shift it to the right spot, AND with address, shift back
  return (address & (((1 << tag_bits) - 1) << (offset_bits + index_bits))) >> (offset_bits + index_bits);
}

/**
 * Subroutine for handling a cache hit
 *
 * @param block The current block where the cache successfully hit
 * @param rw The current memory operation (READ or WRITE)
 * @param address The memory address you are looking up
 * @param stats Pointer to the stats struct 
 */
void cache_hit(block_t block, char rw, uint64_t address, struct cache_stats_t *stats) {
  if (rw == WRITE) {             // a Write operation
    block->dirty = 1;            // mark current block as dirty
    stats->reads++;   
  } else {                       // a Read operation
    stats->writes++;             // update stats
  }
  
  
}

void cache_miss(char rw, uint64_t address, struct cache_stats_t *stats) {
  block_t* victim_block;
  
  //--------- update stats -----------------------//
  stats->misses++;
  if (rw == WRITE) {             // a Write operation
    stats->read_misses++;   
  } else {                       // a Read operation
    stats->write_misses++;       // update stats
  }
  //----------------------------------------------//
  
  victim_block = find_invalid_block(address);
  
  if (victim_block == NULL) {                   // no invalid blocks, must evict LRU
    victim_block = find_LRU_block(address); 
  }
  
  // IMPLEMENT   swap blocks, update valid

}

block_t* find_invalid_block(uint64_t address) {
  int i;
  set_t* curr_set; 
  block_t* curr_block;
  int index = index_decode(address);

  // search for an invalid block first
  for(i = 0; i < (cache->associativity); i++) {
    curr_set = cache->sets + i;
    curr_block = curr_set->blocks + index;
    if(!curr_block->valid) {


      return curr_block;      // found and invalid block
    }
  }
  return NULL;                // no invalid blocks
}

block_t* find_LRU_block(uint64_t address) {
  LRU_head_t* LRU_list;
  set_t* set;
  int set_index;
  int index = index_decode(address);
  
  LRU_list = cache->LRU + index;             // get the right LRU for the line index
  set_index = (LRU_list->head)->set_index;   // get the set_index from LRU head node

  set = cache->sets + set_index;             // get the set that holds the LRU victim

  update_LRU(address, set_index);  // IMPLEMENT!!!

  return set->blocks + index;                // return the correct block from the set
}

void update_LRU(uint64_t address, int set_index) {
  LRU_head_t* LRU_list;
  LRU_node_t* curr_node;
  set_t* set;
  int hit_fLAG =  0;
  int index = index_decode(address);
  
  LRU_list = cache->LRU + index;             // get the right LRU for the line index
  curr_node = LRU_list->head;                // get the first LRU node for this list
  
  while(curr_node->next != NULL) {           // walk the list 
    if(curr_node->set_index == set_index) {  // looking for the most recent set_index
      hit_flag = 1;                          // if found, mark a flag 
    }
    
    if(hit_flag) {                           // if found, bubble down to tail of list
      curr_node->set_index = (curr_node->next)->set_index;
    }   

    curr_node = curr_node->next;             // walk to next node
  }
}
