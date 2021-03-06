#include "helper.h"


void main() {
    int *result;
	int i;
    char curdir;
    char argc;
    char argv[4][32];
    int succ;
    char buff[MAX_FILENAME + 1];	
    char dest;
    char mode = 0;
    interrupt(0x21, 0x21, &curdir, 0, 0);
    interrupt(0x21, 0x22, &argc, 0, 0);

	interrupt(0x21, 0x23, 0, argv[0], 0);
    if (argc > 0) {
        interrupt(0x21, (curdir << 8) | 0x08, argv[0], &succ, 0); //
        if (succ == ALREADY_EXISTS) {
            interrupt(0x21, 0x0, "Directory exists\r\n", 0, 0); 
        } else if (succ == NOT_FOUND) {
            interrupt(0x21, 0x0, "Directory not found\r\n", 0, 0); 
        }
    }
    interrupt(0x21, 0x07, result, 0, 0);
}