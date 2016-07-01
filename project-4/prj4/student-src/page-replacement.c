#include <stdlib.h>

#include "types.h"
#include "pagetable.h"
#include "global.h"
#include "process.h"

/*******************************************************************************
 * Finds a free physical frame. If none are available, uses a clock sweep
 * algorithm to find a used frame for eviction.
 *
 * updated summer 2016
 *
 * @return The physical frame number of a free (or evictable) frame.
 */
pfn_t get_free_frame(void) {
   int i;
   unsigned char *is_used;
   vpn_t pagetable_index;
   pte_t *current_pagetable;

   /* See if there are any free frames */
   for (i = 0; i < CPU_NUM_FRAMES; i++) {
     if (rlt[i].pcb == NULL) {
         return i;
     }
   }

   /* FIX ME : Problem 5 */
   /* IMPLEMENT A CLOCK SWEEP ALGORITHM HERE */
   /* Note: Think of what kinds of frames can you return before you decide
      to evict one of the pages using the clock sweep and return that frame.
      Note: This method is guaranteed to find a victim */
   
   //--------------------MY CODE----------------------------//
   i = 0; 
   //---- get the used bit of the pagetable_entry for the first pfn in the rlt
   pagetable_index = rlt[i].vpn;
   current_pagetable = (rlt[i].pcb)->pagetable;
   is_used = &((current_pagetable + pagetable_index)->used);
  
   while(*is_used)  {   //loop until finding a non-used virtual page
     (*is_used) = 0;
     
     i++;               // increment pfn index in frame table (reverse page table)         
     i =  i % CPU_NUM_FRAMES;
     
     // get the used bit of the pagetable entry from the next pfn in the rlt
     pagetable_index = rlt[i].vpn;
     current_pagetable = (rlt[i].pcb)->pagetable;
     is_used = &((current_pagetable + pagetable_index)->valid);
   }

   return i;  // return the victim physical frame number
}
