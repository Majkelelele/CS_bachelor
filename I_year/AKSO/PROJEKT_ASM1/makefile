sum.o: sum.asm
	nasm -f elf64 -w+all -w+error -o sum.o sum.asm

sum_example.o: sum_example.c
	gcc -c -Wall -Wextra -std=c17 -O2 -o sum_example.o sum_example.c

sum_example: sum.o sum_example.o
	gcc -z noexecstack -o sum_example sum_example.o sum.o

clean:
	rm *.o sum_example