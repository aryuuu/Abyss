
void main() {
	char curdir;
	char argc, succ;
	char *argv[100], dirSucc, fileSucc;
	char name[16];	
	interrupt(0x21, 0x21, &curdir, 0, 0);
	interrupt(0x21, 0x22, &argc, 0, 0);
	interrupt(0x21, 0x23, 0, name, 0);
	
	if (argc > 0) { //jika nama file atau direktori diberikan
		// Try deleting file
		
		interrupt(0x21, curdir << 8 | 0x09, name, &succ, 0); //coba hapus sebagai file
		if (succ != 0){ //jika ternyata bukan file,
			interrupt(0x21, curdir << 8 | 0x0A, name, &succ, 0); // coba hapus sebagai direktori
		}
			// interrupt(0x21, 0x23, 0, argv[0], 0);
			// Try deleting dir
			// interrupt(0x21, curdir << 8 | 0x0A, argv[0], &succ, 0);
			//interrupt(0x21, 0x00, name, 0, 0);			
			
		if (succ != 0){ //jika ternyata juga bukan direktori di curdir
			interrupt(0x21, 0x00, "No such file or directory\r\n", 0, 0);
		}
		else {
			interrupt(0x21, 0x00, "done\r\n", 0, 0);
		}
	}
	//interrupt(0x21, (curdir << 8) | 0x06, "shell", 0x2000, &succ);
    interrupt(0x21, 0x07, &succ, 0, 0);	
}