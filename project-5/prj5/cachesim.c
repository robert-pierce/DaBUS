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
  cache->sets = (set_t *) calloc(cache->associativity, sizeof(set_t));
  cache->LRU = (LRU_head_t *) calloc(cache->lines, sizeof(LRU_head_t));


  /*Init Sets */
  for(i = 0; i < (cache->associativity); i++) {
    set = cache->sets + i;
    set->lines = 1 << (C-B-S); // 2^(C-B-S)
    set->blocks = (block_t *) calloc(set->lines, sizeof(block_t));  // allocate the blocks in each set
  }

  /*Init LRU*/
  for(i = 0; i < (cache->lines); i++){  // set up head sentinels for each LRU list (one per line)
    curr_LRU_head = cache->LRU + i;
    curr_LRU_head->head = (LRU_node_t *) calloc(1, sizeof(LRU_node_t));  // init the first node in each list
    curr_LRU_node = curr_LRU_head->head;
    for(j = 0; j < (cache->associativity); j++) {   // create each node after the first in each LRU list
      curr_LRU_node->set_index = j;                      // init the LRU_node set_index
      if(j != cache->associativity - 1) {                // special case when at tail 
        curr_LRU_node->next = (LRU_node_t *) calloc(1, sizeof(LRU_node_t)); // init the next member of the list
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
  
  //-------update stats--------------------------------//
  stats->accesses++;     
  if (rw == WRITE) {             // a Write operation
    stats->writes++;   
  } else {                       // a Read operation
    stats->reads++;             // update stats
  }
  //---------------------------------------------------//

  curr_block = block_search(address, cache);  // search the cache for a hit

  if(curr_block == NULL) {          
    cache_miss(rw, address, stats); // cache miss :(
     printf("clock:%" PRIu64 ", tag: %" PRIx64 ", index: %" PRIx64 ", %s, evicted block:%" PRId64 "\n",1, tag_decode(address), index_decode(address), "MISS" );
  } else {
    cache_hit(curr_block, rw, address, stats);  // cache hit!
    printf("clock:%" PRIu64 ", tag: %" PRIx64 ", index: %" PRIx64 ", %s, evicted block:%" PRId64 "\n",1, tag_decode(address), index_decode(address), "HIT"  );
  }
}

/**
 * Subroutine for cleaning up memory operations and doing any calculations
 *
 */
void cache_cleanup (struct cache_stats_t *stats, uint64_t S) {
  LRU_head_t *curr_LRU_head;
  LRU_node_t *curr_LRU_node, *temp_node;
  set_t* curr_set;
  int i;
  //----- Clean up memory----------------------//
  
  // Cleanup LRU
  for(i = 0; i < (cache->lines); i++) {   // walk through all the LRU_heads 
    curr_LRU_head = cache->LRU + i;        // get the LRU head
    curr_LRU_node = curr_LRU_head->head;  // get the head node of the LRU list for this line
    while(curr_LRU_node->next != NULL) {  // walk the LRU list 
      temp_node = curr_LRU_node;          
      curr_LRU_node = temp_node->next;    // get next node
      free(temp_node);                    // free the old node
    }
  }
  free(cache->LRU);                       // free the LRU

  // cleanup Sets
  for(i = 0; i < (cache->associativity); i++) {   // walk through all of the set
    curr_set = cache->sets +  i;                  // get the current set
    free(curr_set->blocks);                       // free its blocks                             
  }
  free(cache->sets);                              // free the sets

  free(cache);                             // free the cache
  //---------------------------------------------------------//

  //--------------- update stats-----------------------------//
  stats->miss_rate = ((double) (stats->misses)) / ((double)(stats->accesses));
  stats->avg_access_time = (stats->access_time)+ 0.2*S + (stats->miss_rate)*(stats->miss_penalty); 

}

/**
 * Subroutine for searching for a matching block (cache hit)
 *
 * @param address The memory address you are looking up
 * @param cache Pointer to the cache
 * @return Pointer to the matching block or NULL if cache miss
 */
block_t * block_search(uint64_t address, cache_t *cache) {
  int i;
  set_t* curr_set;
  block_t* curr_block;
  uint64_t index = index_decode(address);
  uint64_t tag = tag_decode(address);
  
  for(i = 0; i < (cache->associativity); i++) {      // iterate across all sets
    curr_set = cache->sets + i;
    curr_block = curr_set->blocks + index;           // get the current block
    if(curr_block->valid && curr_block->tag == tag){ // cache hit!
      
      // update_LRU(address, i);                        // update the LRU

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
uint64_t index_decode(uint64_t address) {
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
uint64_t tag_decode(uint64_t address) {
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
void cache_hit(block_t *block, char rw, uint64_t address, struct cache_stats_t *stats) {
  if (rw == WRITE) {             // a Write operation
    block->dirty = 1;            // mark current block as dirty
  } 
}

void cache_miss(char rw, uint64_t address, struct cache_stats_t *stats) {
  block_t* victim_block;
  
  victim_block = find_invalid_block(address);    // search for an invalid block first
  
  if(victim_block == NULL) {                   // no invalid blocks, must evict LRU
    victim_block = find_LRU_block(address); 
  }
  
  if(victim_block->dirty) {  // write back
    stats->write_backs++;    // transfer to memory 
  }

  //----- reset victim_block -------------------------//
  victim_block->dirty = 0;
  victim_block->valid = 1;
  victim_block->tag = tag_decode(address);   // set tag for new block 


  //--------- update stats -----------------------//
  stats->misses++;
  if (rw == WRITE) {             // a Write operation
    stats->write_misses++;   
  } else {                       // a Read operation
    stats->read_misses++;       // update stats
  }
}

block_t* find_invalid_block(uint64_t address) {
  int i;
  set_t* curr_set; 
  block_t* curr_block;
  uint64_t index = index_decode(address);

  // search for an invalid block first
  for(i = 0; i < (cache->associativity); i++) {
    curr_set = cache->sets + i;
    curr_block = curr_set->blocks + index;
    if(!curr_block->valid) {
      
      update_LRU(address, i); // update the LRU order

      return curr_block;      // found and invalid block
    }
  }
  return NULL;                // no invalid blocks
}

block_t* find_LRU_block(uint64_t address) {
  LRU_head_t* LRU_list;
  set_t* set;
  int set_index;
  uint64_t index = index_decode(address);
  
  LRU_list = cache->LRU + index;             // get the right LRU for the line index
  set_index = (LRU_list->head)->set_index;   // get the set_index from LRU head node

  set = cache->sets + set_index;             // get the set that holds the LRU victim

  update_LRU(address, set_index);            // update the LRU order

  return set->blocks + index;                // return the correct block from the set
}

void update_LRU(uint64_t address, int set_index) {
  LRU_head_t* LRU_list;
  LRU_node_t* curr_node;
  int hit_flag = 0;
  uint64_t index = index_decode(address);
  
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
  curr_node->set_index = set_index
}
