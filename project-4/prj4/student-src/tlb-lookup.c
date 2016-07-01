#include <stdlib.h>
#include <stdio.h>
#include "tlb.h"
#include "pagetable.h"
#include "global.h" /* for tlb_size */
#include "statistics.h"

/*******************************************************************************
 * Looks up an address in the TLB. If no entry is found, attempts to access the
 * current page table via cpu_pagetable_lookup().
 *
 * updated summer 2016.
 *
 * @param vpn The virtual page number to lookup.
 * @param write If the access is a write, this is 1. Otherwise, it is 0.
 * @return The physical frame number of the page we are accessing.
 */
pfn_t tlb_lookup(vpn_t vpn, int write) {
   pfn_t pfn;
   int tlb_index; 
   int LRU_index = 0;
   uint8_t found = 0;
   /* 
    * FIX ME : Step 6
    */

   /* 
    * Search the TLB for the given VPN. Make sure to increment count_tlbhits if
    * it was a hit!
    */
   //-------------------------MY CODE------------------------------------------------// 
   for (tlb_index = 0; tlb_index < tlb_size; tlb_index++) {
     if((tlb[tlb_index].valid) && (tlb[tlb_index].vpn == vpn)) {
       pfn = tlb[tlb_index].pfn;
       tlb[tlb_index].used = (tlb[tlb_index].used + 1);
       if(write) {
         tlb[tlb_index].dirty = 1;
       }
       count_tlbhits++;
       found = 1;
     }
   }
   //--------------------------------------------------------------------------------//

   //------------------------- EVICTION ----------------------------------------//
   /* If it does not exist (it was not a hit), call the page table reader */
   if(!found) {
     pfn = pagetable_lookup(vpn, write);
   }

   /* 
    * Replace an entry in the TLB if we missed. Pick invalid entries first,
    * if no invalid entry then find the LRU entry as victim.
    */
   //---------------------------MY CODE-----------------------------------------//
   if(!found) {
     uint8_t LRU_value = tlb[0].used;
     for (tlb_index = 0; tlb_index < tlb_size; tlb_index++) {  // iterate through tlb
       if (!(tlb[tlb_index].valid)) {                          // if invalid then you found the victim
         tlb[tlb_index].vpn = vpn;                             // victimize
         tlb[tlb_index].pfn = pfn;
         tlb[tlb_index].valid = 1;
         tlb[tlb_index].used = 0;
         if(write) {
           tlb[tlb_index].dirty = 1; 
         } else {
           tlb[tlb_index].dirty = 0;
         }
         found = 1;
         break;
       }
       if (LRU_value > tlb[tlb_index].used) {                // implements LRU
         LRU_index = tlb_index;
         LRU_value = tlb[tlb_index].used;
       }
     }
   }

   // evict victim found with LRU
   if(!found) {
     tlb[LRU_index].vpn = vpn;
     tlb[LRU_index].pfn = pfn;
     tlb[LRU_index].valid = 1;
     tlb[tlb_index].used = 0;

     if(write) {
       tlb[LRU_index].dirty = 1;
     } else {
       tlb[LRU_index].dirty = 0;
     }
   }
   //-----------------------------------------------------------------------------//

   /*
    * Perform TLB house keeping. This means marking the found TLB entry as
    * accessed and if we had a write, dirty. We also need to update the page
    * table in memory with the same data.
    *
    * We'll assume that this write is scheduled and the CPU doesn't actually
    * have to wait for it to finish (there wouldn't be much point to a TLB if
    * we didn't!).
    */
   if(write) {
     current_pagetable[vpn].dirty = 1;
   }


   return pfn;
}

