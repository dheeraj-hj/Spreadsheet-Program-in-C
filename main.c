#include <stdio.h>
#include <stdlib.h>
#include "src/spreadsheet.h"
#include "src/spreadsheet_display.h"
#include "src/command_handler.h"
#include "src/utils.h"
#include "string.h"

#define MAX_CMD_LENGTH 256

int main(int argc, char *argv[]){
    if(argc != 3){
        printf("Usage : %s <rows> <cols>\n" , argv[0]);
        return 1;
    }
    int rows = atoi(argv[1]);
    int cols = atoi(argv[2]);
     if (rows <= 0 || cols <= 0) {
        fprintf(stderr, "Error: Invalid spreadsheet size.\n");
        return EXIT_FAILURE;
    }
    spreadsheet *sheet = create_spreadsheet(rows, cols);
    char command[MAX_CMD_LENGTH];
    while(1){
        if (fgets(command, MAX_CMD_LENGTH, stdin) == NULL) {
            continue;
        }
        if (command[strlen(command) - 1] != '\n') {
            display_status("Invalid Command", 0);
        }
        command[strcspn(command, "\n")] = '\0';
        if (strcmp(command, "q") == 0) {
            break;
        }
        parse_command(sheet, command);
    }
    return 0;
}
