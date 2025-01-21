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
    return 0;
}
