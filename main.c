#include <stdio.h>
#include <stdlib.h>
#include "src/spreadsheet.h"
#include "src/spreadsheet_display.h"
#include "src/command_handler.h"
#include "src/utils.h"

int main(){
    int rows=100,cols=100;
    spreadsheet *sheet=create_spreadsheet(rows,cols);
    // display_spreadsheet(sheet);
    // handle_control_command('d',sheet);
    // handle_control_command('s',sheet);
    // display_spreadsheet(sheet);
    // int row,col;
    // name_to_indices("D5", &row, &col);
    // printf("%d %d\n",col+1,row+1);
    scroll_to(sheet,"CD9");
    display_spreadsheet(sheet);
    
}