#!/bin/bash
dd if=/dev/zero of=floppya.img bs=512 count=2880
nasm bootload.asm
dd if=bootload of=floppya.img bs=512 count=1 conv=notrunc
dd if=map.img of=floppya.img bs=512 count=1 seek=256 conv=notrunc
dd if=files.img of=floppya.img bs=512 count=1 seek=258 conv=notrunc
dd if=sectors.img of=floppya.img bs=512 count=1 seek=259 conv=notrunc



bcc -ansi -c -o kernel.o kernel.c
as86 kernel.asm -o kernel_asm.o
ld86 -o kernel -d kernel.o kernel_asm.o
echo "kernel linked"
dd if=kernel of=floppya.img bs=512 conv=notrunc seek=3
echo "kernel loaded"
gcc loadFile.c -o loadFile -w

as86 lib.asm -o lib.o

bcc -ansi -c -o shell.o shell.c
bcc -ansi -c -o echo.o echo.c
bcc -ansi -c -o mkdir.o mkdir.c
bcc -ansi -c -o ls.o ls.c
bcc -ansi -c -o rm.o rm.c
bcc -ansi -c -o cat.o cat.c

ld86 -o shell -d shell.o lib.o
ld86 -o echo -d echo.o lib.o
ld86 -o mkdir -d mkdir.o lib.o
ld86 -o ls -d ls.o lib.o
ld86 -o rm -d rm.o lib.o
ld86 -o cat -d cat.o lib.o


./loadFile shell
./loadFile echo
./loadFile mkdir
./loadFile ls
./loadFile rm
./loadFile cat
./loadFile keyproc
./loadFile keyproc2
