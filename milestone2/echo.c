int main() {
    char curdir, res;
    char *argv[100];

    interrupt(0x21, 0x21, &curdir, 0, 0); //syscall getCurdir
    interrupt(0x21, 0x23, 0, argv[0], 0); //dapatkan string yang ingin di echo
    interrupt(0x21, 0x0, argv[0], 0, 0); //print string yang ingin di echo
    interrupt(0x21, 0x0, "\r\n", 0, 0); 
    interrupt(0x21, 0x07, &res, 0, 0); //terminate
}