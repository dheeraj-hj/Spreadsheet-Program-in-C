#include "src/spreadsheet.h"
#include "src/command_handler.h"
#include "src/spreadsheet_display.h"
#include "src/utils.h"
#include "stdlib.h"
#include "stddef.h"
#include "string.h"

int main(){
    int rows=5;
    int cols=5;
    spreadsheetbounds *bounds=(spreadsheetbounds *)malloc(sizeof(spreadsheetbounds));
    spreadsheet *sheet = create_spreadsheet(rows, cols, bounds);
    printf("%d\n", bounds->first_row);
}