#include "src/spreadsheet.h"
#include "src/command_handler.h"
#include "src/spreadsheet_display.h"
#include "src/utils.h"
#include "stdlib.h"
#include "stddef.h"
#include "string.h"

char ex[] = {"MIN(A1:A2)"};
int main(){
    int rows = 5;
    int cols = 5;
    spreadsheet *sheet = create_spreadsheet(rows, cols);
    int a = 0;
    int b = 0;
    int c = 0; 
    int d = 1;
    int e = 0;
    int f = 2;
    int g = 1;
    int h = 0;
    int i = 1;
    int j = 1;
    int k = 1;
    int l = 2;
    number_assign(sheet , &a , &b , "5");
    display_spreadsheet(sheet);
    number_assign(sheet , &c , &d , "1");
    display_spreadsheet(sheet);
    operator_assign(sheet , &e , &f , "B1+A1" );//C1
    display_spreadsheet(sheet);
    operator_assign(sheet , &g , &h , "B1+3" );//A2
    display_spreadsheet(sheet);
    number_assign(sheet , &i , &j , "5");//B2
    display_spreadsheet(sheet);
    min_handling(sheet , &i, &j , "MIN(A1:C1)");//C2
    display_spreadsheet(sheet);
    stdev_handling(sheet , &k , &l , "STDEV(A1:B2)");
    display_spreadsheet(sheet);
    // for(int i = 0 ; i < 2 ; i++){
    //     for(int j = 0 ; j < 3 ; j++){
    //         for(int k = 0 ; k < sheet->table[i][j].num_children ; k++){
    //             printf("Child of %d %d : %d\n" , i , j , sheet->table[i][j].children[k]);
    //         }
    //     }
    // }
}
