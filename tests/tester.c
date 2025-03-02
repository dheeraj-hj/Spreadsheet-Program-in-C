#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/spreadsheet.h"
#include "../include/command_handler.h"
#include "../include/spreadsheet_display.h"
#include "../include/utils.h"

void check_cell_value(spreadsheet *sheet, const char *cell_name, int expected, const char *test_case) {
    int row, col;
    name_to_indices(cell_name, &row, &col);
    if (sheet->table[row][col].val != expected) {
        printf("Test Failed: %s | Expected %d in cell %s, Got %d\n", test_case, expected, cell_name , sheet->table[row][col].val);
    } else {
        printf("Test Passed: %s\n", test_case);
    }
}
void check_cell_error(spreadsheet *sheet, const char *cell_name, char expected, const char *test_case) {
    int row, col;
    name_to_indices(cell_name, &row, &col);
    if (sheet->table[row][col].error != expected) {
        printf("Test Failed: %s | Expected %c in cell %s, Got %c\n", test_case, expected, cell_name , sheet->table[row][col].error);
    } else {
        printf("Test Passed: %s\n", test_case);
    }
}
void check_invalid_input(spreadsheet *sheet , const char *command , const char *test_case , const char *msg){
    int error_code = 0;
    validate_command(sheet , command , &error_code);
    if(error_code != 0){
        printf("Test Passed: %s | %s \n", test_case , msg);
    }else{
        printf("Test Failed: %s | %s \n", test_case , msg);
    }
}
void run_tests(){
    tester_mode = 1;
    tester = 1;
    spreadsheet *sheet = create_spreadsheet(999, 18278);

    parse_command(sheet, "A1=5");
    check_cell_value(sheet, "A1", 5, "A1=5");
    parse_command(sheet, "A2=0005");
    check_cell_value(sheet, "A2", 5, "A2=0005");
    parse_command(sheet, "A3=-35");
    check_cell_value(sheet, "A3", -35, "A3=-35");
    parse_command(sheet, "A4=-0035");
    check_cell_value(sheet, "A4", -35, "A4=-0035");
    parse_command(sheet, "A5=+11");
    check_cell_value(sheet, "A5", 11 , "A5=+11");

    parse_command(sheet , "A7=A1");
    check_cell_value(sheet, "A7", 5, "A7=A1");
    
    parse_command(sheet , "A8=3+5");
    check_cell_value(sheet, "A8", 8, "A8=3+5");
    parse_command(sheet , "A9=3-5");
    check_cell_value(sheet, "A9", -2, "A9=3-5");
    parse_command(sheet , "A10=-3+5");
    check_cell_value(sheet, "A10", 2, "A10=-3+5");
    parse_command(sheet , "A11=-3-5");
    check_cell_value(sheet, "A11", -8, "A11=-3-5");
    parse_command(sheet , "A12=3*5");
    check_cell_value(sheet, "A12", 15, "A12=3*5");
    parse_command(sheet , "A13=-3*5");
    check_cell_value(sheet, "A13", -15, "A13=-3*5");
    parse_command(sheet , "A14=18/5");
    check_cell_value(sheet, "A14", 3, "A14=18/5");
    parse_command(sheet , "A15=-18/5");
    check_cell_value(sheet, "A15", -3, "A15=-18/5");

    parse_command(sheet , "A16=A1+A2");
    check_cell_value(sheet, "A16", 10, "A16=A1+A2");
    parse_command(sheet , "A17=A1-A3");
    check_cell_value(sheet, "A17", 40, "A17=A1-A3");
    parse_command(sheet , "A18=A1*A2");
    check_cell_value(sheet, "A18", 25, "A18=A1*A2");
    parse_command(sheet , "A19=A1/A2");
    check_cell_value(sheet, "A19", 1, "A19=A1/A2");
    parse_command(sheet , "A20=-A1+A2");
    check_cell_value(sheet, "A20", 0, "A20=-A1+A2");
    parse_command(sheet , "A21=-A1-A2");
    check_cell_value(sheet, "A21", -10, "A21=-A1-A2");
    parse_command(sheet , "A22=-A1*A2");
    check_cell_value(sheet, "A22", -25, "A22=-A1*A2");
    parse_command(sheet , "A23=-A1/A2");
    check_cell_value(sheet, "A23", -1, "A23=-A1/A2");

    parse_command(sheet , "A24=A1/0");
    check_cell_error(sheet, "A24", '1', "A24=A1/0");
    parse_command(sheet , "A25=A1/B1");
    check_cell_error(sheet, "A25", '1', "A25=A1/B1");

    parse_command(sheet , "A26=MIN(A1:D5)");
    check_cell_value(sheet, "A26", -35, "A26=MIN(A1:D5)");
    parse_command(sheet , "A27=MAX(A1:D5)");
    check_cell_value(sheet, "A27", 11, "A27=MAX(A1:D5)");
    parse_command(sheet , "A28=AVG(A1:D5)");
    check_cell_value(sheet, "A28", -2, "A28=AVG(A1:D5)");
    parse_command(sheet , "A29=SUM(A1:D5)");
    check_cell_value(sheet, "A29", -49, "A29=SUM(A1:D5)");
    parse_command(sheet , "A30=STDEV(A1:D5)");
    check_cell_value(sheet, "A30", 11, "A30=STDEV(A1:D5)");
    parse_command(sheet , "A31=SLEEP(3)");
    check_cell_value(sheet, "A31", 3, "A31=SLEEP(3)");
    parse_command(sheet , "A32=SLEEP(A1)");
    check_cell_value(sheet, "A32", 5, "A32=SLEEP(A1)");

    parse_command(sheet , "A1=3");
    check_cell_value(sheet , "A22" , -15 , "A1=3 | Recalcuation test for dependent cell" );
    check_cell_value(sheet , "A23" , 0 , "A1=3 | Recalcuation test for dependent cell" );
    check_cell_value(sheet , "A20" , 2 , "A1=3 | Recalcuation test for dependent cell" );
    check_cell_value(sheet , "A21" , -8 , "A1=3 | Recalcuation test for dependent cell" );
    check_cell_value(sheet , "A18" , 15 , "A1=3 | Recalcuation test for dependent cell" );
    check_cell_value(sheet , "A19" , 0 , "A1=3 | Recalcuation test for dependent cell" );
    check_cell_value(sheet , "A16" , 8 , "A1=3 | Recalcuation test for dependent cell" );
    check_cell_value(sheet , "A17" , 38 , "A1=3 | Recalcuation test for dependent cell" );
    check_cell_value(sheet , "A26" , -35 , "A1=3 | Recalcuation test for dependent cell" );
    check_cell_value(sheet , "A27" , 11 , "A1=3 | Recalcuation test for dependent cell" );
    check_cell_value(sheet , "A28" , -2 , "A1=3 | Recalcuation test for dependent cell" );
    check_cell_value(sheet , "A29" , -51 , "A1=3 | Recalcuation test for dependent cell" );
    check_cell_value(sheet , "A30" , 11 , "A1=3 | Recalcuation test for dependent cell" );
    check_cell_value(sheet , "A32" , 3 , "A1=3 | Recalcuation test for dependent cell" );

    int err = 0;
    validate_command(sheet , "B1=B1+2" , &err);
    if(err != 0){
        printf("Test Passed: B1=B1+2 | Cycle Detection test\n");
    }
    err = 0;
    validate_command(sheet , "B1=B2+2" , &err);
    validate_command(sheet, "B2=B1+2", &err);
    if(err != 0){
        printf("Test Passed: Cycle Detection test\n");
    }
    parse_command(sheet , "s");
    if(*sheet->bounds->first_row == 11 && *sheet->bounds->last_row == 20 && *sheet->bounds->first_col == 1 && *sheet->bounds->last_col == 10){
        printf("Test Passed: Scroll functionality | s\n");
    }
    parse_command(sheet , "w");
    if(*sheet->bounds->first_row == 1 && *sheet->bounds->last_row == 10 && *sheet->bounds->first_col == 1 && *sheet->bounds->last_col == 10){
        printf("Test Passed: Scroll functionality | w\n");
    }
    parse_command(sheet , "d");
    if(*sheet->bounds->first_row == 1 && *sheet->bounds->last_row == 10 && *sheet->bounds->first_col == 11 && *sheet->bounds->last_col == 20){
        printf("Test Passed: Scroll functionality | d\n");
    }
    parse_command(sheet , "a");
    if(*sheet->bounds->first_row == 1 && *sheet->bounds->last_row == 10 && *sheet->bounds->first_col == 1 && *sheet->bounds->last_col == 10){
        printf("Test Passed: Scroll functionality | a\n");
    }
    parse_command(sheet , "scroll_to ZZZ999");
    if(*sheet->bounds->first_row == 999 && *sheet->bounds->last_row == 999 && *sheet->bounds->first_col == 18278 && *sheet->bounds->last_col == 18278){
        printf("Test Passed: scroll_to functionality\n");
    }

    check_invalid_input(sheet , "scroll_to ZZZ9999" , "scroll_to ZZZ9999" , "Invalid cell name");
    check_invalid_input(sheet , "ww" , "ww" , "Invalid command");
    check_invalid_input(sheet , "scroll_to" , "scroll_to" , "Invalid command");
    check_invalid_input(sheet , "A1++" , "A1++" , "Invalid command");
    check_invalid_input(sheet , "A1=3+3+" , "A1=3+3+" , "Invalid command");
    check_invalid_input(sheet, "random invalid input", "random invalid input", "Invalid command");
    check_invalid_input(sheet , "A0034=4" , "A0034=4" , "Invalid cell name");
    check_invalid_input(sheet , "A3=MAX(F3:A1)" , "A3=MAX(F3:A1)" , "Invalid range");

}
int main(){
    run_tests();
    return 0;
}
