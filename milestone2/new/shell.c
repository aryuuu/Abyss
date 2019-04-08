#include "helperdec.h"


int main() {
    char workingdir = 0xFF;            // current directory; default root
    char curdir = workingdir;          // directory yang dipakai untuk execute program
    char prefix[3];
    char input[100];
    char argc;              // jumlah argumen
    char** argv;
    int i;              // indeks string input
    int result;
    char dirs[SECTOR_SIZE];

    prefix[0] = '$';
    prefix[1] = ' ';   
    prefix[2] = '\0'; 
    while(1) {
        i = 0;
        argc = 0;
        // argv[0] = '\0';
        interrupt(0x21, 0x00, prefix, 0, 0);// printString("$ ");
        interrupt(0x21, 0x01, input, 0, 0); // readString(input);
        // Chop the spaces, if any
        while (input[i] != '\0') {
            if (input[i] == 0x20) { // 0x20 == space
                argv[argc++] = input + i + 1;
                input[i] = '\0';
            }
            ++i;
        }

        if (strcmp(input, "cd")) {      // pindah ke folder
            if (argc == 0) { //jika tidak ada tujuan
                // interrupt(0x21, 0x00, "Usage: cd relative_path\r\n", 0, 0);
                curdir = 0xFF; //kembali ke root
            } else {            // masuk ke sebuah directory
                if(indexParentFile(argv[0], workingdir) == 0xFE) {
                    interrupt(0x21, 0x00, "No such directory\r\n", 0, 0);
                } else {
                    workingdir = indexParentFile(argv[0], workingdir);
                }
            }
        } else if (strcmp(input, "pwd")) {
            if (workingdir == 0xFF) {
                interrupt(0x21, 0x00, "root\r\n", 0, 0);
            } else {
                interrupt(0x21, 0x2, dirs, DIRS_SECTOR); // read sector
                interrupt(0x21, 0x0, dirs + workingdir * DIRS_ENTRY_LENGTH + 1, 0, 0); // print dir name
                interrupt(0x21, 0x00, "\r\n", 0, 0);
            }
        } else {
            interrupt(0x21, 0x20, workingdir, argc, argv);                          // taruh argumen
            interrupt(0x21, (curdir << 8) | 0x06, input, 0x2000, &result);     // executeProgram
        }
    }
    return 0;
}

#include "helper.h"



// int main() {
//     char curdir;   // default root
//     char prefix[3];
//     char input[100];
//     char argc;           // jumlah argumen
//     char** argv;         //nilai argumen
//     int i;              // indeks string input
//     int result;
//     char dirs[SECTOR_SIZE]; 
//     // enableInterrupts();
//     strcpy("$ ", prefix);
//     curdir = 0xFF;

//     // Main loop
//     while(TRUE) {
//         //inisialisasi
//         i = 0;
//         argc = 0;
//         result = 0;

//         // baca command dan argumen
//         do {
//             interrupt(0x21, 0x00, prefix, 0, 0); //cetak $
//             interrupt(0x21, 0x01, input, 1, 0); //baca command
//         } while(strcmp(input, ""));//ulangi terus jika tidak ada command apapun yang dimasukkan

//         // split argumen 
//         while (input[i] != '\0') {
//             if (input[i] == 0x20) { // 0x20 == space
//                 argv[argc++] = input + i + 1; //salin argumen
//                 input[i] = '\0';
//             }
//             ++i;
//         }

       

//         // Execute the command
//         if (strcmp(input, "cd")) {      // pindah ke folder
//             if (argc == 0){ //jika tidak ada direktori yang dituju
//               curdir = 0xFF;  //kembali ke root
//             } else {
//                 if(indexParentFile(argv[0], curdir) == 0xFE) { //periksa apakah direktori yang dituju ada
//                     interrupt(0x21, 0x00, "No such directory", 0, 0); //direktori tidak ditemukan, tampilkan pesan error
//                 } else {
//                     curdir = indexParentFile(argv[0], curdir); //direktori ditemukan, pindah
//                 }
//             }
//         } else if (strcmp(input, "pwd")) { //print working directory
//             if (curdir == 0xFF) { //sedang di root
//                 interrupt(0x21, 0x00, "root", 0, 0); //print root
//             } else {
//                 interrupt(0x21, 0x2, dirs, DIRS_SECTOR, 0); // baca sektor direktori
//                 interrupt(0x21, 0x0, dirs + curdir * DIRS_ENTRY_LENGTH + 1, 0, 0); // print dir name
                
//             }
//         } else {
//             interrupt(0x21, 0x20, curdir, argc, argv);                // taruh argumen
//             interrupt(0x21, (curdir << 8) | 0x06, input, 0, &result);     // executeProgram
//             if (result == NOT_FOUND){
//               interrupt(0x21, 0x00, "command not found", 0, 0);  
//             } 
//             else if (result == INSUFFICIENT_MEMORY){
//               interrupt(0x21, 0x00, "Insufficient memory", 0, 0);  
//             } 
//         }
//     }
//     return 0;
// }


