#include "src/spreadsheet.h"
#include "src/command_handler.h"
#include "src/spreadsheet_display.h"
#include "src/utils.h"
#include "stdlib.h"
#include "stddef.h"
#include "string.h"

int main(void){
    int rows=55;
    int cols=55;
    spreadsheet *sheet = create_spreadsheet(rows, cols);
    display_spreadsheet(sheet);
    char * range = "A1:ZZZ999" ;
    int row1 =0 ,col1 = 0;
    int row2 =0 ,col2 = 0;
    range_to_indices(range,&row1, &col1,&row2, &col2) ;
//    printf("%s", colIndex_to_name(28));
    
//    printf("Row1: %d, Column1: %d, Row2: %d, Column2: %d\n", row1, col1,row2,col2);
    return 0;
}
