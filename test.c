#include <stdio.h>
 
void foo ( char *line ) {
    int num, i = 0, len;
    while ( sscanf( line, "%d%n", &num, &len) == 1 ) {
        printf( "Number %d is %d\n", i, num );
        line += len;    // step past the number we found
        i++;            // increment our count of the number of values found
    }
}
int main ( ) {
    char test[] = "12 45 6 23 100";
    foo( test );
    return 0;
}