#include "helper.h"

void handleInterrupt21 (int AX, int BX, int CX, int DX);

//file system
void readSector(char *buffer, int sector);
void writeSector(char *buffer, int sector);
void readFile(char *buffer, char *path, int *result, char parentIndex);
void writeFile(char *buffer, char *path, int *result, char parentIndex);
void makeDirectory(char *path, int *result, char parentIndex);
void deleteFile(char *path, int *result, char parentIndex);
void deleteDirectory(char *path, int *success, char parentIndex);

//string
void printString(char *string);
void readString(char *string);

//sh
void executeProgram(char *filename, int segment, int *success);
void terminateProgram (int *result);
void putArgs (char curdir, char argc, char **argv);
void getArgv (char index, char *argv);
void getArgc (char *argc);
void getCurdir (char *curdir);



int main() {
 
   // int 

   makeInterrupt21();
   printString("hi");
   interrupt(0x21, 0xFF << 8 | 0x6, "shell", 0x2000, &success);
   // interrupt()
   // interrupt(0x21,0x0,"bite my shiny metal axe",0,0);
   // interrupt(0x21, 0x0,"program name to run", 0, 0);
   // interrupt(0x21, 0x1, input, 0, 0);
   // interrupt(0x21, 0x0, input, 0, 0);
   // interrupt(0x21, 0x6, input, 0x2000, &s1);//executeProgram
   // interrupt(0x21, 0x4, key, "key.txt", &s2); 
   // interrupt(0x21, 0x0, key, 0, 0);
   // interrupt(0x21, 0x4, fff, "key.txt", &suc);
   // printString(fff);
   // printString("\n\0");
   // interrupt(0x21, 0x6, "keyproc", 0x2000, &suc);

   // if (suc) {
   //    printString("berhasil\n\0");
   // } else {
   //    printString("gagal\n\0");
   // }


   while (1);
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
         putArgs(BX, CX);
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


void printString(char *string){
   char c;
   int i;

   i = 0;
   c = string[i];
   while(c != '\0'){
      interrupt(0x10, 0xE00 + c, 0, 0, 0);
      i = i + 1;
      c = string[i];
   }

   // interrupt(0x10, 0xE00 + '\n', 0, 0, 0);
   // interrupt(0x10, 0xE00 + '\r', 0, 0, 0);

}

void readString(char *string){
	char c;
	int i;

	i = 0;
	c = interrupt(0x16, 0, 0, 0, 0);
	while(c != '\r'){
      if(c == '\b'){ //backspace handling
         if(i > 0){
            interrupt(0x10, 0xE00 + '\b', 0, 0, 0);//move cursor back one char
            interrupt(0x10, 0xE00 + '\0', 0, 0, 0);//delete char where the cursor is pointing at
            
            interrupt(0x10, 0xE00 + '\b', 0, 0, 0);//move cursor back one char
            
            i = i - 1;//go back one index
         }
         c = interrupt(0x16, 0, 0, 0, 0);//read one new char

      } else {
         interrupt(0x10, 0xE00 + c, 0, 0, 0);
         string[i] = c;//set char at string
         i = i + 1;//go forward one index
         c = interrupt(0x16, 0, 0, 0, 0);//read one new char
      }
		
	}
	string[i] = '\0';
	interrupt(0x10, 0xE00 + '\n', 0, 0, 0);
   interrupt(0x10, 0xE00 + '\r', 0, 0, 0);
}



void writeFile(char *buffer, char *path, int *result, char parentIndex) {
   char map[SECTOR_SIZE], files[SECTOR_SIZE], sectors[SECTOR_SIZE];
   int idxMap;
    int idxEmptyFile;
    int idxDirsname;
    int idxFilename;
    int idxDirs;
    int idxFile;
    int idxSector;
    int idxEmptySector;
   
    //Mencari sector map yang kosong
   readSector(map, MAP_SECTOR); //Membaca sector map
    /** map sudah berisi sector map **/
   idxMap = indexEmptySector(map);
   // jika Map tidak ada yang dapat ditempati
   if (idxMap == NOT_FOUND) {
      *result = INSUFFICIENT_MEMORY;
      return;
   }

   // Mencari sektor file yang kosong
   readSector(files, FILES_SECTOR);
    /** files sudah berisi sector files **/
   for (idxEmptyFile = 0; idxEmptyFile < MAX_FILES; ++idxEmptyFile) {
      if (files[idxEmptyFile * DIRS_ENTRY_LENGTH + 1] == '\0') 
            break;
   }
   // jika Files tidak ada yang kosong
   if (idxEmptyFile == MAX_FILES) {
      *result = INSUFFICIENT_MEMORY;
      return;
   }

   // Mencari direktori yang dicari pada path diatas
   while (path[idxDirsname] != '\0') { 
      if (path[idxDirsname] == '/') {
            idxFilename = idxDirsname + 1;
        }
      ++idxDirsname;
   }
    /** idxFilename berisi idx direktori terakhir sebelum file **/

   idxDirsname = 0;
   //Mencari file
   if (idxFilename != 0) { 
      path[idxFilename - 1] = '\0';    
      idxDirs = indexParentFile(path, parentIndex);
      if (idxDirs == 0xFE) {
            *result = NOT_FOUND; 
            return;
        }
   } 
    else {
        idxDirs = parentIndex;
    }
    /** idxDirs menampung indeks direktori pada sektor direktori yang ditemukan **/

   // Mencari apakah file yang ingin dibuat sudah ada atau tidak pada direktori ke-idxDirs
   idxFile = indexFile(path + idxFilename, idxDirs);
    /** idxFile berisikan index ditemukan atau tidaknya file **/

    //Jika file ada pada sektor
   if (idxFile != MAX_FILES) {
        *result = ALREADY_EXISTS; 
        return;
    }

   // Menulis entry file
   files[idxEmptyFile * FILES_ENTRY_LENGTH] = (char) idxDirs;
   idxDirsname = 0;
   while (path[idxFilename + idxDirsname] != '\0') {
      files[idxEmptyFile * FILES_ENTRY_LENGTH + idxDirsname + 1] = path[idxFilename + idxDirsname];   
      ++idxDirsname;
   }

   // Menulis sektor dalam file
   readSector(sectors, SECTORS_SECTOR);   
    /** sectors berisikan sektor-sektor **/
   while (buffer[idxSector * SECTOR_SIZE] != '\0') {
      idxEmptySector = indexEmptySector(map);
      writeSector(buffer + idxSector * SECTOR_SIZE, idxEmptySector);
      sectors[idxEmptyFile * SECTORS_ENTRY_LENGTH + idxSector] = idxEmptySector;
      map[idxEmptySector] = 0xFF;
      ++idxSector;
   }

   // Menulis ke floppy dengan map, files, dan sector yang sudah ditulis
   writeSector(map, MAP_SECTOR);
   writeSector(files, FILES_SECTOR);
   writeSector(sectors, SECTORS_SECTOR);
   *result = SUCCESS;
}




//buffer berisi isi file yang dibaca dari path yang diminta (jika ditemukan)
//path merupakan path menuju file yang diinginkan
//result bernilai -1 jika file tidak ditemukan 
//parentIndex merupakan index dari parent current directory
//parent index akan bernilai 0xff ketika awal pemanggilan (current directory =root)
void readFile(char *buffer, char *path, int *result, char parentIndex) {
   char sectors[SECTOR_SIZE]; //Untuk menampung sector
   int idxDirs = 0; 
   int idxDirsname = 0; 
   int idxFilename = 0; 
   int idxFile = 0; 
   int idxSector = 0;

    //Mencari idx filename pada path
   while (path[idxDirsname] != '\0') { 
      if (path[idxDirsname] == '/') {
            idxFilename = idxDirsname + 1;
        }
      ++idxDirsname;
   }
    
   idxDirsname = 0;
   //Mencari path pada file system
   if (idxFilename != 0) { //Jika file yang dicari berada dalam direktori
      path[idxFilename - 1] = '\0'; // potong direktori terakhir dari file
      idxDirs = indexParentFile(path, parentIndex);

      if (idxDirs == 0xFE) {
            *result = NOT_FOUND; 
            return;
        }
   } 
    else idxDirs = parentIndex; //File yang dicari ada di parentIndex alias ada di root
   
    // Mencari file yang ada di drektori yang sudah di temukan
   idxFile = indexFile(path + idxFilename, idxDirs);

    //Tidak ada file yang dicari
   if (idxFile == MAX_FILES) {
        *result = NOT_FOUND; 
        return;
    }

    //File ditemukan lalu dibaca
   readSector(sectors, SECTORS_SECTOR); //Membaca sector
   while (sectors[idxFile * SECTORS_ENTRY_LENGTH + idxSector] != '\0' && idxSector < SECTORS_ENTRY_LENGTH) {
      readSector(buffer + idxSector * SECTOR_SIZE, sectors[idxFile * SECTORS_ENTRY_LENGTH + idxSector]);
      ++idxSector;
   } 
   *result = idxFile;
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
      if (dirs_offset == 0xFE) {*result = NOT_FOUND; return;}
   } else dirs_offset = parentIndex;
   // Search for the file
   files_offset = indexFile(path + filename_idx, dirs_offset);
   if (files_offset == MAX_FILES) {*result = NOT_FOUND; return;}
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
   *result = SUCCESS;
}

void makeDirectory(char *path, int *result, char parentIndex) {
   char dirs[SECTOR_SIZE];
   int remainedEntryFound = 0;
   int idxPath = 0;
   int idxNewDir = 0;
   int idxParentDir = 0;
   int idxNewParentDir = 0;
   int i = 0;

   readSector(dirs, DIRS_SECTOR);
   // check whether there is remained entry in dirs or not
   remainedEntryFound = isEntryRemained(dirs);
   // if remained entry not exists
   if (remainedEntryFound == -1) {
      *result = -3; // INSUFFICIENT_ENTRIES
      return;
   }
   // if remained entry exists
   // define new directory index in path
   while (path[idxPath] != '\0') {
      if (path[idxPath] == '/') {
         idxPath = idxNewDir + 1;
      }
      idxPath++;
   }  
   // define parent index
   // if path is in the root
   if (idxNewDir == 0) {
      idxParentDir = parentIndex;
   }        
   else { // not root
      path[idxNewDir - 1] = '\0';
      idxParentDir = indexParentFile(path, parentIndex);
      // parent directory index not found
      if (idxParentDir == 0xFE) {
         *result == -1; // NOT_FOUND
         return;
      }
   }
   // new parent directory
   idxNewParentDir = indexParentFile(path[idxNewDir], idxParentDir);
   if (idxNewParentDir != 0xFE) {
      *result = -2; // ALREADY_EXISTS
      return;
   }

   // write index of previous parent dir into first empty entry in dirs
   dirs[remainedEntryFound * ENTRY_LENGTH] = idxParentDir;
   for (idxPath = idxNewDir; path[idxPath] != '\0'; idxPath++) {
      dirs[remainedEntryFound * ENTRY_LENGTH + i] = path[idxPath];
      i++;
   }
   // write dirs into sector
   writeSector(dirs, DIRS_SECTOR);
   *result = 0; // SUCCESS
}


void deleteDirectory(char *path, int *success, char parentIndex) {
   char dirs[SECTOR_SIZE];
   int idxDir = 0;
   char files[SECTOR_SIZE];
   int idxFile = 0;
   int idxDirTemp = 0;
   char* deletedFile;

   readSector(dirs, DIRS_SECTOR);
   readSector(files, FILES_SECTOR);
   // get first index parent file
   idxDir = indexParentFile(path, parentIndex);
   if (idxDir == 0xFE) { // parent directory index not found
      *success = -1; // NOT FOUND
      return;
   }
   // parent directory index found
   // delete all files in the dir
   for (idxFile = 0; idxFile < MAX_FILES; idxFile++) {      
      if ((files[idxFile*ENTRY_LENGTH+1] != '\0') && (files[idxFile*ENTRY_LENGTH] == idxDir)) {
         deleteFile(files[idxFile*ENTRY_LENGTH+1], deletedFile, idxDir);
      }
   }
   // delete all dirs
   for (idxDirTemp = 0; idxDirTemp < MAX_FILES; idxDirTemp++) {
      if ((dirs[idxDirTemp * ENTRY_LENGTH + 1] != '\0') && (dirs[idxDirTemp * ENTRY_LENGTH] == idxDir)) {
         deleteDirectory(dirs[idxDirTemp*ENTRY_LENGTH+1], deletedFile, idxDir);
         readSector(dirs, DIRS_SECTOR);
      }
   }
   // delete dir
   dirs[idxDir*ENTRY_LENGTH+1] = '\0';
   // write dirs into sector
   writeSector(dirs, DIRS_SECTOR);  
   *success = 0; // SUCCESS
}




void executeProgram(char *filename, int segment, int *success){
   char buffer[MAX_SECTORS * SECTOR_SIZE];//alokasi buffer
   
   int i;
   //cari program dengan nama filename
   //kemudian load isinya ke buffer jika ditemukan
   readFile(buffer, filename, success);

   
   //jika file program ditemukan
   if(*success == TRUE){
      for(i = 0; i < (MAX_SECTORS * SECTOR_SIZE); i = i + 1){
         putInMemory(segment, i, buffer[i]);
      }
      
      launchProgram(segment);
   }
}



void readSector(char *buffer, int sector){
   interrupt(0x13, 0x201, buffer, div(sector, 36) * 0x100 + mod(sector, 18) + 1, mod(div(sector, 18), 2) * 0x100);
}

void writeSector(char *buffer, int sector){
   interrupt(0x13, 0x301, buffer, div(sector, 36) * 0x100 + mod(sector, 18) + 1, mod(div(sector, 18), 2) * 0x100);
}




void terminateProgram (int *result) {
   char shell[6];
   shell[0] = 's';
   shell[1] = 'h';
   shell[2] = 'e';
   shell[3] = 'l';
   shell[4] = 'l';
   shell[5] = '\0';
   executeProgram(shell, 0x2000, result, 0xFF);
}


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
      }
      else {
         ++j;
      }
   }

   writeSector(args, ARGS_SECTOR);
}

//curdir akan diisi dengan nama direktori sekarang
void getCurdir (char *curdir) {
   char args[SECTOR_SIZE];
   readSector(args, ARGS_SECTOR);
   *curdir = args[0];
}

//
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
         if (i == index) {
            break;
         }
         else {
         ++i;
         }
      }
   }
}

