/* *********************************************************** */
/* MAKE SURE PAGE_WS_MAX_SIZE = 20 */
/* MAKE SURE Second_List_Size = 2 */
/*FIRST SCENARIO --> ACTIVE LIST NOT FULL*/
/* *********************************************************** */

#include <inc/lib.h>

void _main(void)
{

	//	cprintf("envID = %d\n",envID);
	char arr[PAGE_SIZE*1024*4];

	uint32 actual_active_list[17] = {0xedbfdb78,0xeebfd000,0x803000,0x802000,0x801000,0x800000,0x205000,0x204000,0x203000,0x202000,0x201000,0x200000};
	uint32 actual_second_list[2] = {};
	("STEP 0: checking Initial LRU lists entries ...\n");
	{
		int check = sys_check_LRU_lists(actual_active_list, actual_second_list, 12, 0);
		if(check == 0)
			panic("INITIAL PAGE LRU LISTs entry checking failed! Review size of the LRU lists..!!");
	}

	int usedDiskPages = sys_pf_calculate_allocated_pages() ;
	int freePages = sys_calculate_free_frames();

	int i=0;
	for(;i<=PAGE_SIZE;i++)
	{
		arr[i] = -1;
	}

	i=PAGE_SIZE*1024;
	for(;i<=(PAGE_SIZE*1024 + PAGE_SIZE);i++)
	{
		arr[i] = -1;
	}

	i=PAGE_SIZE*1024*2;
	for(;i<=(PAGE_SIZE*1024*2 + PAGE_SIZE);i++)
	{
		arr[i] = -1;
	}



	cprintf("STEP A: checking PLACEMENT fault handling ... \n");
	{
		if( arr[0] !=  -1)  panic("PLACEMENT of stack page failed");
		if( arr[PAGE_SIZE] !=  -1)  panic("PLACEMENT of stack page failed");

		if( arr[PAGE_SIZE*1024] !=  -1)  panic("PLACEMENT of stack page failed");
		if( arr[PAGE_SIZE*1025] !=  -1)  panic("PLACEMENT of stack page failed");

		if( arr[PAGE_SIZE*1024*2] !=  -1)  panic("PLACEMENT of stack page failed");
		if( arr[PAGE_SIZE*1024*2 + PAGE_SIZE] !=  -1)  panic("PLACEMENT of stack page failed");

		if( (sys_pf_calculate_allocated_pages() - usedDiskPages) !=  5) panic("new stack pages are not written to Page File");

		if( (freePages - sys_calculate_free_frames() ) != 9 ) panic("allocated memory size incorrect");
	}
	cprintf("STEP A passed: PLACEMENT fault handling works!\n\n\n");

	for (int i=16;i>4;i--)
		actual_active_list[i]=actual_active_list[i-5];

	actual_active_list[0]=0xee3fe000;
	actual_active_list[1]=0xee3fdfa4;
	actual_active_list[2]=0xedffe000;
	actual_active_list[3]=0xedffdfa4;
	actual_active_list[4]=0xedbfe000;

	cprintf("STEP B: checking LRU lists entries ...\n");
	{
		int check = sys_check_LRU_lists(actual_active_list, actual_second_list, 17, 0);
			if(check == 0)
				panic("LRU lists entries are not correct, check your logic again!!");
	}
	cprintf("STEP B passed: LRU lists entries test are correct\n\n\n");

	cprintf("Congratulations!! Test of PAGE PLACEMENT FIRST SCENARIO completed successfully!!\n\n\n");
	return;
}
