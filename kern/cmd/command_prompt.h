#ifndef FOS_KERN_MONITOR_H
#define FOS_KERN_MONITOR_H
#ifndef FOS_KERNEL
# error "This is a FOS kernel header; user programs should not #include it"
#endif
#include <inc/stdio.h>
#include <inc/string.h>
#include <inc/memlayout.h>
#include <inc/assert.h>
#include <inc/x86.h>
#include <inc/disk.h>
#include <inc/error.h>

// Function to activate the kernel command prompt
int execute_command(char *command_string);
void run_command_prompt();
int auto_complete(char *command_string);

#endif	// !FOS_KERN_MONITOR_H
