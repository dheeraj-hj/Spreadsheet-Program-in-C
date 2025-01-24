#include "src/spreadsheet.h"
#include "src/command_handler.h"
#include "src/spreadsheet_display.h"
#include "src/utils.h"
#include "stdlib.h"
#include "stddef.h"
#include "string.h"

int main(){
    int rows=15;
    int cols=53;
    spreadsheetbounds *bounds=(spreadsheetbounds *)malloc(sizeof(spreadsheetbounds));
    spreadsheet *sheet = create_spreadsheet(rows, cols, bounds);
    display_sheet(sheet);
}
