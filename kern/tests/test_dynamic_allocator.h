/*
 * test_dynamic_allocator.h
 *
 *  Created on: Oct 14, 2022
 *      Author: HP
 */

#ifndef KERN_TESTS_TEST_DYNAMIC_ALLOCATOR_H_
#define KERN_TESTS_TEST_DYNAMIC_ALLOCATOR_H_
#ifndef FOS_KERNEL
# error "This is a FOS kernel header; user programs should not #include it"
#endif

//2022
int  test_initialize_MemBlocksList();
void test_insert_sorted_allocList();
void test_find_block();
void test_alloc_block_FF();
void test_alloc_block_BF();
void test_alloc_block_NF();
void test_insert_sorted_with_merge_freeList();


#endif /* KERN_TESTS_TEST_DYNAMIC_ALLOCATOR_H_ */
