# OS_project_2022-FOS-
FOS

## Table of Contents
1. [General Info](#general-info)
2. [Collaboration](#team-members)
3. [Honoring](#honoring)

### General Info
***
This's a OS project for the year 2022 at the Faculty of Computers and Information, Ain Shams

Under the supervision Dr, Ahmed Salah

In this project we created 

1. Dynamic Allocator
    1. initialize mem blocks list
    2. find a block with start virtual address
    3. insert a new block in allocated list by a sorted way
    4. allocate a new block using first fit, best fit and next fit 
    5. insert a new block in free list by a sorted way and merge the list if possible
2. Manipulate page table entries
    1. clear table entry
    2. get & set permissions
    3. convert virtual to physical
3. Manipulate RAM chunks
    1. cut & copy pages in memory and share chunks 
    2. allocate a new chunk
    3. calculate allocated & required space in given range
4. kernel heap 
    1. initalize the kernel heap system
    2. allocate (using FIRST & BEST fit & next fit) and free 
    3. convert physical to virtual
    4. reallocate the size if needed
5. Handle PAGE FAULT using CLOCK page replacement algorithm
6. Implement USER HEAP (using the dynamic allocator) 
7. Implement SHARING to share variables between various processes
8. Implement SEMAPHORES to enable OS to protect processes’ shared variables


### Team Members
***
| Name  | Account |
| ----- | ------- |
| Mina Nabil  | @<a href="https://github.com/minasanta/" target="_blank">minasanta</a> |
| Mehrael Ashraf  | @<a href="https://github.com/Mehrael/" target="_blank">Mehrael</a> |
| Mina Adil  | @<a href="https://github.com/Mina927/" target="_blank">Mina927</a> |
| Filobateer Essam  | @<a href="https://github.com/FilobateerEssam/" target="_blank">FilobateerEssam</a> |
| Martina Sabry  | ___ |
| Marlen Tawadrous  | ___ |


### Honoring
***
Our project was selected as one of the top projects, in ranked ninth

![The Certificate](The_Certificate.jpg)
