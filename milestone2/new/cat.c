
int main() {
	char curdir, argc, res;
	char argv[20];
	char buffer[10];
	int *result, i;

	// get curdir
	interrupt(0x21, 0x21, &curdir, 0, 0);
	// get argc
	interrupt(0x21, 0x22, &argc, 0, 0);
	// get arguement(s)
	interrupt(0x21, 0x23, 0, argv, 0);

	if (argc == 2) {	// menuliskan isi file, asumsi argumen kedua adalah flag -w
		// baca input user
		interrupt(0x21, 0x00, ">> ", 0, 0); //
		interrupt(0x21, 0x01, buffer, 0, 0); //baca input yang ingin ditulis ke file			
		// tuliskan input ke dalam file
		interrupt(0x21, 0xFF << 8 | 0x05, buffer, argv, result);
		
		// jika berhasil, cetak pesan berhasil
		if (result == 0) {
			interrupt(0x21, 0x00, "done\r\n", 0, 0);
		}
	} else {			// menampilkan isi file
		// asumsi file dipastikan ada di di curent directory
		interrupt(0x21, (curdir << 8) | 0x04, buffer, argv, result); // readfile
		// print isi ke layar
		if (result == 0) { //
			interrupt(0x21, 0x00, buffer, 0, 0);
			// interrupt(0x21, 0x00, "\r\n", 0, 0);			
		}
	}

	interrupt(0x21, (curdir << 8) | 0x06, "shell", 0x2000, result);
}