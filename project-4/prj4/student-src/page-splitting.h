#ifndef _STUDENT_PAGE_SPLITTING_H_
#define _STUDENT_PAGE_SPLITTING_H_

/*******************************************************************************
 * Your assignment for problem 1 is to fix the following macros.
 * HINT: You have access to the global variable page_size, which tells you how
 *       many addresses a single page contains.
 * HINT: While page_size will be a power of two, allowing you to use bit-wise
 *       arithmetic, consider using modulus division and integer division if
 *       that is more intuitive for you.
 */

/*******************************************************************************
 * Get the page number from a virtual address.
 * 
 * @param addr The virtual address.
 * @return     The virtual page number for this addres.
 */
#define VADDR_PAGENUM(addr)\
({int psize = page_size;\
int off_bits = 0;\
while ( (psize >> 1) > 0) {\
off_bits = off_bits + 1;\
psize = (psize >> 1);} \
(addr >> off_bits);})
   

/*******************************************************************************
 * Get the offset for a particular address.
 * @param addr The virtual address.
 * @return     The offset into a page for the given virtual address.
 */
#define VADDR_OFFSET(addr)\
({int psize = page_size;\
int off_bits = 0;\
while ( (psize >> 1) > 0) {\
off_bits = off_bits + 1;\
psize = (psize >> 1);}\
(((1 << off_bits) - 1) & addr);})


#endif/*_STUDENT_PAGE_SPLITTING_H_*/
