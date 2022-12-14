/*
 * chunk_operations.c
 *
 *  Created on: Oct 12, 2022
 *      Author: HP
 */

#include <kern/trap/fault_handler.h>
#include <kern/disk/pagefile_manager.h>
#include "kheap.h"
#include "memory_manager.h"


/******************************/
/*[1] RAM CHUNKS MANIPULATION */
/******************************/

//===============================
// 1) CUT-PASTE PAGES IN RAM:
//===============================
//This function should cut-paste the given number of pages from source_va to dest_va
//if the page table at any destination page in the range is not exist, it should create it
//Hint: use ROUNDDOWN/ROUNDUP macros to align the addresses
int cut_paste_pages(uint32* page_directory, uint32 source_va, uint32 dest_va, uint32 num_of_pages)
{
	//TODO: [PROJECT MS2] [CHUNK OPERATIONS] cut_paste_pages
	// Write your code here, remove the panic and write your code
	//panic("cut_paste_pages() is not implemented yet...!!");
	for (int i = 0; i < num_of_pages; i++)
	{
		uint32 *ptr_page_table_dest = NULL;
		struct FrameInfo * dest_frame = get_frame_info(page_directory, dest_va + (i*PAGE_SIZE), &ptr_page_table_dest);
		if (dest_frame != NULL)
		{
			return -1;
		}
	}
	for (int i = 0; i < num_of_pages; i++)
	{
		uint32 *ptr_page_table_source = NULL;
		struct FrameInfo * source_frame = get_frame_info(page_directory, source_va + (i*PAGE_SIZE), &ptr_page_table_source);
		map_frame(page_directory, source_frame, dest_va + (i*PAGE_SIZE),pt_get_page_permissions(page_directory, source_va + (i*PAGE_SIZE)));
		unmap_frame(page_directory, source_va + (i*PAGE_SIZE));
	}
	return 0;
}

//===============================
// 2) COPY-PASTE RANGE IN RAM:
//===============================
//This function should copy-paste the given size from source_va to dest_va
//if the page table at any destination page in the range is not exist, it should create it
//Hint: use ROUNDDOWN/ROUNDUP macros to align the addresses
int copy_paste_chunk(uint32* page_directory, uint32 source_va, uint32 dest_va, uint32 size)
{
	//TODO: [PROJECT MS2] [CHUNK OPERATIONS] copy_paste_chunk
	// Write your code here, remove the panic and write your code
	//panic("copy_paste_chunk() is not implemented yet...!!");
	for (int i = dest_va; i <dest_va + size ; i+=PAGE_SIZE)
	{
		uint32 *ptr_page_table_dest = NULL;
		struct FrameInfo * dest_frame = get_frame_info(page_directory, i , &ptr_page_table_dest);
		if (dest_frame != NULL && (pt_get_page_permissions(page_directory,i)&PERM_WRITEABLE)== 0)
		{
			return -1;
		}
	}
	for (uint32 i = source_va, j = dest_va; i < source_va + size;)
	{
		uint32 *ptr_page_table_dest = NULL;
		struct FrameInfo * dest_frame = get_frame_info(page_directory, j , &ptr_page_table_dest);
		if (dest_frame == NULL)
		{
			struct FrameInfo * temp_frame;
			int result = allocate_frame(&temp_frame);
			uint32 *ptr_page_table_source = NULL;
			get_page_table(page_directory, i, &ptr_page_table_source);
			int user = ptr_page_table_source[PTX(i)] & PERM_USER;
			map_frame(page_directory,temp_frame,j,PERM_WRITEABLE|user);
			temp_frame->va = j;
		}
		for (int k = 0; k < PAGE_SIZE; k++)
		{
			if (i > source_va + size || j > dest_va + size)
			{
				return 0;
			}
			unsigned char *ptr_src = (unsigned char *)(i);
			unsigned char *ptr_dest = (unsigned char *)(j);
			*ptr_dest = *ptr_src;
			i++;
			j++;
		}
	}
	return 0;
}
//===============================
// 3) SHARE RANGE IN RAM:
//===============================
//This function should share the given size from dest_va with the source_va
//Hint: use ROUNDDOWN/ROUNDUP macros to align the addresses
int share_chunk(uint32* page_directory, uint32 source_va,uint32 dest_va, uint32 size, uint32 perms)
{
	//TODO: [PROJECT MS2] [CHUNK OPERATIONS] share_chunk
	// Write your code here, remove the panic and write your code
	//panic("share_chunk() is not implemented yet...!!");
	uint32 start_dst_va =ROUNDDOWN(dest_va,PAGE_SIZE);
	uint32 end_dst_va =ROUNDUP(dest_va + size,PAGE_SIZE);
	uint32 start_src_va =ROUNDDOWN(source_va,PAGE_SIZE);
	uint32 end_src_va =ROUNDUP(source_va + size,PAGE_SIZE);
	for (int i = start_dst_va; i <end_dst_va ; i+=PAGE_SIZE)
	{
		uint32 *ptr_page_table_dest = NULL;
		struct FrameInfo * dest_frame = get_frame_info(page_directory, i, &ptr_page_table_dest);
		if (dest_frame != NULL)
		{
			return -1;
		}
	}
	for (int i = start_src_va, j = start_dst_va; i < end_src_va; i+=PAGE_SIZE, j+=PAGE_SIZE)
	{
		uint32 *ptr_page_table_source = NULL;
		struct FrameInfo * source_frame = get_frame_info(page_directory, i, &ptr_page_table_source);
		map_frame(page_directory, source_frame, j, perms);
	}
	return 0;
}

//===============================
// 4) ALLOCATE CHUNK IN RAM:
//===============================
//This function should allocate in RAM the given range [va, va+size)
//Hint: use ROUNDDOWN/ROUNDUP macros to align the addresses
int allocate_chunk(uint32* page_directory, uint32 va, uint32 size, uint32 perms)
{
	//TODO: [PROJECT MS2] [CHUNK OPERATIONS] allocate_chunk
	// Write your code here, remove the panic and write your code
	//panic("allocate_chunk() is not implemented yet...!!");
	uint32 start_va =ROUNDDOWN(va,PAGE_SIZE);
	uint32 end_va =ROUNDUP(va + size,PAGE_SIZE);
	for(uint32 i =start_va; i<end_va; i+=PAGE_SIZE)
	{
		uint32 *ptr_page_table = NULL;
		struct FrameInfo * dest_page = get_frame_info(page_directory, i, &ptr_page_table);
		if (dest_page != NULL)
		{
			return -1;
		}
	}
	for(uint32 i =start_va; i<end_va; i+=PAGE_SIZE)
	{
		struct FrameInfo *ptr = NULL;
		allocate_frame(&ptr);
		map_frame(page_directory,ptr,i,perms);
		ptr->va = i;
	}

	return 0;

}

/*BONUS*/
//=====================================
// 5) CALCULATE ALLOCATED SPACE IN RAM:
//=====================================
void calculate_allocated_space(uint32* page_directory, uint32 sva, uint32 eva,
		uint32 *num_tables, uint32 *num_pages) {
	//TODO: [PROJECT MS2 - BONUS] [CHUNK OPERATIONS] calculate_allocated_space
	// Write your code here, remove the panic and write your code
	//panic("calculate_allocated_space() is not implemented yet...!!");

	uint32 *ptr_p_table = NULL;
	uint32 page_cntr = 0, table_cntr = 0;
	uint32 start_table = ROUNDDOWN(sva, PAGE_SIZE*1024);
	uint32 start = ROUNDDOWN(sva, PAGE_SIZE);
	while(start < ROUNDUP(eva, PAGE_SIZE))
	{
		ptr_p_table = NULL;
		if (start_table < ROUNDUP(eva, PAGE_SIZE))
		{
			get_page_table(page_directory, start_table, &ptr_p_table);
			if (ptr_p_table != NULL)
				table_cntr++;
			start_table+=PAGE_SIZE*1024;
		}
		struct FrameInfo * page = get_frame_info(page_directory, start, &ptr_p_table);
		if(page != NULL)
			page_cntr++;
		start += PAGE_SIZE;
	}


	*num_pages = page_cntr;
	*num_tables = table_cntr;
}

/*BONUS*/
//=====================================
// 6) CALCULATE REQUIRED FRAMES IN RAM:
//=====================================
// calculate_required_frames:
// calculates the new allocation size required for given address+size,
// we are not interested in knowing if pages or tables actually exist in memory or the page file,
// we are interested in knowing whether they are allocated or not.
uint32 calculate_required_frames(uint32* page_directory, uint32 sva, uint32 size)
{
	//TODO: [PROJECT MS2 - BONUS] [CHUNK OPERATIONS] calculate_required_frames
	// Write your code here, remove the panic and write your code
	//panic("calculate_required_frames() is not implemented yet...!!");
	uint32 start_frames = ROUNDDOWN(sva, PAGE_SIZE);
	uint32 start_tables = ROUNDDOWN(sva, PAGE_SIZE*1024);
	uint32 end = ROUNDUP(sva + size, PAGE_SIZE);
	uint32 num_of_frames = 0;
	while (start_frames < end)
	{
		uint32 *ptr_page_table = NULL;
		struct FrameInfo * dest_page = get_frame_info(page_directory, start_frames, &ptr_page_table);
		if (dest_page == NULL)
			num_of_frames +=1;
		if (start_tables < end)
		{
			uint32 *page_table_info = NULL;
			get_frame_info(page_directory, start_tables, &page_table_info);
			if (page_table_info == NULL)
				num_of_frames +=1;
			start_tables+=PAGE_SIZE*1024;
		}
		start_frames+=PAGE_SIZE;
	}
	return num_of_frames;
}

//=================================================================================//
//===========================END RAM CHUNKS MANIPULATION ==========================//
//=================================================================================//

/*******************************/
/*[2] USER CHUNKS MANIPULATION */
/*******************************/

//======================================================
/// functions used for USER HEAP (malloc, free, ...)
//======================================================

//=====================================
// 1) ALLOCATE USER MEMORY:
//=====================================
void allocate_user_mem(struct Env* e, uint32 virtual_address, uint32 size)
{
	// Write your code here, remove the panic and write your code
	panic("allocate_user_mem() is not implemented yet...!!");
}

//=====================================
// 2) FREE USER MEMORY:
//=====================================
void free_user_mem(struct Env* e, uint32 virtual_address, uint32 size)
{
    //TODO: [PROJECT MS3] [USER HEAP - KERNEL SIDE] free_user_mem
    // Write your code here, remove the panic and write your code
    //panic("free_user_mem() is not implemented yet...!!");
	virtual_address = ROUNDDOWN(virtual_address, PAGE_SIZE);
	uint32 end = ROUNDUP(virtual_address + size, PAGE_SIZE);

   //This function should:
    //1. Free ALL pages of the given range from the Page File
    for(uint32 i = virtual_address; i < end; i+=PAGE_SIZE )
    {
        pf_remove_env_page(e, i);
    }
    //2. Free ONLY pages that are resident in the working set from the memory

    for(uint32 i = virtual_address; i < end; i+=PAGE_SIZE)
    {
    	unmap_frame(e->env_page_directory, i);
    	env_page_ws_invalidate(e, i);
    }

    //3. Removes ONLY the empty page tables (i.e. not used) (no pages are mapped in the table)
	uint32 start_tables = ROUNDDOWN(virtual_address, PAGE_SIZE*1024);
	uint32 end_tables = ROUNDUP(virtual_address + size, PAGE_SIZE*1024);
	for (uint32 i = start_tables; i < end_tables; i += PAGE_SIZE*1024)
	{
		bool found = 0;
		uint32 * ptr_page_table= NULL;
		get_page_table(e->env_page_directory, i, &ptr_page_table);
		if(ptr_page_table != NULL)
		{
			for(int j = i; j < i + 1024*PAGE_SIZE; j+=PAGE_SIZE)
			{

				struct FrameInfo *fr = get_frame_info(e->env_page_directory, j, &ptr_page_table);
				if (fr != NULL)
				{
					found = 1;
					break;
				}
			}
		    if(found == 0)
			{
		    	kfree((void *) kheap_virtual_address(e->env_page_directory[PDX(i)]));
				e->env_page_directory[PDX(i)] = 0;
			}
		}
	}
}

//=====================================
// 2) FREE USER MEMORY (BUFFERING):
//=====================================
void __free_user_mem_with_buffering(struct Env* e, uint32 virtual_address, uint32 size)
{
	// your code is here, remove the panic and write your code
	panic("__free_user_mem_with_buffering() is not implemented yet...!!");

	//This function should:
	//1. Free ALL pages of the given range from the Page File
	//2. Free ONLY pages that are resident in the working set from the memory
	//3. Free any BUFFERED pages in the given range
	//4. Removes ONLY the empty page tables (i.e. not used) (no pages are mapped in the table)
}

//=====================================
// 3) MOVE USER MEMORY:
//=====================================
void move_user_mem(struct Env* e, uint32 src_virtual_address, uint32 dst_virtual_address, uint32 size)
{
	//TODO: [PROJECT MS3 - BONUS] [USER HEAP - KERNEL SIDE] move_user_mem
	//your code is here, remove the panic and write your code
	panic("move_user_mem() is not implemented yet...!!");

	// This function should move all pages from "src_virtual_address" to "dst_virtual_address"
	// with the given size
	// After finished, the src_virtual_address must no longer be accessed/exist in either page file
	// or main memory

	/**/
}

//=================================================================================//
//========================== END USER CHUNKS MANIPULATION =========================//
//=================================================================================//

