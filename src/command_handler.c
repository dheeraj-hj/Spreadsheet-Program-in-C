#include "command_handler.h"
#include "utils.h"
#include "spreadsheet.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "stdlib.h"

#define MAX_CELL_NAME 7 // ZZZ999 // have to make it dynamic size
#define MAX_EXPRESSION 21 // SLEEP(AAA111:ZZZ999)
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

void parse_command(spreadsheet* sheet, const char *command){
    char targetcell[MAX_CELL_NAME];
    char expression[MAX_EXPRESSION];
    int error_code = 0;
    validate_command(sheet ,command , targetcell , expression , &error_code);
    // if(error_code != 0){
    //     error_message(error_code);
    //     return;
    // }
}

void validate_command(spreadsheet* sheet, const char *command , char *targetcell , char *expression , int *error_code){
    /*
        Validates the given commands and assigns work for respective command handling functions
        ERROR Meaning :
        1 : INVALID command / Command not recognized
        2 : INVALID reference to Cell
        3 : INVALID expression
        4 : INVALID range
    */

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
    printf("Expression Type : %d\n", expr_type);
    // if(expr_type != -1){
    //     switch (expr_type){
    //         case 0: 
    //             number_assign(sheet , &targetcell_rowid , &targetcell_colid , expression);
    //             break;
    //         case 1:
    //             value_assign(sheet , &targetcell_rowid , &targetcell_colid , expression);
    //             break;
    //         case 2:
    //             operator_assign(sheet , &targetcell_rowid , &targetcell_colid , expression);
    //             break;
    //         case 3:
    //             min_handling(sheet , &targetcell_rowid , &targetcell_colid , expression);
    //             break;
    //         case 4:
    //             max_handling(sheet , &targetcell_rowid , &targetcell_colid , expression);
    //             break;
    //         case 5:
    //             avg_handling(sheet , &targetcell_rowid , &targetcell_colid , expression);
    //             break;
    //         case 6:
    //             sum_handling(sheet , &targetcell_rowid , &targetcell_colid , expression);
    //             break;
    //         case 7:
    //             stdev_handling(sheet , &targetcell_rowid , &targetcell_colid , expression);
    //             break;
    //         case 8:
    //             sleep_handling(sheet , &targetcell_rowid , &targetcell_colid , expression);
    //             break;
    //         case 9:
    //             sleep_handling(sheet , &targetcell_rowid , &targetcell_colid , expression);
    //             break;
    //         default:
    //             break;
    //     }
    // }

}


// void number_assign(spreadsheet* sheet, int *row , int *col, char *expr){
//     sheet->table[row][col].val = atoi(expr);
//     sheet->table[row][col].dependency = '0';
//     sheet->table[row][col].expr = NULL;
// }

// void value_assign(spreadsheet* sheet, int *row , int *col, char *expr){

//     sheet->table[row][col].val = evaluate_cell(sheet , row , col);
//     sheet->table[row][col].dependency = '1';
//     sheet->table[row][col].expr = expr;
// }


void handle_control_command(char control,spreadsheet *sheet){
    int num_rows=sheet->rows;
    int num_cols=sheet->cols;
    int *firstrow=sheet->bounds->first_row,*firstcol=sheet->bounds->first_col;
    int *lastrow=sheet->bounds->last_row,*lastcol=sheet->bounds->last_col;    
    switch(control){
        case 'w':
            *lastrow=(*firstrow-10>0)?*lastrow-10:*lastrow-*firstrow+1;
            *firstrow=MAX(1,*firstrow-10);
            break;
        case 'a':
            *lastcol=(*firstcol-10>0)?*lastcol-10:*lastcol-*firstcol+1;
            *firstcol=MAX(1,*firstcol-10);
            break;
        case 's':
            *firstrow=(*lastrow+10<=num_rows)?*firstrow+10:*firstrow+num_rows-*lastrow;
            *lastrow=MIN(num_rows,*lastrow+10);
            break;
        case 'd':
            *firstcol=(*lastcol+10<=num_cols)?*firstcol+10:*firstcol+num_cols-*lastcol;
            *lastcol=MIN(num_cols,*lastcol+10);
            break;
        case 'q':
            printf("Quitting the program\n");
            exit(0);
        default:
            printf("Invalid command! Use w/a/s/d to scroll or q to quit.\n");
    }
}
