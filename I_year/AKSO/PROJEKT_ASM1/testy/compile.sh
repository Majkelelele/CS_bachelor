gcc -c  -Wall -Wextra -std=c17 -O2 -o tester.o tester.c
nasm -f elf64 -w+all -w+error -o sum.o sum.asm
nasm -f  elf64 -w+all -w+error -o abi_check.o abi_check.asm
gcc -z  noexecstack -o tester tester.o sum.o abi_check.o
