#include "helperdec.h"


void handleInterrupt21(int AX, int BX, int CX, int DX); // asm linking purposes
// Utility
void printString(char *string);
void printInt(int i);
void readString(char *string);
// #include "../library/declaration/math_dec.h"
// #include "../library/declaration/strutils_dec.h"
// #include "../library/declaration/fsutils_dec.h"
// void printLogo();
// File System
void readSector(char *buffer, int sector);
void writeSector(char *buffer, int sector);
void readFile(char *buffer, char *path, int *result, char parentIndex);
void writeFile(char *buffer, char *path, int *result, char parentIndex);
void deleteFile(char *path, int *result, char parentIndex); 
void makeDirectory(char *path, int *result, char parentIndex);
void deleteDirectory(char *path, int *success, char parentIndex); 
// Execute a Program
void putArgs (char curdir, char argc, char **argv);
void getCurdir (char *curdir);
void getArgc (char *argc);
void getArgv (char index, char *argv);
void executeProgram(char *path, int segment, int *result, char parentIndex); // segment = 0x2000
void terminateProgram(int* result); 

int main() {      
   int* result;
   char buf[512];
   char arg1[4], arg2[4];
   char* arg[2]; // arg = array of array of char = array of string
   makeInterrupt21();
   interrupt(0x21, 0x0, "hi\r\n", 0, 0);
   makeDirectory("riandi", result, 0xFF);
   makeDirectory("riandi/fikri", result, 0xFF);
   makeDirectory("riandi/fikri/algah", result, 0xFF);
   interrupt(0x21, 0xFF << 8 | 0x06, "shell", 0x2000, *result);
   //terminateProgram(success);
   while(1) {}
}

void handleInterrupt21 (int AX, int BX, int CX, int DX) {
   char AL, AH;
   AL = (char) (AX);
   AH = (char) (AX >> 8);

   switch (AL) {
      case 0x00:
         printString(BX);
         break;
      case 0x01:
         readString(BX);
         break;
      case 0x02:
         readSector(BX, CX);
         break;
      case 0x03:
         writeSector(BX, CX);
         break;
      case 0x04:
         readFile(BX, CX, DX, AH);
         break;
      case 0x05:
         writeFile(BX, CX, DX, AH);
         break;
      case 0x06:
         executeProgram(BX, CX, DX, AH);
         break;
      case 0x07:
         terminateProgram(BX);
         break;
      case 0x08:
         makeDirectory(BX, CX, AH);
         break;
      case 0x09:
         deleteFile(BX, CX, AH);
         break;
      case 0x0A:
         deleteDirectory(BX, CX, AH);
         break;
      case 0x20:
         putArgs(BX, CX, DX);
         break;
      case 0x21:
         getCurdir(BX);
         break;
      case 0x22:
         getArgc(BX);
         break;
      case 0X23:
         getArgv(BX, CX);
         break;
      default:
         printString("Invalid interrupt");
   }
}


//=======================================================================================
// Utility
//=======================================================================================

void printString(char *string) { // Works like println
   int i = 0;
   while (string[i] != '\0') interrupt(0x10, 0xE00 + string[i++], 0, 0, 0);
   // interrupt(0x10, 0xE00 + '\r', 0, 0, 0);       
   // interrupt(0x10, 0xE00 + '\n', 0, 0, 0);
}

void printInt(int i) {
   char ir = '0' + (char) div(i, 100);
   char ip = '0' + (char) div(mod(i, 100), 10);
   char is = '0' + (char) mod(i, 10);
   interrupt(0x10, 0xE00 + ir, 0, 0, 0);
   interrupt(0x10, 0xE00 + ip, 0, 0, 0);
   interrupt(0x10, 0xE00 + is, 0, 0, 0);
   interrupt(0x10, 0xE00 + '\r', 0, 0, 0);       
   interrupt(0x10, 0xE00 + '\n', 0, 0, 0);
}

void readString(char *string) {
   int index = 0;
   char input_buffer = 0x00; // To remember last input
   do {
      input_buffer = interrupt(0x16, 0, 0, 0, 0); // Read a character from the keyboard
      if (input_buffer == '\r') { // If it's an ENTER, terminate it with a NULL
         interrupt(0x10, 0xE00 + input_buffer, 0, 0, 0); // Print it
         string[index] = '\0'; 
      } else if (input_buffer != '\b') { // If it's not a backspace, input it
         interrupt(0x10, 0xE00 + input_buffer, 0, 0, 0); // Print it
         string[index++] = input_buffer; 
      } else if (index > 0) { // If it is backspace, space over the last character
         --index;
         interrupt(0x10, 0xE00 + input_buffer, 0, 0, 0); // Print it
         interrupt(0x10, 0xE00 + 0x20, 0, 0, 0);
         interrupt(0x10, 0xE00 + '\b', 0, 0, 0);
      }
   } while (input_buffer != '\r'); // Wait for ENTER 
   interrupt(0x10, 0xE00 + '\n', 0, 0, 0);
}
#include "helper.h"
// #include "../library/math.h"
// #include "../library/strutils.h"
// #include "../library/fsutils.h"

// void printCenter(int row, int ln, char* s){
//     int i = 0;
//     int offset = 0x8000 + ((row-1)*80*2) + (40-ln/2)*2;
//     while(s[i] != '\0'){
//          putInMemory(0xB000, offset + i*2, s[i]);
//          i++;
//     }
// }

// void printLogo(){
//    int i = 0;
   
//    // clear the screen
//    while (i < 4000){
//       putInMemory(0xB000, 0x8000 + i*2, '\0');
//       i++;
//    }

//    // print logo in center
//    printCenter(7, 20, "========        ");
//    printCenter(8, 20, "==              ");
//    printCenter(9, 20, "==   ===        ");
//    printCenter(10, 20,"==    ==        ");
//    printCenter(11, 20,"========        ");
//    printCenter(12, 20,"  / \\   ==   ==");
//    printCenter(13, 20," /   \\  ==   ==");
//    printCenter(14, 20,"/ 0 0 \\ =======");
//    printCenter(15, 20,"\\  .  / =======");
//    printCenter(16, 20," \\ v /  ==   ==");
//    printCenter(17, 20,"  \\ /   ==   ==");
//    printCenter(18, 20," Ghetto ==   ==");
//    printString("\n\n\nPress any key to continue...");
//    interrupt(0x16,0,0,0,0);

// }


//=======================================================================================
// File System
//=======================================================================================

void readSector(char *buffer, int sector) {
   interrupt(0x13, 0x201, buffer, div(sector, 36) * 0x100 + mod(sector, 18) + 1, mod(div(sector, 18), 2) * 0x100);
}

void writeSector(char *buffer, int sector) {
   interrupt(0x13, 0x301, buffer, div(sector, 36) * 0x100 + mod(sector, 18) + 1, mod(div(sector, 18), 2) * 0x100);
}

void readFile(char *buffer, char *path, int *result, char parentIndex) {
   char sectors[SECTOR_SIZE];
   int dirs_offset = 0, dirsname_offset = 0, filename_idx = 0, files_offset = 0, sectors_offset = 0;
   // Find the index of first character of the filename, to determine when to search for the filename instead of dirsname
   while (path[dirsname_offset] != '\0') {   
      if (path[dirsname_offset] == '/') filename_idx = dirsname_offset + 1;
      ++dirsname_offset;
   }
   dirsname_offset = 0;
   // Search for path
   if (filename_idx != 0) {
      path[filename_idx - 1] = '\0'; // cut the slash
      dirs_offset = indexParentFile(path, parentIndex);
      if (dirs_offset == 0xFE) {*result = -1; return;}
   } else dirs_offset = parentIndex;
   // Search for the file in the path
   files_offset = indexFile(path + filename_idx, dirs_offset);
   if (files_offset == MAX_FILES) {*result = -2; return;}   
   // Read the file from its sectors
   readSector(sectors, SECTORS_SECTOR);
   while(sectors[files_offset * SECTORS_ENTRY_LENGTH + sectors_offset] != '\0' && sectors_offset < SECTORS_ENTRY_LENGTH) {
      readSector(buffer + sectors_offset * SECTOR_SIZE, sectors[files_offset * SECTORS_ENTRY_LENGTH + sectors_offset]);
      ++sectors_offset;
   } 
   *result = 0;
}

void writeFile(char *buffer, char *path, int *result, char parentIndex) {
   char map[SECTOR_SIZE], files[SECTOR_SIZE], sectors[SECTOR_SIZE];
   int map_offset = 0;
   int dirs_offset = 0, dirsname_offset = 0;
   int files_offset = 0, filesname_offset = 0, empty_files_index = 0;
   int sectors_offset = 0, empty_sector = 0;
   int filename_idx = 0;
   // Search for empty sector
   readSector(map, MAP_SECTOR);
   map_offset = indexEmptySector(map);
   // If there's no empty sectors
   if (map_offset == NOT_FOUND) {
      *result = 0x00;
      return;
   }
   // Search for empty file entry
   readSector(files, FILES_SECTOR);
   for (empty_files_index = 0; empty_files_index < MAX_FILES; ++empty_files_index) {
      if (files[empty_files_index * DIRS_ENTRY_LENGTH + 1] == '\0') break;
   }
   // If there's no empty entries
   if (files_offset == MAX_FILES) {
      *result = -3;
      return;
   }
   // Find the index of first character of the filename, to determine when to search for the filename instead of dirsname
   while (path[dirsname_offset] != '\0') {   
      if (path[dirsname_offset] == '/') filename_idx = dirsname_offset + 1;
      ++dirsname_offset;
   }
   dirsname_offset = 0;
   // Search for path
   if (filename_idx != 0) { 
      path[filename_idx - 1] = '\0';      
      dirs_offset = indexParentFile(path, parentIndex);
      if (dirs_offset == 0xFE) {*result = -1; return;}
   } else dirs_offset = parentIndex;
   // Search whether the file exists or not
   files_offset = indexFile(path + filename_idx, dirs_offset);
   if (files_offset != MAX_FILES) {*result = -2; return;}
   // Write to file entry
   files[empty_files_index * FILES_ENTRY_LENGTH] = (char) dirs_offset;
   dirsname_offset = 0;
   while (path[filename_idx + dirsname_offset] != '\0') {
      files[empty_files_index * FILES_ENTRY_LENGTH + dirsname_offset + 1] = path[filename_idx + dirsname_offset]; 
      ++dirsname_offset;
   }
   // Write buffer
   readSector(sectors, SECTORS_SECTOR);   
   while (buffer[sectors_offset * SECTOR_SIZE] != '\0') {
      empty_sector = indexEmptySector(map);
      writeSector(buffer + sectors_offset * SECTOR_SIZE, empty_sector);
      sectors[empty_files_index * SECTORS_ENTRY_LENGTH + sectors_offset] = empty_sector;
      map[empty_sector] = 0xFF;
      ++sectors_offset;
   }
   // Write to floppy
   writeSector(map, MAP_SECTOR);
   writeSector(files, FILES_SECTOR);
   writeSector(sectors, SECTORS_SECTOR);
   *result = 0;
}

void deleteFile(char *path, int *result, char parentIndex) {
   char map[SECTOR_SIZE], files[SECTOR_SIZE], sectors[SECTOR_SIZE];
   int dirs_offset = 0, dirsname_offset = 0, filename_idx = 0;
   int files_offset = 0;
   int sectors_offset = 0;
   
   // Find the index of the last slash, to determine when to search for the filename instead of dirsname
   while (path[dirsname_offset] != '\0') {   
      if (path[dirsname_offset] == '/') filename_idx = dirsname_offset + 1;
      ++dirsname_offset;
   }
   // Search for path
   if (filename_idx != 0) {
      path[filename_idx - 1] = '\0';
      dirs_offset = indexParentFile(path, parentIndex);
      if (dirs_offset == 0xFE) {*result = -1; return;}
   } else dirs_offset = parentIndex;
   // Search for the file
   files_offset = indexFile(path + filename_idx, dirs_offset);
   if (files_offset == MAX_FILES) {*result = -2; return;}
   // Delete in map
   readSector(map, MAP_SECTOR);
   readSector(sectors, SECTORS_SECTOR);
   sectors_offset = 0;
   while (sectors[files_offset * SECTORS_ENTRY_LENGTH + sectors_offset] != '\0') {
      map[sectors[files_offset * SECTORS_ENTRY_LENGTH + sectors_offset]] = 0x00;
      // clear sectors?
      ++sectors_offset;
   }
   // Delete file entry
   readSector(files, FILES_SECTOR);
   files[files_offset * FILES_ENTRY_LENGTH + 1] = '\0';
   // Write buffer
   writeSector(map, MAP_SECTOR);
   writeSector(files, FILES_SECTOR);
   writeSector(sectors, SECTORS_SECTOR);
   *result = 0;
}


void makeDirectory(char *path, int *result, char parentIndex) {
   char dirs[SECTOR_SIZE], sectors[SECTOR_SIZE];
   int i;
   int dirsOffset = 0, dirsNameOffset = 0, target_dir_idx = 0;
   int unusedEntry, toBeCreatedOffset = 0;
   readSector(dirs,DIRS_SECTOR);
   // Check empty entries in dir
   unusedEntry = isEntryRemained(dirs);
   if (unusedEntry == NOT_FOUND){ // No empty location
      *result = -3;
      return;
   }
   // Find the index of last slash ,to gain directory name that want to be created
   while(path[dirsNameOffset] != '\0') {
      if(path[dirsNameOffset] == '/') {
         target_dir_idx = dirsNameOffset + 1;
      }
      dirsNameOffset += 1;
   }
   // Search for parent path
   if (target_dir_idx != 0) {
      path[target_dir_idx - 1] = '\0';
      dirsOffset = indexParentFile(path, parentIndex);
      if (dirsOffset == 0xFE) {*result = -1; return;}
   } else dirsOffset = parentIndex;
   // Check the availability of path name
   toBeCreatedOffset = indexParentFile(path + target_dir_idx, dirsOffset);
   if (toBeCreatedOffset != 0xFE) {*result = -2; return;}
   
   // No dirs yet, write directory
   dirs[unusedEntry * DIRS_ENTRY_LENGTH] = dirsOffset;
   i = 1;
   for(dirsNameOffset = target_dir_idx; path[dirsNameOffset] != '\0'; ++dirsNameOffset) {
      dirs[unusedEntry * DIRS_ENTRY_LENGTH + i] = path[dirsNameOffset];
      i++;
   }
   writeSector(dirs,DIRS_SECTOR);
   *result = 0;   
}

void deleteDirectory(char *path, int *success, char parentIndex) {
   char dirs[SECTOR_SIZE], files[SECTOR_SIZE];
   int dirdelOffset = 0;
   int dirsOffset = 0;
   int files_offset = 0;
   char* delresult;
   readSector(dirs,DIRS_SECTOR);
   // Search for path
   dirsOffset = indexParentFile(path, parentIndex);
   if (dirsOffset == 0xFE) {*success = -1; return;}
   // Delete the file inside the dir, if any
   readSector(files, FILES_SECTOR);
   for(files_offset = 0; files_offset < MAX_FILES; ++files_offset) {    
      if (files[files_offset * FILES_ENTRY_LENGTH] == dirsOffset && files[files_offset * FILES_ENTRY_LENGTH + 1] != '\0') {
         deleteFile(files + files_offset * FILES_ENTRY_LENGTH + 1, delresult, dirsOffset);
      }
   }
   // Delete the dir inside the dir, if any
   for(dirdelOffset = 0; dirdelOffset < MAX_FILES; ++dirdelOffset) {
      if(dirs[dirdelOffset * DIRS_ENTRY_LENGTH] == dirsOffset && dirs[dirdelOffset * DIRS_ENTRY_LENGTH + 1] != '\0') {
         deleteDirectory(dirs + dirdelOffset * DIRS_ENTRY_LENGTH + 1, delresult, dirsOffset);
         readSector(dirs,DIRS_SECTOR);
      }
   }
   // Delete the dir
   dirs[dirsOffset * DIRS_ENTRY_LENGTH + 1] = '\0';
   writeSector(dirs, DIRS_SECTOR);  
   *success = 0;
}


//=======================================================================================
// Program execution
//=======================================================================================

void putArgs (char curdir, char argc, char **argv) {
   char args[SECTOR_SIZE];
   int i, j, p;
   clear(args, SECTOR_SIZE);
   args[0] = curdir;
   args[1] = argc;
   i = 0;
   j = 0;
   for (p = 1; p < ARGS_SECTOR && i < argc; ++p) {
      args[p] = argv[i][j];
      if (argv[i][j] == '\0') {
         ++i;
         j = 0;
      } else ++j;
   }
   writeSector(args, ARGS_SECTOR);
}

void getCurdir (char *curdir) {
   char args[SECTOR_SIZE];
   readSector(args, ARGS_SECTOR);
   *curdir = args[0];
}

void getArgc (char *argc) {
   char args[SECTOR_SIZE];
   readSector(args, ARGS_SECTOR);
   *argc = args[1];
}

void getArgv (char index, char *argv) {
   char args[SECTOR_SIZE];
   int i, j, p;
   readSector(args, ARGS_SECTOR);
   i = 0;
   j = 0;
   for (p = 1; p < ARGS_SECTOR; ++p) {
      if (i == index) {
         argv[j] = args[p];
         ++j;
      }
      if (args[p] == '\0') {
         if (i == index) break; else ++i;
      }
   }
}

void executeProgram(char *path, int segment, int *result, char parentIndex) {
   char buffer[MAX_SECTORS * SECTOR_SIZE];   
   int i;            
   readFile(buffer, path, result, parentIndex); 
   if (*result) return; 
   for (i = 0; i < MAX_SECTORS * SECTOR_SIZE; ++i) putInMemory(segment, i, buffer[i]);       
   launchProgram(segment); 
}

void terminateProgram (int *result) {
   int a;
   char shell[6];
   shell[0] = 's';
   shell[1] = 'h';
   shell[2] = 'e';
   shell[3] = 'l';
   shell[4] = 'l';
   shell[5] = '\0';  
   executeProgram(shell, 0x2000, &a, 0xFF);
}