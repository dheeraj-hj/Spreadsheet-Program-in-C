#include "command_handler.h"
#include "utils.h"
#include "spreadsheet.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "stdlib.h"
#include "limits.h"
#include "math.h" // Use -lm while compiling

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

void validate_command(spreadsheet* sheet, const char *cmd , char *targetcell , char *expression , int *error_code){
    /*
        Validates the given commands and assigns work for respective command handling functions
        ERROR Meaning :
        1 : INVALID command / Command not recognized
        2 : INVALID reference to Cell
        3 : INVALID expression
        4 : INVALID range
    */
    char *command = strdup(cmd);
    char *equal = strchr(command , '=');
    if(equal == NULL){
        *error_code = 1; // Invalid command
        free(command);
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
        free(command);
        return;
    }
    int _err = 0;
    int expr_type = -1;
    if(!valid_expression(sheet , expression , &expr_type , &_err)){
        if(_err == 1){
            *error_code = 4; // Invalid Range
            free(command);
            return;
        }
        *error_code = 3; // Invalid Expression
        free(command);
        return;
    }
    // printf("Expression Type : %d\n", expr_type);
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
    free(command);

}


void number_assign(spreadsheet* sheet, int *row , int *col, const char *_expr){
    sheet->table[*row][*col].val = atoi(_expr);
    sheet->table[*row][*col].dependency = '0';
    sheet->table[*row][*col].formula = NULL;
}

void value_assign(spreadsheet* sheet, int *row , int *col, const char *_expr){
    char *expr = strdup(_expr);
    int dependent_row;
    int dependent_col;
    valid_cell(sheet , expr , &dependent_row , &dependent_col);
    if(*row == dependent_row && *col == dependent_col){
        // error_message(6); // Self reference
        free(expr);
        return;
    }
    int dependent_cell_hash = hash_index(sheet , dependent_row , dependent_col);
    int current_cell_hash = hash_index(sheet , *(row) , *(col));
    if(check_cycle(sheet , &sheet->table[*row][*col] , &dependent_cell_hash)){
        // error_message(7); // Cycle Formation
        free(expr);
        return;
    }
    delete_parent_connections(sheet , &sheet->table[*row][*col]);
    add_child(&sheet->table[dependent_row][dependent_col] , current_cell_hash);
    add_parent(&sheet->table[*row][*col] , dependent_cell_hash);
    sheet->table[*row][*col].val = sheet->table[dependent_row][dependent_col].val;
    sheet->table[*row][*col].dependency = '1';
    sheet->table[*row][*col].formula = expr;
    free(expr);
}

void operator_assign(spreadsheet* sheet, int *row , int *col, const char *_expr){
    char *exprdup = strdup(_expr);
    char *operators = "+-*/";
    char *op = strpbrk(exprdup , operators);
    char *left = exprdup;
    char *right = op + 1;
    char operation = *op;
    for(int i = 0; i < strlen(left); i++){
        if(left[i] == '+' || left[i] == '-' || left[i] == '*' || left[i] == '/'){
            left[i] = '\0';
        }
    }
    int left_val;
    int right_val;
    int left_row=-1;
    int left_col;
    int right_row=-1;
    int right_col;
    int left_cell_hash;
    int right_cell_hash;
    int current_cell_hash = hash_index(sheet , *(row) , *(col));
    if(is_number(left)){
        left_val = atoi(left);
    }else{
        valid_cell(sheet , left , &left_row , &left_col);
        if(*row == left_row && *col == left_col){
            // error_message(6); // Self reference
            free(exprdup);
            return;
        }
        left_cell_hash = hash_index(sheet , left_row , left_col);
        if(check_cycle(sheet , &sheet->table[*row][*col] , &left_cell_hash)){
            // error_message(7); // Cycle Formation
            free(exprdup);
            return;
        } 
    }
    if(is_number(right)){
        right_val = atoi(right);
    }else{
        valid_cell(sheet , right , &right_row , &right_col);
        if(*row == right_row && *col == right_col){
            // error_message(6); // Self reference
            free(exprdup);
            return;
        }
        right_cell_hash = hash_index(sheet , right_row , right_col);
        if(check_cycle(sheet , &sheet->table[*row][*col] , &right_cell_hash)){
            // error_message(7); // Cycle Formation
            free(exprdup);
            return;
        }
    }
    delete_parent_connections(sheet , &sheet->table[*row][*col]);

    if(left_row != -1){
        left_val = sheet->table[left_row][left_col].val;
        add_child(&sheet->table[left_row][left_col] , current_cell_hash);
        add_parent(&sheet->table[*row][*col] , left_cell_hash);
    }
    if(right_row != -1){
        right_val = sheet->table[right_row][right_col].val;
        add_child(&sheet->table[right_row][right_col] , current_cell_hash);
        add_parent(&sheet->table[*row][*col] , right_cell_hash);
    }
    // printf("left val %d right val %d\n" , left_val , right_val);
    // printf("Operation %c\n" , operation);
    switch (operation){
        case '+':
            sheet->table[*row][*col].val = left_val + right_val;
            break;
        case '-':
            sheet->table[*row][*col].val = left_val - right_val;
            break;
        case '*':
            sheet->table[*row][*col].val = left_val * right_val;
            break;
        case '/':
            // if(right_val == 0){
            //     error_message(5); // Division by zero
            //     return;
            // }
            sheet->table[*row][*col].val = left_val / right_val;
            break;
        default:
            break;
    }
    sheet->table[*row][*col].dependency = '0';
    sheet->table[*row][*col].formula = exprdup;
    free(exprdup);
}

void min_handling(spreadsheet* sheet , int *row , int *col ,const char *_expr){
    // Duplicate expr to work on a modifiable copy
    char *exprdup = strdup(_expr);

    // Find function start and range end
    char *fun_end = strchr(exprdup, '(');
    char *range_end = strchr(exprdup, ')');
    char *range = fun_end + 1;

    // Null-terminate at specific locations
    *fun_end = '\0';
    *range_end = '\0';

    // Find the colon separator
    char *colon = strchr(range, ':');
    *colon = '\0';
    char *first_cell = range;
    char *last_cell = colon + 1;

    // Convert cell names to indices
    int row1, col1, row2, col2;
    name_to_indices(first_cell, &row1, &col1);
    name_to_indices(last_cell, &row2, &col2);

    // Validate cells and check for cycles or self-references
    for (int i = row1; i <= row2; i++) {
        for (int j = col1; j <= col2; j++) {
            if (i == *row && j == *col) {
               // error_message(6); // Self reference
               free(exprdup);
                return;
            }

            int dependent_cell_hash = hash_index(sheet, i, j);
            if (check_cycle(sheet, &sheet->table[*row][*col], &dependent_cell_hash)) {
                // error_message(7); // Cycle Formation
                free(exprdup);
                return;
            }
        }
    }
    delete_parent_connections(sheet, &sheet->table[*row][*col]);
    // printf("parent size : %d \n" , sheet->table[*row][*col].num_parents);
    int min_val = INT_MAX;
    for (int i = row1; i <= row2; i++) {
        for (int j = col1; j <= col2; j++) {
            min_val = MIN(min_val, sheet->table[i][j].val);
            add_child(&sheet->table[i][j], hash_index(sheet, *row, *col));
            add_parent(&sheet->table[*row][*col], hash_index(sheet, i, j));
            // printf("index : %d , %d ,hash : %d\n" , i , j , hash_index(sheet , i , j));
            // for(int i = 0 ;i < sheet->table[*row][*col].num_parents ; i++){
            //     printf("parent : %d " , sheet->table[*row][*col].parents[i]);
            // }
            // printf("\n");
        }
    }
    sheet->table[*row][*col].val = min_val;
    sheet->table[*row][*col].formula = exprdup;

    free(exprdup);
}

void max_handling(spreadsheet* sheet , int *row, int *col , const char *_expr){
    char *exprdup = strndup(_expr, strlen(_expr));
    char *fun_end = strchr(exprdup, '(');
    char *range_end = strchr(exprdup, ')');
    char *range = fun_end + 1;
    *fun_end = '\0';
    *range_end = '\0';
    char *colon = strchr(range, ':');
    *colon = '\0';
    char *first_cell = range;
    char *last_cell = colon + 1;
    int row1, col1, row2, col2;
    name_to_indices(first_cell, &row1, &col1);
    name_to_indices(last_cell, &row2, &col2);
    for (int i = row1; i <= row2; i++) {
        for (int j = col1; j <= col2; j++) {
            if (i == *row && j == *col) {
                // error_message(6); // Self reference
                free(exprdup);
                return;
            }
            int dependent_cell_hash = hash_index(sheet, i, j);
            if (check_cycle(sheet, &sheet->table[*row][*col], &dependent_cell_hash)) {
                // error_message(7); // Cycle Formation
                free(exprdup);
                return;
            }
        }
    }
    delete_parent_connections(sheet, &sheet->table[*row][*col]);
    int max_val = INT_MIN;
    for (int i = row1; i <= row2; i++) {
        for (int j = col1; j <= col2; j++) {
            max_val = MAX(max_val, sheet->table[i][j].val);
            add_child(&sheet->table[i][j], hash_index(sheet, *row, *col));
            add_parent(&sheet->table[*row][*col], hash_index(sheet, i, j));
        }
    }
    sheet->table[*row][*col].val = max_val;
    sheet->table[*row][*col].formula = exprdup;
    free(exprdup);
}

void sum_handling(spreadsheet* sheet , int *row , int *col , const char *_expr){
    char *exprdup = strndup(_expr, strlen(_expr));
    char *fun_end = strchr(exprdup, '(');
    char *range_end = strchr(exprdup, ')');
    char *range = fun_end + 1;
    *fun_end = '\0';
    *range_end = '\0';
    char *colon = strchr(range, ':');
    *colon = '\0';
    char *first_cell = range;
    char *last_cell = colon + 1;
    int row1, col1, row2, col2;
    name_to_indices(first_cell, &row1, &col1);
    name_to_indices(last_cell, &row2, &col2);
    for (int i = row1; i <= row2; i++) {
        for (int j = col1; j <= col2; j++) {
            if (i == *row && j == *col) {
                // error_message(6); // Self reference
                free(exprdup);
                return;
            }
            int dependent_cell_hash = hash_index(sheet, i, j);
            if (check_cycle(sheet, &sheet->table[*row][*col], &dependent_cell_hash)) {
                // error_message(7); // Cycle Formation
                free(exprdup);
                return;
            }
        }
    }
    delete_parent_connections(sheet, &sheet->table[*row][*col]);
    int sum = 0;
    for (int i = row1; i <= row2; i++) {
        for (int j = col1; j <= col2; j++) {
            sum = sum + sheet->table[i][j].val;
            add_child(&sheet->table[i][j], hash_index(sheet, *row, *col));
            add_parent(&sheet->table[*row][*col], hash_index(sheet, i, j));
        }
    }
    sheet->table[*row][*col].val = sum;
    sheet->table[*row][*col].formula = exprdup;
    free(exprdup);
}

void avg_handling(spreadsheet* sheet , int *row, int *col , const char *_expr){
    char *exprdup = strndup(_expr, strlen(_expr));
    char *fun_end = strchr(exprdup, '(');
    char *range_end = strchr(exprdup, ')');
    char *range = fun_end + 1;
    *fun_end = '\0';
    *range_end = '\0';
    char *colon = strchr(range, ':');
    *colon = '\0';
    char *first_cell = range;
    char *last_cell = colon + 1;
    int row1, col1, row2, col2;
    name_to_indices(first_cell, &row1, &col1);
    name_to_indices(last_cell, &row2, &col2);
    for (int i = row1; i <= row2; i++) {
        for (int j = col1; j <= col2; j++) {
            if (i == *row && j == *col) {
                // error_message(6); // Self reference
                free(exprdup);
                return;
            }
            int dependent_cell_hash = hash_index(sheet, i, j);
            if (check_cycle(sheet, &sheet->table[*row][*col], &dependent_cell_hash)) {
                // error_message(7); // Cycle Formation
                free(exprdup);
                return;
            }
        }
    }
    delete_parent_connections(sheet, &sheet->table[*row][*col]);
    float sum = 0;
    float cnt = 0;
    for (int i = row1; i <= row2; i++) {
        for (int j = col1; j <= col2; j++) {
            sum = sum + sheet->table[i][j].val;
            cnt++;
            add_child(&sheet->table[i][j], hash_index(sheet, *row, *col));
            add_parent(&sheet->table[*row][*col], hash_index(sheet, i, j));
        }
    }
    float favg = sum/cnt;
    int avg = round(favg);
    sheet->table[*row][*col].val = avg;
    sheet->table[*row][*col].formula = exprdup;
    free(exprdup);
}

void stdev_handling(spreadsheet *sheet, int *row , int *col , const char *_expr){
    char *exprdup = strndup(_expr, strlen(_expr));
    char *fun_end = strchr(exprdup, '(');
    char *range_end = strchr(exprdup, ')');
    char *range = fun_end + 1;
    *fun_end = '\0';
    *range_end = '\0';
    char *colon = strchr(range, ':');
    *colon = '\0';
    char *first_cell = range;
    char *last_cell = colon + 1;
    int row1, col1, row2, col2;
    name_to_indices(first_cell, &row1, &col1);
    name_to_indices(last_cell, &row2, &col2);
    for (int i = row1; i <= row2; i++) {
        for (int j = col1; j <= col2; j++) {
            if (i == *row && j == *col) {
                // error_message(6); // Self reference
                free(exprdup);
                return;
            }
            int dependent_cell_hash = hash_index(sheet, i, j);
            if (check_cycle(sheet, &sheet->table[*row][*col], &dependent_cell_hash)) {
                // error_message(7); // Cycle Formation
                free(exprdup);
                return;
            }
        }
    }
    delete_parent_connections(sheet, &sheet->table[*row][*col]);
    float sum = 0;
    float cnt = 0;
    for (int i = row1; i <= row2; i++) {
        for (int j = col1; j <= col2; j++) {
            sum = sum + sheet->table[i][j].val;
            cnt++;
            add_child(&sheet->table[i][j], hash_index(sheet, *row, *col));
            add_parent(&sheet->table[*row][*col], hash_index(sheet, i, j));
        }
    }
    float mean = sum/cnt;
    float values = 0.0;
    for (int i = row1; i <= row2; i++) {
        for (int j = col1; j <= col2; j++) {
            values += pow(sheet->table[i][j].val - mean , 2);
        }
    }
    float stddev = sqrt(values/cnt);
    int sd = round(stddev);
    sheet->table[*row][*col].val = sd;
    sheet->table[*row][*col].formula = exprdup;
    free(exprdup);

}

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
            *firstrow=(*lastrow+10<=num_rows)?*firstrow+10:*firstrow+num_rows-*lastrow;
            *lastrow=MIN(num_rows,*lastrow+10);
            break;
        case 'd':
            *firstcol=(*lastcol+10<=num_cols)?*firstcol+10:*firstcol+num_cols-*lastcol;
            *lastcol=MIN(num_cols,*lastcol+10);
            *firstcol=(*lastcol+10<=num_cols)?*firstcol+10:*firstcol+num_cols-*lastcol;
            *lastcol=MIN(num_cols,*lastcol+10);
            break;
        case 'q':
            printf("Quitting the program\n");
            exit(0);
        default:
            printf("Invalid command! Use w/a/s/d to scroll or q to quit.\n");
        
        // Display function to be called after this
    }
}

void scroll_to(spreadsheet *sheet, char *cell){
    // The check for output suppressed to be done before calling this function
    int num_rows=sheet->rows;
    int num_cols=sheet->cols;
    int *firstrow=sheet->bounds->first_row,*firstcol=sheet->bounds->first_col;
    int *lastrow=sheet->bounds->last_row,*lastcol=sheet->bounds->last_col;  

    int row,col;
    name_to_indices(cell,&row,&col); // Convert the cell name to row and column indices

    row+=1,col+=1;
    if(row>num_rows || col>num_cols){
        printf("Invalid cell name\n");
        return;
    }

    // This is the top left cell
    *firstrow=row; 
    *firstcol=col; 

    // This is the bottom right cell
    *lastrow=MIN(num_rows,*firstrow+9); 
    *lastcol=MIN(num_cols,*firstcol+9); 

    return;

    // Display function to be called after this
}
