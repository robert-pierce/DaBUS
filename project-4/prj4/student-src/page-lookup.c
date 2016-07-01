#include "swapfile.h"
#include "statistics.h"
#include "pagetable.h"

/*******************************************************************************
 * Looks up an address in the current page table. If the entry for the given
 * page is not valid, increments count_pagefaults and traps to the OS.
 *
 * @param vpn The virtual page number to lookup.
 * @param write If the access is a write, this is 1. Otherwise, it is 0.
 * @return The physical frame number of the page we are accessing.
 */
pfn_t pagetable_lookup(vpn_t vpn, int write) {
   pfn_t pfn; 

   /*  Determine the PFN corresponding to the passed in VPN.
    * Perform the lookup using the current_pagetable.
    */
   pte_t *pte =  current_pagetable + vpn;
   pte->used = 1;                  // mark virtual frame as used
   if (pte->valid) {
     pfn = pte->pfn;                  // get physical frame number
   }
   else {                            // page fault!
     count_pagefaults++;              
     pfn = pagefault_handler(vpn, write);  // trap into pagefault_handler
   }

   return pfn;
}
