#define MAX_BYTE 256
#define SECTOR_SIZE 512
#define MAX_FILES 16
#define MAX_FILENAME 12
#define MAX_SECTORS 20
#define DIR_ENTRY_LENGTH 32
#define MAP_SECTOR 1
#define DIR_SECTOR 2
#define TRUE 1
#define FALSE 0
#define INSUFFICIENT_SECTORS 0
#define NOT_FOUND -1
#define INSUFFICIENT_DIR_ENTRIES -1
#define EMPTY 0x00
#define USED 0xFF

void handleInterrupt21 (int AX, int BX, int CX, int DX);
void printString(char *string);
void readString(char *string);
int mod(int a, int b);
int div(int a, int b);
void readSector(char *buffer, int sector);
void writeSector(char *buffer, int sector);
void readFile(char *buffer, char *filename, int *success);
void clear(char *buffer, int length);
void writeFile(char *buffer, char *filename, int *sectors);
void executeProgram(char *filename, int segment, int *success);

char fff[SECTOR_SIZE * MAX_SECTORS];

int main() {
   char * input;
   char * key;
   int suc;
   
   //os logo
   // putInMemory(0xB000, 0x8000, );



   makeInterrupt21();

   // interrupt()
   // interrupt(0x21,0x0,"bite my shiny metal axe",0,0);
   // interrupt(0x21, 0x0,"program name to run", 0, 0);
   // interrupt(0x21, 0x1, input, 0, 0);
   // interrupt(0x21, 0x0, input, 0, 0);
   // interrupt(0x21, 0x6, input, 0x2000, &s1);//executeProgram
   // interrupt(0x21, 0x4, key, "key.txt", &s2); 
   // interrupt(0x21, 0x0, key, 0, 0);
   interrupt(0x21, 0x4, fff, "key.txt", &suc);
   printString(fff);
   // printString("\n\0");
   // interrupt(0x21, 0x6, "keyproc", 0x2000, &suc);

   // if (suc) {
   //    printString("berhasil\n\0");
   // } else {
   //    printString("gagal\n\0");
   // }


   while (1);
}

void handleInterrupt21 (int AX, int BX, int CX, int DX){
   switch (AX) {
      case 0x0:
         printString(BX);
         break;
      case 0x1:
         readString(BX);
         break;
      case 0x2:
         readSector(BX, CX);
         break;
      case 0x3:
         writeSector(BX, CX);
         break;
      case 0x4:
         readFile(BX, CX, DX);
         break;
      case 0x5:
         writeFile(BX, CX, DX);
         break;
      case 0x6:
         executeProgram(BX, CX, DX);
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

   interrupt(0x10, 0xE00 + '\n', 0, 0, 0);
   interrupt(0x10, 0xE00 + '\r', 0, 0, 0);

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

void writeFile(char *buffer, char *filename, int *sectors){
   char map[SECTOR_SIZE];
   char dir[SECTOR_SIZE];
   char sectorBuffer[SECTOR_SIZE];
   int dirIndex;

   readSector(map, MAP_SECTOR);
   readSector(dir, DIR_SECTOR);

   for (dirIndex = 0; dirIndex < MAX_FILES; ++dirIndex) {
      if (dir[dirIndex * DIR_ENTRY_LENGTH] == '\0') {
         break;
      }
   }

   if (dirIndex < MAX_FILES) {
      int i, j, sectorCount;
      for (i = 0, sectorCount = 0; i < MAX_BYTE && sectorCount < *sectors; ++i) {
         if (map[i] == EMPTY) {
            ++sectorCount;
         }
      }

      if (sectorCount < *sectors) {
         *sectors = INSUFFICIENT_SECTORS;
         return;
      } else {
         clear(dir + dirIndex * DIR_ENTRY_LENGTH, DIR_ENTRY_LENGTH);
         for (i = 0; i < MAX_FILENAME; ++i) {
            if (filename[i] != '\0') {
               dir[dirIndex * DIR_ENTRY_LENGTH + i] = filename[i];
            } else {
               break;
            }
         }

         for (i = 0, sectorCount = 0; i < MAX_BYTE && sectorCount < *sectors; ++i) {
            if (map[i] == EMPTY) {
               map[i] = USED;
               dir[dirIndex * DIR_ENTRY_LENGTH + MAX_FILENAME + sectorCount] = i;
               clear(sectorBuffer, SECTOR_SIZE);
               for (j = 0; j < SECTOR_SIZE; ++j) {
                  sectorBuffer[j] = buffer[sectorCount * SECTOR_SIZE + j];
               }
               writeSector(sectorBuffer, i);
               ++sectorCount;
            }
         }
      }
   } else { 
      *sectors = INSUFFICIENT_DIR_ENTRIES;
      return;
   }

   writeSector(map, MAP_SECTOR);
   writeSector(dir, DIR_SECTOR);

}

void readFile(char *buffer, char *filename, int *success){
   char dir[SECTOR_SIZE];  
   int dirIndex;
   char found; 
   int sectIndex;
   
   clear(dir, SECTOR_SIZE);
   readSector(dir, DIR_SECTOR);

   for (dirIndex = 0; dirIndex < SECTOR_SIZE; dirIndex  += DIR_ENTRY_LENGTH) {
      int i, count = 0;
      found = TRUE;
      for (i=0; i<MAX_FILENAME; ++i) {
         if (filename[i] == '\0') {
            break;
         } else if (dir[dirIndex+i] != filename[i]) {
            found = FALSE;
            break;
         }
      }
      if (found == TRUE)  {
         break;
      }
   }
   if (!found) {
      *success = FALSE;
      return;
   } else {
      for (sectIndex=0; sectIndex < MAX_SECTORS; ++sectIndex) {
         if (dir[MAX_FILENAME + dirIndex + sectIndex] == 0) {
            break;
         }
         else { 
            readSector(buffer + sectIndex * SECTOR_SIZE,  dir[MAX_FILENAME + dirIndex + sectIndex]); 
         }
      }
      *success = TRUE;
      return;
   }

}

void executeProgram(char *filename, int segment, int *success){
   char buffer[MAX_SECTORS * SECTOR_SIZE];//alokasi buffer
   
   int i;

   readFile(buffer, filename, success);

   

   if(*success == TRUE){
      for(i = 0; i < (MAX_SECTORS * SECTOR_SIZE); i = i + 1){
         putInMemory(segment, i, buffer[i]);
      }
      // printString("y\0");
      launchProgram(segment);
   }
}



void readSector(char *buffer, int sector){
   interrupt(0x13, 0x201, buffer, div(sector, 36) * 0x100 + mod(sector, 18) + 1, mod(div(sector, 18), 2) * 0x100);
}

void writeSector(char *buffer, int sector){
   interrupt(0x13, 0x301, buffer, div(sector, 36) * 0x100 + mod(sector, 18) + 1, mod(div(sector, 18), 2) * 0x100);
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
