#include "command_handler.h"
#include "utils.h"
#include "spreadsheet.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"

#define MAX_CELL_NAME 100
#define MAX_EXPRESSION 1000

// void parse_command(spreadsheet* sheet, const char *command){
//     char targetcell[MAX_CELL_NAME];
//     char expression[MAX_EXPRESSION];
//     int error_code = 0;
//     validate_command(sheet ,command , targetcell , expression , &error_code);
//     if(error_code != 0){
//         error_message(error_code);
//         return;
//     }
// }

// void validate_command(spreadsheet* sheet, const char *command , char *targetcell , char *expression , int *error_code){

//     char *equal = strchr(command , '=');
//     if(equal == NULL){
//         *error_code = 1; // Invalid command
//         return;
//     }
//     strncpy(targetcell , command , equal - command);
//     targetcell[equal - command] = '\0';
//     strcpy(expression , equal + 1);
//     // trim_space(targetcell);
//     // trim_space(expression);
//     int targetcell_rowid;
//     int targetcell_colid;
//     if(!valid_cell(sheet , targetcell , &targetcell_rowid , &targetcell_colid)){ // valid cell can also be used to extract row and column indices
//         *error_code = 2; // Invalid Cell reference
//         return;
//     }
//     int _err = 0;
//     int expr_type = -1;
//     if(!valid_expression(sheet , expression , &expr_type , &_err)){
//         if(_err == 1){
//             *error_code = 4; // Invalid Range
//             return;
//         }
//         *error_code = 3; // Invalid Expression
//         return;
//     }
//     if(expr_type != -1){
//         switch (expr_type){
//             case 0: 
//                 number_assign(targetcell_rowid , targetcell_colid , expression);
//                 break;
//             case 1:
//                 value_assign(targetcell_rowid , targetcell_colid , expression);
//                 break;
//             case 2:
//                 operator_assign(targetcell_rowid , targetcell_colid , expression);
//                 break;
//             case 3:
//                 min_handling(targetcell_rowid , targetcell_colid , expression);
//                 break;
//             case 4:
//                 max_handling(targetcell_rowid , targetcell_colid , expression);
//                 break;
//             case 5:
//                 avg_handling(targetcell_rowid , targetcell_colid , expression);
//                 break;
//             case 6:
//                 sum_handling(targetcell_rowid , targetcell_colid , expression);
//                 break;
//             case 7:
//                 stdev_handling(targetcell_rowid , targetcell_colid , expression);
//                 break;
//             case 8:
//                 sleep_handling(targetcell_rowid , targetcell_colid , expression);
//                 break;
//             case 9:
//                 sleep_handling(targetcell_rowid , targetcell_colid , expression);
//                 break;
//             default:
//                 break;
//         }
//     }

// }

spreadsheet *create_spreadsheet(int rows, int cols){
    spreadsheet *s = (spreadsheet *)malloc(sizeof(spreadsheet));
    s->rows = rows;
    s->cols = cols;
    s->table = (cell **)malloc(rows * sizeof(cell *));
    s->bounds=(spreadsheetbounds *)malloc(sizeof(spreadsheetbounds));
    for(int i = 0; i < rows; i++){
        s->table[i] = (cell *)malloc(cols * sizeof(cell));
        for(int j = 0; j < cols; j++){
            s->table[i][j].val = NULL;
            s->table[i][j].dependency = 0;
        }
    }

    s->bounds->first_row=(int *)malloc(sizeof(int));
    s->bounds->first_col=(int *)malloc(sizeof(int));
    s->bounds->last_row=(int *)malloc(sizeof(int));
    s->bounds->last_col=(int *)malloc(sizeof(int));
    *(s->bounds->first_row)=1;
    *(s->bounds->first_col)=1;
    *(s->bounds->last_row)=(rows<10)?rows:10;
    *(s->bounds->last_col)=(cols<10)?cols:10;
    return s;
}

void handle_control_command(char control,spreadsheet *sheet){
    int num_rows=sheet->rows;
    int num_cols=sheet->cols;
    int *firstrow=sheet->bounds->first_row,*firstcol=sheet->bounds->first_col;
    int *lastrow=sheet->bounds->last_row,*lastcol=sheet->bounds->last_col;    
    switch(control){
        case 'w':
            *lastrow=(*firstrow-10>0)?*lastrow-10:*lastrow-*firstrow+1;
            *firstrow=max(1,*firstrow-10);
            break;
        case 'a':
            *lastcol=(*firstcol-10>0)?*lastcol-10:*lastcol-*firstcol+1;
            *firstcol=max(1,*firstcol-10);
            break;
        case 's':
            *firstrow=(*lastrow+10<=num_rows)?*firstrow+10:*firstrow+num_rows-*lastrow;
            *lastrow=min(num_rows,*lastrow+10);
            break;
        case 'd':
            *firstcol=(*lastcol+10<=num_cols)?*firstcol+10:*firstcol+num_cols-*lastcol;
            *lastcol=min(num_cols,*lastcol+10);
            break;
        case 'q':
            printf("Quitting the program\n");
            exit(0);
        default:
            printf("Invalid command! Use w/a/s/d to scroll or q to quit.\n");
    }
}

// int main(){
//     int rows=12;
//     int cols=12;
//     spreadsheet *sheet = create_spreadsheet(rows, cols);
//     printf("%d\n",sheet->rows);
//     printf("%d\n",sheet->cols);
//     printf("%d\n",*(sheet->bounds->first_row));
//     printf("%d\n",*(sheet->bounds->first_col));
//     printf("%d\n",*(sheet->bounds->last_row));
//     printf("%d\n",*(sheet->bounds->last_col));
//     handle_control_command('d',sheet);
//     handle_control_command('d',sheet);
//     handle_control_command('s',sheet);
//     handle_control_command('s',sheet);
//     handle_control_command('w',sheet);
//     handle_control_command('a',sheet);
//     handle_control_command('a',sheet);
//     printf("%d\n",*(sheet->bounds->first_row));
//     printf("%d\n",*(sheet->bounds->first_col));
//     printf("%d\n",*(sheet->bounds->last_row));
//     printf("%d\n",*(sheet->bounds->last_col));
// }