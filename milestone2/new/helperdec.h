#ifndef HELPERDEC_H
#define HELPERDEC_H

#define EMPTY 0x00
#define USED 0xFF
#define TRUE 1
#define FALSE 0
#define SUCCESS 0
#define NOT_FOUND -1
#define ALREADY_EXISTS -2
#define INSUFFICIENT_MEMORY -3
// File System Constants
#define SECTOR_SIZE 512
#define MAP_SECTOR 256
#define DIRS_SECTOR 257
#define FILES_SECTOR 258
#define SECTORS_SECTOR 259
#define ARGS_SECTOR 512
#define DIRS_ENTRY_LENGTH 16
#define FILES_ENTRY_LENGTH 16
#define SECTORS_ENTRY_LENGTH 16
#define MAX_DIRS 32
#define MAX_FILES 32
#define MAX_SECTORS 32
// Yg bawah ini harusnya udah gak kepake
#define NAME_OFFSET 1
#define ENTRY_LENGTH 16
#define MAX_ENTRIES 32
#define MAX_FILENAME 15
#define MAX_BYTE 256
#define INSUFFICIENT_SECTORS 0
#define INSUFFICIENT_SEGMENTS -1
#define INSUFFICIENT_DIR_ENTRIES -1

char indexParentFile(char* path, char parentIndex);
int indexFile(char* filename, char dir_index);
int indexEmptySector(char *map);
int isEntryRemained(char *entries);
int mod(int a, int b);
int div(int a, int b);
int strcmp(char* s1, char* s2); 
void strcpy(char* sc, char* sd);
void clear(char *buffer, int length);


#endif