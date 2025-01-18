#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

#include "spreadsheet.h"

void parse_command(spreadsheet *sheet, const char *command); // Parse the command 
void execute_command(); // Execute the command
void validate_command(); // Validate the command
void error_message(int error_code); // Print error message
// Still more functions to be added

#endif