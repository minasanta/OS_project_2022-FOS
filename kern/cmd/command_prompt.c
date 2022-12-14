/*	Simple command-line kernel prompt useful for
	controlling the kernel and exploring the system interactively.


KEY WORDS
==========
CONSTANTS:	WHITESPACE, NUM_OF_COMMANDS
VARIABLES:	Command, commands, name, description, function_to_execute, number_of_arguments, arguments, command_string, command_line, command_found
FUNCTIONS:	readline, cprintf, execute_command, run_command_prompt, command_kernel_info, command_help, strcmp, strsplit, start_of_kernel, start_of_uninitialized_data_section, end_of_kernel_code_section, end_of_kernel
=====================================================================================================================================================================================================
 */


#include <kern/cmd/command_prompt.h>

#include <kern/proc/user_environment.h>
#include <kern/trap/kdebug.h>
#include <kern/cons/console.h>
#include "commands.h"


int auto_complete(char *command_string) {
	struct Command foundCommands[NUM_OF_COMMANDS];
	int numerOfFoundCommands = 0;
	for (int i = 0; i < NUM_OF_COMMANDS; i++)
	{
		int found = 1;
		if(strlen(command_string) < strlen(commands[i].name)) {
			for (int j = 0; j < strlen(command_string); j++) {
				if(command_string[j] != commands[i].name[j]) {
					found = 0;
					break;
				}
			}
		}
		else {
			found = 0;
		}
		if(found == 1) {
			foundCommands[numerOfFoundCommands].name = commands[i].name;
			foundCommands[numerOfFoundCommands].description = commands[i].description;
			foundCommands[numerOfFoundCommands].function_to_execute = commands[i].function_to_execute;
			numerOfFoundCommands++;
		}
	}
	if(numerOfFoundCommands > 0) {
		for (int i = 0; i < numerOfFoundCommands; i++)
			cprintf("%s\n", foundCommands[i].name);
		return 0;
	}
	cprintf("Unknown command %s\n", command_string);
	return 0;
}
//invoke the command prompt
void run_command_prompt()
{
	char command_line[1024];

	while (1==1)
	{
		//get command line
		readline("FOS> ", command_line);

		//parse and execute the command
		if (command_line != NULL)
			if (execute_command(command_line) < 0)
				break;
	}
}

/***** Kernel command prompt command interpreter *****/

//define the white-space symbols
#define WHITESPACE "\t\r\n "

//Function to parse any command and execute it
//(simply by calling its corresponding function)
int execute_command(char *command_string)
{
	// Split the command string into whitespace-separated arguments
	int number_of_arguments;
	//allocate array of char * of size MAX_ARGUMENTS = 16 found in string.h
	char *arguments[MAX_ARGUMENTS];


	strsplit(command_string, WHITESPACE, arguments, &number_of_arguments) ;
	if (number_of_arguments == 0)
		return 0;

	// Lookup in the commands array and execute the command
	int command_found = 0;
	int i ;
	for (i = 0; i < NUM_OF_COMMANDS; i++)
	{
		if (strcmp(arguments[0], commands[i].name) == 0)
		{
			command_found = 1;
			break;
		}
	}

	if(command_found)
	{
		int return_value;
		return_value = commands[i].function_to_execute(number_of_arguments, arguments);
		return return_value;
	}
	else
	{
		auto_complete(arguments[0]);
		//if not found, then it's unknown command
		//cprintf("Unknown command '%s'\n", arguments[0]);
		return 0;
	}
}
