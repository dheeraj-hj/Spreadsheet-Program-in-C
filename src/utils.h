#ifndef UTILS_H
#define UTILS_H

void name_to_indices(const char *name, int *row, int *col); // Convert the cell name to row and column indices
char* trim_space(char *str); // Trim the leading and trailing spaces from a string
int is_number(const char *val); // Check if a string is a number
// Still more functions to be added

#endif