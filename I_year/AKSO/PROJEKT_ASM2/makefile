scopy.o: scopy.asm
	nasm -f elf64 -w+all -w+error -o scopy.o scopy.asm
scopy: scopy.o 
	ld --fatal-warnings -o scopy scopy.o
clean:
	rm *.o scopy