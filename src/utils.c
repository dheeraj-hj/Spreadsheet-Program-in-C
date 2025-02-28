#include "../include/utils.h"
#include "../include/spreadsheet.h"
#include "stdlib.h"
#include "ctype.h"
#include "string.h"
#include "limits.h"
#include "math.h"
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define INIT_SIZE 10

void name_to_indices(const char *name, int *row, int *col){
    /*
        Takes cell as input and break it down to row and column index and stores it in
        row and col
        - name : string cell, eg A2,CX394 
        - row : pointer to the row index
        - col : pointer to the col index

    */
    *col=0;
    *row=0;
    int i= 0;
    while (isalpha(name[i])) {
        *col= *col * 26 +(name[i] -'A' +1)  ;
        i++ ;
    }
    
    while(isdigit(name[i])) {
        *row=  *row*10 + (name[i]- '0');
        i++ ;
    }
    *row-=1;
    *col-=1;
}
void range_to_indices(const char *range, int *row1, int *col1, int *row2, int *col2){
    /*
        This function will analyze range and stores upper and lower bound values of range
        in row1,col1 and row2,col2. If given range string is invalid row1 col1 row2 col2
        would be storing -1 
        Input:
        - range : string range of format {cell}:{cell}
        - row1 , col1, row2 , col2 : pointer to corresponding integers 
    */
    char start_cell[10] ={0} ;
    char end_cell[10] ={0};
    int i =0,j  = 0 ;
    while (isalpha(range[i])  || isdigit(range[i])) {
        start_cell[j++] =  range[i++]  ;
    }
    start_cell[j] ='\0';
    if (range[i] == ':') {
        i++ ;
    } else {
        *row1 = *col1  = *row2 = *col2 =-1 ;
        return ;
    }
    j= 0 ;
    while (isalpha(range[i]) || isdigit(range[i])) {
        end_cell[j++] = range[i++];
    }
    end_cell[j] ='\0' ;
    name_to_indices(start_cell, row1, col1);
    name_to_indices(end_cell, row2, col2) ;
    if(*row1 <0 || *col1 <0 || *row2< 0 || *col2 < 0) {
        *row1 = *col1 = *row2 = *col2 =-1 ;
    }
    return;
}


char* colIndex_to_name(int i){
    // function assumses that i lies between 0 and 18,277
    static char col_name[3];
    int index = 0;

    while (i >=0) {
        col_name[index++] = (i % 26) + 'A';
        i = (i/26)-1;
        if (i< 0) break;
    }

    // col_name[index] ='\0';
    
    for (int j = 0, k = index - 1; j < k; j++, k--) {
            char temp = col_name[j];
            col_name[j] = col_name[k];
            col_name[k] = temp;
        }
    
    return col_name;

}

int is_number(const char *val) {
    /*
        This function checks if the input string is a number
        Input:
        - val : input string
        Return:
        - 0 : if string is not a number
        - 1 : if string is a number
    */
    int i;
    if(val[0] == '-' || val[0] == '+'){
        i = 1;
    }
    else{
        i = 0;
    }
    for (; val[i] != '\0'; i++) {
        if (!(val[i] >= '0' && val[i] <= '9')) {
            return 0;
        }
    }
    return 1;
}

int valid_cell(spreadsheet* sheet ,const char *_cell , int *row_id, int *col_id){
    /*
        This function takes a cell value input and checks weather the input cell is in correct format 
        and cell bounded inside the sheet
        Input:
        - sheet : pointer to the spreadsheet
        - _cell : cell value input
        - row_id : pointer to store row index in zero based index
        - col_id : pointer to store column index in zero based index
        Return:
        - 0 : if cell is invalid
        - 1 : if cell is valid and row_id, col_id stores the 0 based index of row and column
    */
    char* cell = strdup(_cell);
    int i = 0;
    while(cell[i] && isalpha(cell[i])){
        i++;
    }
    if(i == 0){
        free(cell);
        return 0;
    }
    char col[i + 1];
    strncpy(col , cell , i);
    col[i] = '\0';

    char *row_str = cell + i;
    int j = 0;
    while(row_str[j]){
        if(!isdigit(row_str[j])){
            free(cell);
            return 0;
        }
        j++;
    }
    if(j == 0) return 0;
    *row_id = atoi(row_str);
    *col_id = 0;
    char *temp_col = col;
    while(*temp_col){
        if(*temp_col < 'A' || *temp_col > 'Z'){
            free(cell);
            return 0;
        }
        *col_id = *col_id * 26 + (*temp_col - 'A' + 1);
        temp_col++;
    }
    
    if(*col_id > sheet->cols || *row_id > sheet->rows){
        free(cell);
        return 0;
    }
    *row_id = *row_id - 1;
    *col_id = *col_id -1; 
    free(cell);
    return 1;

}

int valid_range(spreadsheet *sheet ,const char *_range , int *error_code){
    /*
        This function takes a range value input and checks weather the input range is in correct format 
        and range bounded inside the sheet
        Input:
        - sheet : pointer to the spreadsheet
        - _range : range value input
        - error_code : pointer to store the error code (1 if range is invalid)
        Return:
        - 0 : if range is invalid
        - 1 : if range is valid
    */
    char *range = strdup(_range);
    char *colon = strchr(range , ':');
    if(colon){
        *colon = '\0';
        char *start = range;
        char *end = colon + 1;
        int row_topleft , col_topleft , row_bottomright , col_bottomright;
        if(valid_cell(sheet , start , &row_topleft , &col_topleft) && valid_cell(sheet , end , &row_bottomright , &col_bottomright)){
            if(row_topleft <= row_bottomright && col_topleft <= col_bottomright){
                free(range);
                return 1;
            }else{
                *error_code = 1;
                free(range);
                return 0;
            }
        }
    }
    free(range);
    return 0;
}

int valid_expression(spreadsheet* sheet, const char *_expression , int *expr_type , int *error_code){
/*
    This function will check weather input expression is valid and assigns the type of expression
    Value of expr_type (if valid expression):
    0 - number
    1 - cell
    2 - val (operator) val
    3 - MIN
    4 - MAX
    5 - AVG
    6 - SUM
    7 - STDEV
    8 - SLEEP(number)
    9 - SLEEP(cell)
    Input:
    - sheet : pointer to the spreadsheet
    - _expression : expression to validate
    - expr_type : pointer to store the type of expression
    - error_code : pointer to store the error code
    Return:
    - 0 : if expression is invalid
    - 1 : if expression is valid and expr_type stores the type of expression
*/
    char *expression = strdup(_expression);
    int row_id;
    int col_id;
    if(is_number(expression)){
        *expr_type = 0;
        free(expression);
        return 1;
    }
    else if(valid_cell(sheet , expression , &row_id , &col_id)){
        *expr_type = 1;
        free(expression);
        return 1;
    }
    else if((strncmp(expression, "MIN(" , 4) == 0) ){
        char *fun_end = strchr(expression , '(');
        char *range_end = strchr(expression , ')');
        if(fun_end && range_end && range_end > fun_end){
            char *range = fun_end + 1;
            *fun_end = '\0';
            *range_end = '\0';
            if(valid_range(sheet , range , error_code)){
                *expr_type = 3;
                free(expression);
                return 1;
            }
        }
    }
    else if ((strncmp(expression, "MAX(" , 4) == 0) ){
        char *fun_end = strchr(expression , '(');
        char *range_end = strchr(expression , ')');
        if(fun_end && range_end && range_end > fun_end){
            char *range = fun_end + 1;
            *fun_end = '\0';
            *range_end = '\0';
            if(valid_range(sheet , range , error_code)){
                *expr_type = 4;
                free(expression);
                return 1;
            }
        }
    }
    else if ((strncmp(expression, "AVG(" , 4) == 0)) {
        char *fun_end = strchr(expression , '(');
        char *range_end = strchr(expression , ')');
        if(fun_end && range_end && range_end > fun_end){
            char *range = fun_end + 1;
            *fun_end = '\0';
            *range_end = '\0';
            if(valid_range(sheet , range , error_code)){
                *expr_type = 5;
                free(expression);
                return 1;
            }
        }
    }
    else if ((strncmp(expression, "SUM(" , 4) == 0)) {
        char *fun_end = strchr(expression , '(');
        char *range_end = strchr(expression , ')');
        if(fun_end && range_end && range_end > fun_end){
            char *range = fun_end + 1;
            *fun_end = '\0';
            *range_end = '\0';
            if(valid_range(sheet , range , error_code)){
                *expr_type = 6;
                free(expression);
                return 1;
            }
        }
    }
    else if ((strncmp(expression, "STDEV(" , 6) == 0)) {
        char *fun_end = strchr(expression , '(');
        char *range_end = strchr(expression , ')');
        if(fun_end && range_end && range_end > fun_end){
            char *range = fun_end + 1;
            *fun_end = '\0';
            *range_end = '\0';
            if(valid_range(sheet , range , error_code)){
                *expr_type = 7;
                free(expression);
                return 1;
            }
        }
    }
    else if((strncmp(expression , "SLEEP(" , 6) == 0)){
        char *range_end = strchr(expression , ')');
        char *fun_end = strchr(expression , '(');
        if(fun_end && range_end && range_end > fun_end){
            char *time = fun_end + 1;
            *fun_end = '\0';
            *range_end = '\0';
            if(is_number(time)){
                *expr_type = 8;
                free(expression);
                return 1;
            }
            else if(valid_cell(sheet , time,  &row_id , &col_id)){
                *expr_type = 9;
                free(expression);
                return 1;
            }
            
        }

    }
    else if(strchr(expression, '+') || strchr(expression, '-') || strchr(expression, '*') || strchr(expression, '/')){
        char *operators = "+-*/";
        char *op = strpbrk(expression+1 , operators);
        if(op){

            char *left = expression;
            char *right = op+1;
            *op = '\0';
            if(left[0] == '-' || left[0] == '+'){
                left++;
            }
            if((is_number(left) || valid_cell(sheet , left ,  &row_id , &col_id)) && (is_number(right) || valid_cell(sheet , right, &row_id , &col_id))){
                *expr_type = 2;
                free(expression);
                return 1;
            }
        }
    }
    free(expression);
    return 0;
}

void add_child(cell *c, int child_hash) {
    /*
        This function will add the child hash value to a cell if it is not already present
        Input:
        - c : cell pointer
        - child_hash : integer value of child hash to store
    */
    int left = 0, right = c->children.size - 1;
    while(left <= right) {
        int mid = left + (right - left)/2;
        if(c->children.data[mid] == child_hash) return;
        if(c->children.data[mid] < child_hash) left = mid + 1;
        else right = mid - 1;
    }
    if(c->children.size >= c->children.capacity) {
        c->children.capacity = c->children.capacity ? c->children.capacity * 2 : 4;
        c->children.data = (int *)realloc(c->children.data, c->children.capacity * sizeof(int));
    }
    for(int i = c->children.size; i > left; i--) {
        c->children.data[i] = c->children.data[i-1];
    }
    c->children.data[left] = child_hash;
    c->children.size++;
    // printf("Added child\n");
    // printf("Size of children %d\n", c->children.size);
}

void add_parent(cell *c, int parent_hash) {
    /*
        This function will add the parent hash value to a cell 
        Input:
        - c : cell pointer
        - parent_hash : integer value of parent hash to store
    */
    int left = 0, right = c->parents.size - 1;
    while(left <= right) {
        int mid = left + (right - left)/2;
        if(c->parents.data[mid] == parent_hash) return;
        if(c->parents.data[mid] < parent_hash) left = mid + 1;
        else right = mid - 1;
    }
    if(c->parents.size >= c->parents.capacity) {
        c->parents.capacity = c->parents.capacity ? c->parents.capacity * 2 : 4;
        c->parents.data = (int *)realloc(c->parents.data, c->parents.capacity * sizeof(int));
    }
    for(int i = c->parents.size; i > left; i--) {
        c->parents.data[i] = c->parents.data[i-1];
    }
    c->parents.data[left] = parent_hash;
    c->parents.size++;
    // printf("Added parent\n");
    // printf("Size of parents %d\n", c->parents.size);
}

int hash_index(spreadsheet *sheet , int row, int col) {
    /*
        Returns the hash value of a cell in following format
        ( for nx6 grid 
          0  1  2  3  4   5
          6  7  8  9  10  11
          ..... )
        Input :
        - sheet : pointer to the spreadsheet
        - row : integer row of a cell
        - col : integer column of a cell
    */
    return (row * sheet->cols) + col;
}

int check_cycle(spreadsheet *sheet ,cell *c, int* target_cell_hash){
    /*
        This function reccursively checks the presence of cycle
        It checks weather the given target cell hash values present 
        anywhere among their childern
        Input:
        - sheet : pointer to the spreadsheet
        - c : cell pointer
        - target_cell_hash : point to hash value of target cell
        Return:
        0 : If there is no cycle
        1 : If there is a presence of cycle
    */
    Stack *stk = createStack(2);
    int found = 0;
    for(int i = 0; i < c->children.size; i++){
        push(stk , c->children.data[i]);
    }
    while(!isEmpty(stk)){
        int current_cell_hash = pop(stk);
        int col = current_cell_hash % sheet->cols;
        int row = current_cell_hash / sheet->cols;
        if(current_cell_hash == *target_cell_hash){
            found = 1;
            break;
        }
        for(int i = 0; i < sheet->table[row][col].children.size; i++){
            push(stk , sheet->table[row][col].children.data[i]);
        }
    }
    free(stk->array);
    free(stk);
    return found;
}

void delete_parent_connections(spreadsheet *sheet, cell *c , int *row , int *col){
    /*
        This function deletes all the connection from its parent and frees the parent array
        Input:
        - sheet : pointer to the spreadsheet
        - c : cell pointer
        - row : pointer to the row index
        - col : pointer to the col index
    */
    int current_cell_hash =  ((*(row)) * sheet->cols) + (*(col));
    // printf("Deleting parent connections\n");
    for(size_t i = 0; i < c->parents.size; i++){
        // printf("I am in loop\n");

        int parent_hash = c->parents.data[i];
        int prow = parent_hash / sheet->cols;
        int pcol = parent_hash % sheet->cols;
        cell *parent = &sheet->table[prow][pcol];
        int left = 0, right = parent->children.size - 1;
        int index = -1;
        while(left <= right){
            int mid = left + (right-left)/2;
            if(parent->children.data[mid] == current_cell_hash){
                index = mid;
                break;
            }
            if(parent->children.data[mid] < current_cell_hash){
                left = mid + 1;
            }
            else{
                right = mid - 1;
            }
        }
        if(index == -1){
            printf("Error in deleting parent connections\n");
            return;
        }
        for(int j = index; j < parent->children.size - 1; j++){
            parent->children.data[j] = parent->children.data[j+1];
        }
        parent->children.size--;

        if(parent->children.size <= parent->children.capacity/4 && parent->children.capacity > 4){
            parent->children.capacity /= 2;
            parent->children.data = realloc(parent->children.data , parent->children.capacity * sizeof(int));
        }
    }
    free(c->parents.data);
    c->parents.data = NULL;
    c->parents.size = c->parents.capacity = 0;
    // printf("Deleted parent connections\n");
}

void dfs(spreadsheet *sheet , int row , int col , Stack *stk){
    /*
        This performs dfs if visit value of cell is 0
    */
    // sheet->table[row][col].vis = 1;
    // for(int i = 0; i < sheet->table[row][col].children.size; i++){
    //     int c_col = sheet->table[row][col].children.data[i] % sheet->cols;
    //     int c_row = sheet->table[row][col].children.data[i] / sheet->cols;
    //     // printf("Child row in dfs %d col %d and vis is %d\n", c_row , c_col , sheet->table[c_row][c_col].vis);
    //     if(!sheet->table[c_row][c_col].vis){
    //         dfs(sheet , c_row , c_col , stk);
    //     }
    // }
    // push(stk , hash_index(sheet , row , col));
    Stack *traversal_stack = createStack(16);
    int current_hash = row * sheet->cols + col;
    push(traversal_stack, (current_hash << 1) | 0);  // Encode unprocessed state

    while (!isEmpty(traversal_stack)) {
        int encoded = pop(traversal_stack);
        int processed = encoded & 1;
        current_hash = encoded >> 1;
        int current_row = current_hash / sheet->cols;
        int current_col = current_hash % sheet->cols;
        if (processed) {
            push(stk, current_hash); 
            continue;
        }
        if (sheet->table[current_row][current_col].vis) continue;
        sheet->table[current_row][current_col].vis = 1;
        push(traversal_stack, (current_hash << 1) | 1);
        for (int i = sheet->table[current_row][current_col].children.size - 1; i >= 0; i--) {
            int child_hash = sheet->table[current_row][current_col].children.data[i];
            int child_row = child_hash / sheet->cols;
            int child_col = child_hash % sheet->cols;
            
            if (!sheet->table[child_row][child_col].vis) {
                push(traversal_stack, (child_hash << 1) | 0);
            }
        }
    }
    free(traversal_stack->array);
    free(traversal_stack);
}


void recalculate_dependent_cells(spreadsheet *sheet , int *row ,int *col){
    /*
        This will perform Topological sort algorithm to recalculate all its dependent cells
        Input :
        - sheet : pointer to the spreadsheet
        - row : pointer to the row index
        - col : pointer to the col index
    */
    Stack *stk = createStack(INIT_SIZE);   
    dfs(sheet , *row , *col , stk);
    pop(stk);
    sheet->table[*row][*col].vis = 0;
    while(!isEmpty(stk)){
        int cell_hash = pop(stk);
        // printf("Cell hash %d\n", cell_hash);
        int _col = cell_hash % sheet->cols;
        int _row = cell_hash / sheet->cols;

        evaluate_cell(sheet , _row , _col);
    } 
    // dfs2(sheet , *row , *col);
}