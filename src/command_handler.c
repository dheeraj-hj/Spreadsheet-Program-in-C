#include "command_handler.h"
#include "utils.h"
#include "spreadsheet.h"
#include "spreadsheet_display.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "stdlib.h"
#include "limits.h"
#include "math.h" // Use -lm while compiling
#include "unistd.h"
#include "time.h"
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

int display = 2;

void parse_command(spreadsheet* sheet, const char *command){
    /*
        This function takes input command from main function
        validate the command and displays sheet, status
        Input:
        - sheet : pointer to the spreadsheet
        - command : Input string as a command given by the user 
    */
   struct timespec start_time, end_time;
    char *targetcell;
    char *expression;
    int error_code = 0;
    clock_gettime(CLOCK_MONOTONIC, &start_time);
    validate_command(sheet ,command , &error_code);
    clock_gettime(CLOCK_MONOTONIC, &end_time); 

    double t = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_nsec - start_time.tv_nsec) / 1.0e9;    
    if(error_code != 0){
        if(display == 2){
            display_spreadsheet(sheet);
        }
        error_message(error_code , t);
        return;
    }else{
        if(display == 2){
            display_spreadsheet(sheet);
        }
        display_status("ok" , t);
    }
    if(display == 1){
        display = 2;
    }
}

void  error_message(int error_code , int time){
    /*
        This function prints the error status according to error code
        Input:
        - error_code : integer error_code value
        - time : time taken to run the program
    */
    char *error_messages[] = {
        "Command not recognized",
        "INVALID reference to Cell",
        "INVALID expression",
        "INVALID range",
        "Division by zero",
        "Self reference",
        "Cycle Formation"
    };
    display_status(error_messages[error_code-1] , time);
}

void validate_command(spreadsheet* sheet, const char *cmd , int *error_code){
    /*
        Validates the given commands and assigns work for respective command handling functions
        ERROR Meaning :
        1 : INVALID command / Command not recognized
        2 : INVALID reference to Cell
        3 : INVALID expression
        4 : INVALID range
        5 : Division by zero
        6 : Self reference
        7 : Cycle Formation
        Input:
        - sheet - pointer to the spreadsheet
        - cmd - string command to validate
        - error_code - pointer to store the error code
    */
    char *command = strdup(cmd);
    if(strlen(command) == 1){
        if(command[0] == 'w' || command[0] == 's' || command[0] == 'a' || command[0] == 'd'){
            handle_control_command(command[0] , sheet);
            return;
        }
    }
    if(strncmp(command , "scroll_to " , 10) == 0){
        scroll_to(sheet , command + 10 , error_code);
        free(command);
        return;
    }
    if(strncmp(command , "disable_output" , 14) == 0){
        display = 0;
        free(command);
        return;
    }
    if(strncmp(command , "enable_output" , 13) == 0){
        display = 1;
        free(command);
        return;
    }
    char *equal = strchr(command , '=');
    if(equal == NULL){
        *error_code = 1;
        free(command);
        return;
    }
    const char *targetcell = command;
    const char *expression = equal + 1;
    *equal = '\0';
    // trim_space(targetcell);
    // trim_space(expression);
    int targetcell_rowid;
    int targetcell_colid;
    if(!valid_cell(sheet , targetcell , &targetcell_rowid , &targetcell_colid)){ 
        *error_code = 2; 
        free(command);
        return;
    }
    int _err = 0;
    int expr_type = -1;
    if(!valid_expression(sheet , expression , &expr_type , &_err)){
        if(_err == 1){
            *error_code = 4;
            free(command);
            return;
        }
        *error_code = 3;
        free(command);
        return;
    }
    if(expr_type != -1){
        switch (expr_type){
            case 0: 
                number_assign(sheet , &targetcell_rowid , &targetcell_colid , expression);
                break;
            case 1:
                value_assign(sheet , &targetcell_rowid , &targetcell_colid , expression , error_code);
                break;
            case 2:
                operator_assign(sheet , &targetcell_rowid , &targetcell_colid , expression, error_code);
                break;
            case 3:
                min_handling(sheet , &targetcell_rowid , &targetcell_colid , expression, error_code);
                break;
            case 4:
                max_handling(sheet , &targetcell_rowid , &targetcell_colid , expression, error_code);
                break;
            case 5:
                avg_handling(sheet , &targetcell_rowid , &targetcell_colid , expression, error_code);
                break;
            case 6:
                sum_handling(sheet , &targetcell_rowid , &targetcell_colid , expression, error_code);
                break;
            case 7:
                stdev_handling(sheet , &targetcell_rowid , &targetcell_colid , expression, error_code);
                break;
            case 8:
                sleep_handling(sheet , &targetcell_rowid , &targetcell_colid , expression, error_code);
                break;
            case 9:
                sleep_handling(sheet , &targetcell_rowid , &targetcell_colid , expression, error_code);
                break;
            default:
                break;
        }
    }
    free(command);

}

void number_assign(spreadsheet* sheet, int *row , int *col, const char *_expr){
    /*
        This function assigns a number to the cell
        Input:
        - sheet : pointer to the spreadsheet
        - row : pointer to the row index
        - col : pointer to the col index
        - _expr : string which contains number to assign
    */
    sheet->table[*row][*col].val = atoi(_expr);
    sheet->table[*row][*col].vis = 0;
    sheet->table[*row][*col].formula = NULL;
    sheet->table[*row][*col].error = '0';
    recalculate_dependent_cells(sheet , row , col);
}

void value_assign(spreadsheet* sheet, int *row , int *col, const char *_expr , int *error_code){
    /*
        This function assigns a value of another cell to this cell and adjusts dependencies
        error_code is set to 6 if self reference is found, 7 if cycle is found
        Input:
        - sheet : pointer to the spreadsheet
        - row : pointer to the row index
        - col : pointer to the col index
        - _expr : string which contains the cell reference
        - error_code : pointer to store the error code 
    */
    char *expr = strdup(_expr);
    char *expr_to_store = strdup(_expr);
    int dependent_row;
    int dependent_col;
    name_to_indices(expr , &dependent_row , &dependent_col);
    if(*row == dependent_row && *col == dependent_col){
        *error_code = 6;
        free(expr);
        return;
    }
    int dependent_cell_hash = hash_index(sheet , dependent_row , dependent_col);
    int current_cell_hash = hash_index(sheet , *(row) , *(col));
    if(check_cycle(sheet , &sheet->table[*row][*col] , &dependent_cell_hash)){
        *error_code = 7; 
        free(expr);
        return;
    }
    delete_parent_connections(sheet , &sheet->table[*row][*col] , row , col);
    add_child(&sheet->table[dependent_row][dependent_col] , current_cell_hash);
    add_parent(&sheet->table[*row][*col] , dependent_cell_hash);
    if(sheet->table[dependent_row][dependent_col].error == '1'){
        sheet->table[*row][*col].error = '1';
    }else{
        sheet->table[*row][*col].error = '0';
    }
    sheet->table[*row][*col].val = sheet->table[dependent_row][dependent_col].val;
    sheet->table[*row][*col].vis = 0;
    sheet->table[*row][*col].formula = expr_to_store;
    recalculate_dependent_cells(sheet , row , col);
    free(expr);
}

void operator_assign(spreadsheet* sheet, int *row , int *col, const char *_expr , int *error_code){
    /*
        This function assigns the result of an expression to the cell
        error_code is set to 6 if self reference is found, 7 if cycle is found
        Input:
        - sheet : pointer to the spreadsheet
        - row : pointer to the row index
        - col : pointer to the col index
        - _expr : string which contains the expression
        - error_code : pointer to store the error code
    */
    char *exprdup = strdup(_expr);
    char *expr_to_store = strdup(_expr);
    char *operators = "+-*/";
    char *op = strpbrk(exprdup+1 , operators);
    char *left = exprdup;
    char *right = op + 1;
    char operation = *op;
    int i = 1;
    *op = '\0';  
    int left_val;
    int left_mul = 1;
    int right_val;
    int right_mul = 1;
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
        if(left[0] == '-'){
            left_mul = -1;
            left++;
        }
        if(left[0] == '+'){
            left++;
        }
        name_to_indices(left , &left_row , &left_col);
        if(*row == left_row && *col == left_col){
           *error_code = 6;  // Self reference
            free(exprdup);
            return;
        }
        left_cell_hash = hash_index(sheet , left_row , left_col);
        if(check_cycle(sheet , &sheet->table[*row][*col] , &left_cell_hash)){
           *error_code = 7; // Cycle Formation
            free(exprdup);
            return;
        } 
    }
    if(is_number(right)){
        right_val = atoi(right);
    }else{
        if(right[0] == '-'){
            right_mul = -1;
            right++;
        }
        if(right[0] == '+'){
            right++;
        }
        name_to_indices(right , &right_row , &right_col);
        if(*row == right_row && *col == right_col){
            *error_code = 6; // Self reference
            free(exprdup);
            return;
        }
        right_cell_hash = hash_index(sheet , right_row , right_col);
        if(check_cycle(sheet , &sheet->table[*row][*col] , &right_cell_hash)){
            *error_code = 7; // Cycle Formation
            free(exprdup);
            return;
        }
    }
    delete_parent_connections(sheet , &sheet->table[*row][*col] , row , col);

    if(left_row != -1){
        if(sheet->table[left_row][left_col].error == '1'){
            sheet->table[*row][*col].error = '1';
        }else{
            sheet->table[*row][*col].error = '0';
        }
        left_val = sheet->table[left_row][left_col].val * left_mul;
        add_child(&sheet->table[left_row][left_col] , current_cell_hash);
        add_parent(&sheet->table[*row][*col] , left_cell_hash);
    }
    if(right_row != -1){
        if(sheet->table[right_row][right_col].error == '1'){
            sheet->table[*row][*col].error = '1';
        }else{
            sheet->table[*row][*col].error = '0';
        }
        right_val = sheet->table[right_row][right_col].val * right_mul;
        add_child(&sheet->table[right_row][right_col] , current_cell_hash);
        add_parent(&sheet->table[*row][*col] , right_cell_hash);
    }
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
            if(right_val == 0){
                sheet->table[*row][*col].error = '1';
            }else{
                sheet->table[*row][*col].val = left_val / right_val;
            }
            break;
        default:
            break;
    }
    sheet->table[*row][*col].vis = 0;
    sheet->table[*row][*col].formula = expr_to_store;
    recalculate_dependent_cells(sheet , row , col);
    free(exprdup);
}

void min_handling(spreadsheet* sheet , int *row , int *col ,const char *_expr , int *error_code){
    /*
        This function will assign the minimum value of the range to the cell
        error_code is set to 6 if self reference is found, 7 if cycle is found
        Input:
        - sheet : pointer to the spreadsheet
        - row : pointer to the row index
        - col : pointer to the col index
        - _expr : string which contains the formula of format MIN(_:_)
        - error_code : pointer to store error code
    */

    char *exprdup = strdup(_expr);
    char *expr_to_store = strdup(_expr);
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
    struct timespec start_time, end_time;
    // clock_gettime(CLOCK_MONOTONIC, &start_time); // Start time
    for (int i = row1; i <= row2; i++) {
        for (int j = col1; j <= col2; j++) {
            if (i == *row && j == *col) {
               *error_code = 6;  // Self reference
               free(exprdup);
                return;
            }

            int dependent_cell_hash = hash_index(sheet, i, j);
            if (check_cycle(sheet, &sheet->table[*row][*col], &dependent_cell_hash)) {
                *error_code = 7;  // Cycle Formation
                free(exprdup);
                return;
            }
        }
    }
    // clock_gettime(CLOCK_MONOTONIC, &end_time);   // End time
    // double elapsed_time = (end_time.tv_sec - start_time.tv_sec) +
    //                       (end_time.tv_nsec - start_time.tv_nsec) / 1.0e9;
    // printf("Time spent in first loop: %.1f seconds\n", elapsed_time);

    // struct timespec start_time1, end_time1;
    // clock_gettime(CLOCK_MONOTONIC, &start_time1); // Start time
    delete_parent_connections(sheet, &sheet->table[*row][*col] , row , col);
    // clock_gettime(CLOCK_MONOTONIC, &end_time1);   // End time
    // double elapsed_time1 = (end_time1.tv_sec - start_time1.tv_sec) +
    //                       (end_time1.tv_nsec - start_time1.tv_nsec) / 1.0e9;
    // printf("Time spent in delete parent connections: %.1f seconds\n", elapsed_time1);

    // struct timespec start_time2, end_time2;
    // clock_gettime(CLOCK_MONOTONIC, &start_time2); // Start time
    int min_val = INT_MAX;
    for (int i = row1; i <= row2; i++) {
        for (int j = col1; j <= col2; j++) {
            if(sheet->table[i][j].error == '1'){
                sheet->table[*row][*col].error = '1';
            }else{
                sheet->table[*row][*col].error = '0';
            }
            min_val = MIN(min_val, sheet->table[i][j].val);
            add_child(&sheet->table[i][j], hash_index(sheet, *row, *col));
            add_parent(&sheet->table[*row][*col], hash_index(sheet, i, j));
        }
    }
    // clock_gettime(CLOCK_MONOTONIC, &end_time2);   // End time
    // double elapsed_time2 = (end_time2.tv_sec - start_time2.tv_sec) +
    //                       (end_time2.tv_nsec - start_time2.tv_nsec) / 1.0e9;
    // printf("Time spent in second loop: %.1f seconds\n", elapsed_time2);
    sheet->table[*row][*col].val = min_val;
    sheet->table[*row][*col].formula = expr_to_store;
    // struct timespec start_time3, end_time3;
    // clock_gettime(CLOCK_MONOTONIC, &start_time3); // Start time

    recalculate_dependent_cells(sheet , row , col);

    // clock_gettime(CLOCK_MONOTONIC, &end_time3);   // End time
    // double elapsed_time3 = (end_time3.tv_sec - start_time3.tv_sec) +
    //                       (end_time3.tv_nsec - start_time3.tv_nsec) / 1.0e9;
    // printf("Time spent in recalculate dependent cells: %.1f seconds\n", elapsed_time3);
    free(exprdup);
}

void max_handling(spreadsheet* sheet , int *row, int *col , const char *_expr , int *error_code){
    /*
        This function will assign the maximum value of the range to the cell
        error_code is set to 6 if self reference is found, 7 if cycle is found
        Input:  
        - sheet : pointer to the spreadsheet
        - row : pointer to the row index
        - col : pointer to the col index
        - _expr : string which contains the formula of format MAX(_:_)
        - error_code : pointer to store error code
    */
    char *exprdup = strndup(_expr, strlen(_expr));
    char *expr_to_store = strdup(_expr);
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
                *error_code = 6;  // Self reference
                free(exprdup);
                return;
            }
            int dependent_cell_hash = hash_index(sheet, i, j);
            if (check_cycle(sheet, &sheet->table[*row][*col], &dependent_cell_hash)) {
                *error_code = 7;  // Cycle Formation
                free(exprdup);
                return;
            }
        }
    }
    delete_parent_connections(sheet, &sheet->table[*row][*col] , row , col);
    int max_val = INT_MIN;
    for (int i = row1; i <= row2; i++) {
        for (int j = col1; j <= col2; j++) {
            if(sheet->table[i][j].error == '1'){
                sheet->table[*row][*col].error = '1';
            }else{
                sheet->table[*row][*col].error = '0';
            }
            max_val = MAX(max_val, sheet->table[i][j].val);
            add_child(&sheet->table[i][j], hash_index(sheet, *row, *col));
            add_parent(&sheet->table[*row][*col], hash_index(sheet, i, j));
        }
    }
    sheet->table[*row][*col].val = max_val;
    sheet->table[*row][*col].formula = expr_to_store;
    recalculate_dependent_cells(sheet , row , col);
    free(exprdup);
}

void sum_handling(spreadsheet* sheet , int *row , int *col , const char *_expr , int *error_code){
    /*
        This function will assign the sum of all cell values in range to the cell
        error_code will be set to 6 if self reference is found, 7 if cycle is found
        Input:  
        - sheet : pointer to the spreadsheet
        - row : pointer to the row index
        - col : pointer to the col index
        - _expr : string which contains the formula of format SUM(_:_)
        - error_code : pointer to store error code
    */
    char *exprdup = strndup(_expr, strlen(_expr));
    char *expr_to_store = strdup(_expr);
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
               *error_code = 6;  // Self reference
                free(exprdup);
                return;
            }
            int dependent_cell_hash = hash_index(sheet, i, j);
            if (check_cycle(sheet, &sheet->table[*row][*col], &dependent_cell_hash)) {
                *error_code = 7;  // Cycle Formation
                free(exprdup);
                return;
            }
        }
    }
    delete_parent_connections(sheet, &sheet->table[*row][*col] , row , col);
    int sum = 0;
    for (int i = row1; i <= row2; i++) {
        for (int j = col1; j <= col2; j++) {
            if(sheet->table[i][j].error == '1'){
                sheet->table[*row][*col].error = '1';
            }else{
                sheet->table[*row][*col].error = '0';
            }
            sum = sum + sheet->table[i][j].val;
            add_child(&sheet->table[i][j], hash_index(sheet, *row, *col));
            add_parent(&sheet->table[*row][*col], hash_index(sheet, i, j));
        }
    }
    sheet->table[*row][*col].val = sum;
    sheet->table[*row][*col].formula = expr_to_store;
    recalculate_dependent_cells(sheet , row , col);
    free(exprdup);
}

void avg_handling(spreadsheet* sheet , int *row, int *col , const char *_expr ,int *error_code){
    /*
        This function will assign the average of all cell values in range to the cell
        error_code will be set to 6 if self reference is found, 7 if cycle is found
        Input:  
        - sheet : pointer to the spreadsheet
        - row : pointer to the row index
        - col : pointer to the col index
        - _expr : string which contains the formula of format AVG(_:_)
        - error_code : pointer to store error code
    */
    char *exprdup = strndup(_expr, strlen(_expr));
    char *expr_to_store = strdup(_expr);
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
                *error_code = 6;  // Self reference
                free(exprdup);
                return;
            }
            int dependent_cell_hash = hash_index(sheet, i, j);
            if (check_cycle(sheet, &sheet->table[*row][*col], &dependent_cell_hash)) {
               *error_code = 7;  // Cycle Formation
                free(exprdup);
                return;
            }
        }
    }
    delete_parent_connections(sheet, &sheet->table[*row][*col] , row , col);
    float sum = 0;
    float cnt = 0;
    for (int i = row1; i <= row2; i++) {
        for (int j = col1; j <= col2; j++) {
            if(sheet->table[i][j].error == '1'){
                sheet->table[*row][*col].error = '1';
            }else{
                sheet->table[*row][*col].error = '0';
            }
            sum = sum + sheet->table[i][j].val;
            cnt++;
            add_child(&sheet->table[i][j], hash_index(sheet, *row, *col));
            add_parent(&sheet->table[*row][*col], hash_index(sheet, i, j));
        }
    }
    float favg = sum/cnt;
    int avg = round(favg);
    sheet->table[*row][*col].val = avg;
    sheet->table[*row][*col].formula = expr_to_store;
    recalculate_dependent_cells(sheet , row , col);
    free(exprdup);
}

void stdev_handling(spreadsheet *sheet, int *row , int *col , const char *_expr , int *error_code){
     /*
        This function will assign the standard deviation of all cell values in range to the cell
        error_code will be set to 6 if self reference is found, 7 if cycle is found
        Input:  
        - sheet : pointer to the spreadsheet
        - row : pointer to the row index
        - col : pointer to the col index
        - _expr : string which contains the formula of format STDEV(_:_)
        - error_code : pointer to store error code
    */
    char *exprdup = strndup(_expr, strlen(_expr));
    char *expr_to_store = strdup(_expr);
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
                *error_code = 6;  // Self reference
                free(exprdup);
                return;
            }
            int dependent_cell_hash = hash_index(sheet, i, j);
            if (check_cycle(sheet, &sheet->table[*row][*col], &dependent_cell_hash)) {
                *error_code = 7;  // Cycle Formation
                free(exprdup);
                return;
            }
        }
    }
    delete_parent_connections(sheet, &sheet->table[*row][*col] , row , col);
    float sum = 0;
    float cnt = 0;
    for (int i = row1; i <= row2; i++) {
        for (int j = col1; j <= col2; j++) {
            if(sheet->table[i][j].error == '1'){
                sheet->table[*row][*col].error = '1';
            }else{
                sheet->table[*row][*col].error = '0';
            }
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
    sheet->table[*row][*col].formula = expr_to_store;
    recalculate_dependent_cells(sheet , row , col);
    free(exprdup);

}

void sleep_handling(spreadsheet* sheet,int *row , int *col,const char *_expr , int* error_code){
    /*
        This function will sleep the running of program for time in string expression
        if number in positive and assign that value to the cell
        Input:
        - sheet : pointer to the spreadsheet
        - row : pointer to the row index
        - col : pointer to the col index
        - _expr : string which contains the formula of format SLEEP(_:_)
        - error_code : pointer to store error code

    */
    char *exprdup = strdup(_expr);
    char *expr_to_store = strdup(_expr);
    char *fun_end = strchr(exprdup, '(');
    char *range_end = strchr(exprdup, ')');
    char *cell = fun_end + 1;
    *fun_end = '\0';
    *range_end = '\0';
    int delay_time;
    if(is_number(cell)){
        delay_time = atoi(cell);
    }else{
        int dependent_row;
        int dependent_col;
        name_to_indices(cell , &dependent_row , &dependent_col);
        if(*row == dependent_row && *col == dependent_col){
            *error_code = 6; // Self reference
            free(exprdup);
            return;
        }
        int dependent_cell_hash = hash_index(sheet , dependent_row , dependent_col);
        int current_cell_hash = hash_index(sheet , *(row) , *(col));
        if(check_cycle(sheet , &sheet->table[*row][*col] , &dependent_cell_hash)){
            *error_code = 7; // Cycle Formation
            free(exprdup);
            return;
        }
        delete_parent_connections(sheet , &sheet->table[*row][*col] , row , col);
        add_child(&sheet->table[dependent_row][dependent_col] , current_cell_hash);
        add_parent(&sheet->table[*row][*col] , dependent_cell_hash);
        if(sheet->table[dependent_row][dependent_col].error == '1'){
            sheet->table[*row][*col].error = '1';
        }else{
            sheet->table[*row][*col].error = '0';
        }
        delay_time = sheet->table[dependent_row][dependent_col].val;
    }
    sheet->table[*row][*col].val = delay_time;
    sheet->table[*row][*col].vis = 0;
    sheet->table[*row][*col].formula = expr_to_store;
    if(sheet->table[*row][*col].error == '0'){
        if(delay_time > 0){
            sleep(delay_time);
        }
    }
    recalculate_dependent_cells(sheet , row , col);
    free(exprdup);
}

void handle_control_command(char control,spreadsheet *sheet){
    /*
        This function takes control command as a input and changes the bounds of the sheet
        accordingly for w(up),a(left),s(down),d(right)
        Input:
        - control : w/a/s/d
        - sheet : pointer to the spreadsheet
    */

    int num_rows=sheet->rows;
    int num_cols=sheet->cols;
    int *firstrow=sheet->bounds->first_row,*firstcol=sheet->bounds->first_col;
    int *lastrow=sheet->bounds->last_row,*lastcol=sheet->bounds->last_col;    
    switch(control){
        case 'w':
            *firstrow=MAX(1,*firstrow-10);
            *lastrow=MIN(num_rows,*firstrow+9);
            break;
        case 'a':
            *firstcol=MAX(1,*firstcol-10);
            *lastcol=MIN(num_cols,*firstcol+9);
            break;
        case 's':
            *lastrow=MIN(num_rows,*lastrow+10);
            *firstrow=MAX(1,*lastrow-9);
            break;
        case 'd':
            *lastcol=MIN(num_cols,*lastcol+10);
            *firstcol=MAX(1,*lastcol-9);
            break;
        default:
           break;
    }
}

void scroll_to(spreadsheet *sheet, const char *cell , int *error_code){
    /*
        This function takes cell value input and adjust bounds of the sheet
        If input is invalid, error_code is set to 2
        Input:
        - sheet : pointer to the spreadsheet
        - cell : string which contains the formula of format scroll_to {cell}
        - error_code : pointer to store error code
    */
    int num_rows=sheet->rows;
    int num_cols=sheet->cols;
    int *firstrow=sheet->bounds->first_row,*firstcol=sheet->bounds->first_col;
    int *lastrow=sheet->bounds->last_row,*lastcol=sheet->bounds->last_col;  
    int row,col;
    if(!valid_cell(sheet,cell,&row,&col)){
       *error_code = 2;
        return;
    }
    *firstrow=row+1; 
    *firstcol=col+1; 
    *lastrow=MIN(num_rows,*firstrow+9); 
    *lastcol=MIN(num_cols,*firstcol+9); 
    return;
}