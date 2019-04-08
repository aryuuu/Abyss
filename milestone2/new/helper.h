#ifndef HELPER_H
#define HELPER_H

#include "helperdec.h"



char indexParentFile(char* path, char parentIndex) {
    char dirs[SECTOR_SIZE]; //untuk menampung sector yang ingin dicek
    char parent; //menampung parent dari path yang sedang dicari
	/** dirs sudah berisi sector **/
    int found = FALSE;
    int idxDirsname = 0;
    int idxCheckDirs = 0;
    int idxDirs = 0;
    parent = parentIndex;

    /* PROSES */
    //Membaca sector
    interrupt(0x21, 0x2, dirs, DIRS_SECTOR, 0); //memanggil readSector
    
    //Mencari direktori yang sesuai dengan path
    do { 
        found = FALSE; //Inisialisasi setiap pindah parent
        do { 
            if (dirs[idxDirsname * DIRS_ENTRY_LENGTH] == parent) { //jika ditemukan parent
                //Pencarian direktori
                found = TRUE;
                for (idxDirsname = 0; idxDirsname <= MAX_FILES && path[idxCheckDirs + idxDirsname] != '/' && path[idxCheckDirs + idxDirsname] != '\0'; ++idxDirsname) { 
                    if (dirs[(idxDirs * DIRS_ENTRY_LENGTH) + idxDirsname + 1] != path[idxCheckDirs + idxDirsname]) { 
                        //nama direktori pada direktori ke-idxDirs tidak sama dengan path yang dicari                               
                        found = FALSE;
                        ++idxDirs;
                        break;
                    } 
                }
            } 
            else ++idxDirs; //kalau dirs nya tidak sama dengan parent
        } while (!found && idxDirs < MAX_ENTRIES);
        
        //Pencarian berakhir 
        //tidak ditemukan direktori
        if (!found) {
            return 0xFE; 
        }
        //Ditemukannya direktori yang sesuai maka, lanjut ke anak nya direktori itu
        idxCheckDirs += idxDirsname + 1;
        parent = idxDirs; //parent di set direktori yang sudah ditemukan
    } while (path[idxCheckDirs - 1] != '\0');
    
    return parent;
}

int indexFile(char* filename, char dir_index) {
	char files[SECTOR_SIZE];
    int idxFile = 0;
    interrupt(0x21, 0x2, files, FILES_SECTOR, 0); // readSector pada file	    	                     
	do {
        //Jika direktori nya sesuai
		if (files[idxFile * FILES_ENTRY_LENGTH] == dir_index) {
			//Cek apakah ada file ada direktori itu
			if(strcmp(files + idxFile * FILES_ENTRY_LENGTH + 1, filename)) break;
		}                  
		++idxFile;
	} while (idxFile < MAX_FILES);

    if (idxFile == MAX_FILES) {
        return -1;
    }
    else {
        return idxFile;
    }
}

int indexEmptySector(char *map) {
  int idx;
  for (idx = 0; idx < MAX_BYTE; ++idx) {
    //map ke-idx kosong
    if (map[idx] == 0x00) {
      return idx;
    }
  }
  //Map tidak ada yang kosong
  return NOT_FOUND;
}

// find whether there is remained entry that hasn't been used or not
int isEntryRemained(char *entry) {
   int found = FALSE;
   int i;
   int idx;
   for (i = 0; i < MAX_ENTRIES; i++) {
      idx = i * ENTRY_LENGTH + NAME_OFFSET;
      if (entry[idx] == '\0') {
         found = TRUE;
      }
   }
   if (found) {
      return i;
   }
   else {
      return -1;
   }
}


int mod(int a, int b) {
   while(a >= b) {
      a = a - b;
   }
   return a;
}

int div(int a, int b) {
   int q = 0;
   while(q*b <= a) {
      q = q+1;
   }
   return q-1;
}

void clear(char *buffer, int length) {
   int i;
   for(i = 0; i < length; ++i) {
      buffer[i] = EMPTY;
   }
}


int strcmp(char* s1, char* s2) {
	int i = 0;
	while (!(s1[i] == '\0' && s2[i] == '\0')) {
		if (s1[i] != s2[i]) return 0;
		++i;
	}
	return 1;
}

void strcpy(char* sc, char* sd) {
	int i = 0;
	clear(sd, 15);
	do {
		sd[i] = sc[i];
		++i;
	} while(sc[i] != '\0');
}

// void printInt(int i) {
// 	char ir = '0' + (char) div(i, 100);
// 	char ip = '0' + (char) div(mod(i, 100), 10);
// 	char is = '0' + (char) mod(i, 10);
// 	interrupt(0x10, 0xE00 + ir, 0, 0, 0);
// 	interrupt(0x10, 0xE00 + ip, 0, 0, 0);
// 	interrupt(0x10, 0xE00 + is, 0, 0, 0);
// }









#endif