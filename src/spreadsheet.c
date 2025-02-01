#include "spreadsheet.h"
#include "utils.h"
#include "stdlib.h"
#include "stddef.h"
#include "string.h"
#include "math.h"
#include "unistd.h"
#include "spreadsheet_display.h"
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define MAX(x, y) (((x) > (y)) ? (x) : (y))

spreadsheet *create_spreadsheet(int rows, int cols){
    spreadsheet *s = (spreadsheet *)malloc(sizeof(spreadsheet));
    s->rows = rows;
    s->cols = cols;
    s->table = (cell **)malloc(rows * sizeof(cell *));
    s->bounds=(spreadsheetbounds *)malloc(sizeof(spreadsheetbounds));
    s->bounds=(spreadsheetbounds *)malloc(sizeof(spreadsheetbounds));
    for(int i = 0; i < rows; i++){
        s->table[i] = (cell *)malloc(cols * sizeof(cell));
        for(int j = 0; j < cols; j++){
            s->table[i][j].val = 0;
            s->table[i][j].vis = 0;
            s->table[i][j].formula = NULL;
            s->table[i][j].parents = NULL;
            s->table[i][j].children = NULL;
            s->table[i][j].num_parents = 0;
            s->table[i][j].num_children = 0;
        }
    }
    s->bounds->first_row=(int *)malloc(sizeof(int));
    s->bounds->first_col=(int *)malloc(sizeof(int));
    s->bounds->last_row=(int *)malloc(sizeof(int));
    s->bounds->last_col=(int *)malloc(sizeof(int));
    *(s->bounds->first_row)=1;
    *(s->bounds->first_col)=1;
    *(s->bounds->last_row)=MIN(rows,10);
    *(s->bounds->last_col)=MIN(cols,10);
    display_spreadsheet(s);
    display_status("OK",0);
    return s;
}

void evaluate_cell(spreadsheet *sheet , int row , int col){
    if(sheet->table[row][col].formula == NULL){
        return;
    }
    char *formula = strdup(sheet->table[row][col].formula);
    if(strncmp(formula, "MIN(", 4) == 0 ){
        int row1, col1, row2, col2;
        range_to_indices(formula + 4, &row1, &col1, &row2, &col2);
        int min_val = sheet->table[row1][col1].val;
        for(int i = row1; i <= row2; i++){
            for(int j = col1; j <= col2; j++){
                min_val = MIN(min_val, sheet->table[i][j].val);
            }
        }
        sheet->table[row][col].val = min_val;
    }
    else if(strncmp(formula , "MAX(" , 4) == 0){
        int row1, col1, row2, col2;
        range_to_indices(formula + 4, &row1, &col1, &row2, &col2);
        int max_val = sheet->table[row1][col1].val;
        for(int i = row1; i <= row2; i++){
            for(int j = col1; j <= col2; j++){
                max_val = MAX(max_val, sheet->table[i][j].val);
            }
        }
        sheet->table[row][col].val = max_val;
    }
    else if(strncmp(formula , "AVG(" , 4) == 0){
        int row1, col1, row2, col2;
        range_to_indices(formula + 4, &row1, &col1, &row2, &col2);
        float sum = 0;
        float cnt = 0;
        for(int i = row1; i <= row2; i++){
            for(int j = col1; j <= col2; j++){
                sum = sum + sheet->table[i][j].val;
                cnt++;
            }
        }
        float favg = sum/cnt;
        int avg = round(favg);
        sheet->table[row][col].val = avg;
    }
    else if(strncmp(formula , "SUM(" , 4) == 0){
        int row1, col1, row2, col2;
        range_to_indices(formula + 4, &row1, &col1, &row2, &col2);
        int sum = 0;
        for(int i = row1; i <= row2; i++){
            for(int j = col1; j <= col2; j++){
                sum = sum + sheet->table[i][j].val;
            }
        }
        sheet->table[row][col].val = sum;
    }
    else if(strncmp(formula , "STDEV(" , 6) == 0){
        int row1, col1, row2, col2;
        range_to_indices(formula + 6, &row1, &col1, &row2, &col2);
        float sum = 0;
        float cnt = 0;
        for(int i = row1; i <= row2; i++){
            for(int j = col1; j <= col2; j++){
                sum = sum + sheet->table[i][j].val;
                cnt++;
            }
        }
        float mean = sum/cnt;
        float values = 0.0;
        for(int i = row1; i <= row2; i++){
            for(int j = col1; j <= col2; j++){
                values += pow(sheet->table[i][j].val - mean , 2);
            }
        }
        float stddev = sqrt(values/cnt);
        int sd = round(stddev);
        sheet->table[row][col].val = sd;
    }
    else if(strncmp(formula , "SLEEP(" , 6) == 0){
        char *cell_end = strchr(formula , ')');
        char *cell = formula + 6;
        *cell_end = '\0';
        int delay_time;
        if(is_number(cell)){
            delay_time = atoi(cell);
        }
        else{
            int cell_row;
            int cell_col;
            name_to_indices(cell , &cell_row , &cell_col);
            delay_time = sheet->table[cell_row][cell_col].val;
        }
        sheet->table[row][col].val = delay_time;
        sleep(delay_time);
    }
    else{
        int left_val;
        int right_val;
        int left_row=-1;
        int left_col;
        int right_row=-1;
        int right_col;
        int left_cell_hash;
        int right_cell_hash;
        char *operators = "+-*/";
        char *op = strpbrk(formula+1 , operators);
        char *left = formula;
        char *right = op + 1;
        char operation = *op;
        for(int i = 0; i < strlen(left); i++){
            if(left[i] == '+' || left[i] == '-' || left[i] == '*' || left[i] == '/'){
                left[i] = '\0';
            }
        }
        if(is_number(left)){
            left_val = atoi(left);
        }
        else{
            name_to_indices(left , &left_row , &left_col);
            left_val = sheet->table[left_row][left_col].val;
        }
        if(is_number(right)){
            right_val = atoi(right);
        }
        else{
            name_to_indices(right , &right_row , &right_col);
            right_val = sheet->table[right_row][right_col].val;
        }
        switch(operation){
            case '+':
                sheet->table[row][col].val = left_val + right_val;
                break;
            case '-':
                sheet->table[row][col].val = left_val - right_val;
                break;
            case '*':
                sheet->table[row][col].val = left_val * right_val;
                break;
            case '/':
                sheet->table[row][col].val = left_val / right_val;
                break;
            default:
                break;
        }
    }
    
}

Stack* createStack(int capacity) {
    Stack* stack = (Stack*)malloc(sizeof(Stack));
    stack->capacity = capacity;
    stack->top = -1;
    stack->array = (int*)malloc(stack->capacity * sizeof(int));
    return stack;
}

int isEmpty(Stack* stack) {
    return stack->top == -1;
}

void resizeStack(Stack* stack) {
    stack->capacity *= 2; // Double the capacity
    stack->array = (int*)realloc(stack->array, stack->capacity * sizeof(int));
}

void push(Stack* stack, int item) {
    if (stack->top == stack->capacity - 1) {
        resizeStack(stack); // Increase the size of the stack if it's full
    }
    stack->array[++stack->top] = item;
}

int pop(Stack* stack) {
    return stack->array[stack->top--];
}