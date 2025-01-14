#include "utils.h"
#include "stdlib.h"

int is_number(const char *val) {
    for (int i = 0; val[i] != '\0'; i++) {
        if (!(val[i] >= '0' && val[i] <= '9')) {
            return 0;
        }
    }
    return 1;
}