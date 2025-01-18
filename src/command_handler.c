#include "command_handler.h"
#include "utils.h"
#include "spreadsheet.h"
#include "stdio.h"
#include "string.h"

#define MAX_CELL_NAME 100
#define MAX_EXPRESSION 1000

void parse_command(spreadsheet* sheet, const char *command){
    char targetcell[MAX_CELL_NAME];
    char expression[MAX_EXPRESSION];
    int error_code = 0;
    validate_command(sheet ,command , targetcell , expression , &error_code);
    printf("%d\n" , error_code);
}

void validate_command(spreadsheet* sheet, const char *command , char *targetcell , char *expression , int *error_code){

    char *equal = strchr(command , '=');
    if(equal == NULL){
        *error_code = 1; // Invalid command
        return;
    }
    strncpy(targetcell , command , equal - command);
    targetcell[equal - command] = '\0';
    strcpy(expression , equal + 1);
    // trim_space(targetcell);
    // trim_space(expression);
    int targetcell_rowid;
    int targetcell_colid;
    if(!valid_cell(sheet , targetcell , &targetcell_rowid , &targetcell_colid)){ // valid cell can also be used to extract row and column indices
        *error_code = 2; // Invalid Cell reference
        return;
    }
    int _err = 0;
    int expr_type = -1;
    if(!valid_expression(sheet , expression , &expr_type , &_err)){
        if(_err == 1){
            *error_code = 4; // Invalid Range
            return;
        }
        *error_code = 3; // Invalid Expression
        return;
    }

}