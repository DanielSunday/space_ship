#include "engine.h"
#include <limits.h>
#include <ctype.h>

#define min(a, b) ((a < b) ? a : b) /* Returns the smaller argument */

static int str2uint(const char *str) /* On failure returns a negative integer */
{
    int uint = 0, i = 0, digit;
    while (isspace(str[i])) ++i; /* Skip  leading whitespace */
    while (isdigit(str[i]))
    {
        if (INT_MAX / 10 < uint) return -1; /* Product overflow safeguard */
        uint *= 10; /* Shifting left by one decimal place */
        digit = str[i] - '0'; /* Standard guarantees that 0-9 are sequential */
        if (INT_MAX - digit < uint) return -2; /*  Sum overflow safeguard */
        uint += digit; /* Filling the right-most decimal place */
        ++i;
    }
    while (isspace(str[i])) ++i; /* Skip trailing whitespace */
    if (str[i] != '\0') return -3; /* Invalid string format */
    return uint;
}

int main(int argc, char **argv) /*Get inclusive level index range;start engine*/
{
    int first = -1, last = -1, i;
    if (argc > 1) first = str2uint(argv[1]);
    if (argc > 2)  last = str2uint(argv[2]);
    if (first < 0) first = 0; /* Default value of first level index */
    if ( last < 0)  last = INT_MAX - 1; /* Default value of last level index */
    init_engine();
    last = min(last, get_engine_no_levels() - 1);
    for (i = first; i <= last; ++i) run_engine(i);
    exit_engine();
    return 0;
}
