; Program implementuje program scoopy, który przyjmuje dwa parametry będące nazwami plików: ./scopy in_file out_file
; Program sprawdza liczbę parametrów. Jeśli ich liczba jest różna od 2, to program kończy się kodem 1.
; Program próbuje utworzyć do zapisu nowy plik out_file z uprawnieniami -rw-r--r--. Jeśli się nie udało, to program kończy się kodem 1.
; Dla każdego odczytanego z pliku in_file bajtu, którego wartość jest kodem ASCII litery s lub S zapisuje ten bajt do pliku out_file.
; Dla każdego odczytanego z pliku in_file maksymalnego niepustego ciągu bajtów niezawierającego bajtu, którego wartość jest kodem ASCII litery s lub S,
; Zapisuje do pliku out_file 16-bitową liczbę zawierającą liczbę bajtów w tym ciągu modulo 65536. 
; Liczbę tę zapisuje binarnie w porządku cienkokońcówkowym (ang. little-endian).
; Michał Zmyślony 07.06.2023


; program przyjmuje dwa argumenty z command line
; liczba argumentów znajduje się w [rsp]
; nazwa pierwszego pliku (argumentu) znajduje się w [rsp + 16]
; nazwa drugiego pliku (argumentu) znajduje się w [rsp + 24] 


global _start               ; zadeklarowanie głównej funkcji

SYS_EXIT equ 60             ; wartość funkcji systemowej SYS_EXIT
SYS_OPEN equ 2              ; wartość funkcji systemowej SYS_OPEN
SYS_READ equ 0              ; wartość funkcji systemowej SYS_READ
SYS_CLOSE equ 3             ; wartość funkcji systemowej SYS_CLOSE
SYS_WRITE equ 1             ; wartość funkcji systemowej SYS_WRITE
O_RDONLY equ 0              ; wartość flagi oznaczającej plik tylko do czytania
O_CREATE equ 64             ; wartość flagi oznaczającej tworzenie pliku
O_WRONLY equ 1              ; wartość flagi oznaczającej plik tylko do zapisywania
O_EXCL equ 0200             ; wartość flagi oznaczającej, że przy połączeniu z O_CREATE nowy plik musi zostać stworzony
s_value equ 0x73            ; wartość 's' w kodzie ASCII
S_value equ 0x53            ; wartość 'S' w kodzie ASCII
buffer_write_size equ 2     ; rozmiar buffera_2 służącego do zapisywania dwóch bajtów do pliku OUT_FILE
buffer_read_size equ 65536  ; rozmiar buffer_read
modulo equ 65536            ; wartość przez jaką mamy wykonywać modulo przy zliczaniu bajtów nie będących 's' ani 'S'
file_permission equ 0644o   ; flaga oznaczająca pozwolenie -rw-r--r-- dla pliku



section .bss 
 buffer_write resw buffer_write_size          ; tworzy buffer_write o rozmiarze 2*buffer_write_size służący do zapisu 2 bajtów
 buffer_read resb buffer_read_size            ; tworzy buffer_read o rozmiarze buffer_read_size służący do czytania z pliku 


section .text

_start:
 mov r8, -1                     ; r8 będzie służyło do zapamiętywania deskryptora dla otwartego pliku IN_FILE, wartość == -1 oznacza nieotworzenie pliku IN_FILE
 mov r9, -1                     ; r9 będzie służyło do zapamiętywania deskryptora dla otwartego pliku OUT_FILE, wartość == -1 oznacza nieotworzenie pliku OUT_FILE
 
 mov rcx, [rsp]                 ; przenosze liczbę argumentów podanych do rcx
 cmp rcx, 3                     ; sprawdzam czy liczba argumentów == 3, 2 argumenty + jeden ukryty z nazwą pliku
 jne error                      ; jeżeli nie to kończę program z wartością 1
 
 
 mov rax, SYS_OPEN              ; funkcja systemowa do otwierania plików
 mov rdi, [rsp + 16]            ; przenosze const char *filename pliku IN_FILE
 mov rsi, O_RDONLY              ; plik służy tylko do odczytu
 syscall                        ; wywołuje funkcje systemową

 cmp rax, 0xfffffffffffff000    ; jeżeli wystąpił bład to w rax jest wartość od -1 do -4095,
 ja error                       ; jeżeli wystąpił błąd to kończę program z wartością 1
 
 mov r8, rax                    ; jeżeli nie wystąpił błąd to w rax deskryptor pliku IN_FILE, zapisuje go w r8, od tej pory r8 > 0
   
 mov rax, SYS_OPEN                          ; funkcja systemowa służąca do odczytu/tworzenia pliku
 mov rdi, [rsp + 24]                        ; przenoszę char *filename pliku OUT_FILE
 mov rsi,  O_CREATE | O_WRONLY | O_EXCL     ; flagi które upewniają się, że nowy plik powstanie, będzie można w nim zapisywać oraz, że nie istnieje jeszcze plik o podanej nazwie 
 mov rdx, file_permission                   ; uprawnienia -rw-r--r--
 syscall                                    ; wywołuje funkcje systemową

 cmp rax, 0xfffffffffffff000                ; jeżeli wystąpił bład to w rax jest wartość od -1 do -4095
 ja error                                   ; jeżeli wystąpił błąd to kończę program z wartością 1
 
 mov r9, rax                                ; jeżeli nie wystąpił błąd to w rax deskryptor pliku OUT_FILE, zapisuje go w r9, od tej pory r9 > 0

 xor r11, r11                               ; upewniam się, że r11 i r10 == 0, r11 będzie używane do zliczania bajtów nie będących 's' ani 'S', r10 będzie służył do sprawdzenia, czy 
 xor r10, r10                               ; wystąpił koniec pliku IN_FILE
 

  push r12                                  ; r12 i r13 będą używane, dlatego zapamiętujemy ich wartości aby przywrócić na końcu działania programu
  push r13

read_and_write_loop:                        ; pętla służąca do przeczytania IN_FILE oraz zapisania odpowiednich wartości w OUT_FILE
 
 mov rdi, r8                                ; deskryptor IN_FILE w r8
 mov rax, SYS_READ                       
 mov rsi, buffer_read                       ; wartość przeczytana będzie zapisana w buffer
 mov rdx, buffer_read_size                  ; rozmiar czytanego fragmentu
 push r11                                   ; przenoszę ilość bajtów bez 's' ani 'S' na stos, ponieważ syscall może nadpisać r11
 syscall


 pop r11                                    ; zdejmuje ilość bajtów bez 's' ani 'S'

 cmp rax, 0xfffffffffffff000                ; jeżeli wystąpił bład to w rax jest wartość od -1 do -4095
 ja error                                   ; jeżeli wystąpił błąd to kończę program z wartością 1


 cmp rax, 0                                 ; jeżeli plik IN_FILE został cały przeczytany rax == 0, przechodzimy do zapisania ilości znaków bez 's' i 'S' od ostatniego 's' lub 'S'
 je write_last_count                        

 mov r13, rax                               ; w rax liczba przeczytanych bajtów, będziemy iść pętlą po wszystkich przeczytanych bajtach i podejmować odpowiednie decyzje
 xor r12, r12
   


buffer_loop:                                ; loop w którym zajmuje się przeczytanymi bajtami zapisanymi w buffer_read (jest ich buffer_read_size)

 movzx rax, byte [buffer_read+r12*1]        ; przenoszę wartość przeczytaną z pamięci
 cmp rax, S_value                           ; sprawdzam czy przeczytałem 's' lub 'S'
 je write_count_no_Ss                       ; jeżeli tak to przechodzę do odpowiedniej etykiety   
 cmp rax, s_value
 je write_count_no_Ss                       ; jeżeli tak to skaczę do etykiety, która zajmuje się zapisaniem 's' lub 'S'
 
  
 inc r11                                    ; jeżeli obecnie analizowany bajt != 's' ani 'S' to zwiększam ich ilość
 inc r12                                    ; dodatkowo przechodzę do kolejnego bajtu wczytanego do buffer_read
 cmp r12, r13                               ; jeżeli przeczytane zostały wszystkie bajty z buffer_read to przechodzę do czytania kolejnej porcji bajtów
 je read_and_write_loop                     ; czytanie nowej porcji bajtów
 jne buffer_loop                            ; czytam kolejny bajt z buffer_read

write_last_count:                           ; jeżeli plik IN_FILE się zakończył, to sprawdzam, czy jest jeszcze coś do zapisania (jeżeli ostatnim bajtem nie było 's' ani 'S')
 cmp r11, 0                                 ; jeżeli r11 == 0 to ostatnim przeczytanym bajtem jest 's' lub 'S' 
 je finished_correctly                      ; czytanie zakończone powodzeniem
 mov r10, 1                                 ; r10 == 1 oznacza IN_FILE przeczytany do końca
 
 
write_count_no_Ss:                          ; służy do zapisania liczby bajtów nie będących 's' ani 'S'
 cmp r11, 0                                 ; jeżeli r11 == 0 to przeczytany bajt == 's' lub 'S'
 je write_Ss                                    
 xor rdx, rdx                               
 mov rax, r11                               ; w r11 liczba bajtów nie będących 's' ani 'S'
 mov r11, modulo                            ; modulo == 65536
 div r11                                    ; wykonuje operacje modulo, w rdx reszta z dzielenia rax mod modulo
 mov [buffer_write], dx                     ; będę chciał zapisać do OUT_FILE resztę z dzielenia
 mov rdi, r9                                ; deskryptor pliku OUT_FILE
 mov rax, SYS_WRITE                         ; funckja systemowa
 mov rsi, buffer_write                      ; trzyma liczbę bajtów nie będących 's' ani 'S' mod 65536
 mov rdx, buffer_write_size                 ; rozmiar buffer_write
 
 syscall
 
  
 cmp rax, 0xfffffffffffff000                ; jeżeli wystąpił bład to w rax jest wartość od -1 do -4095
 ja error                                   ; jeżeli wystąpił błąd to kończę program z wartością 1
   
 
 
 cmp r10, 1                                 ; r10 == 1 oznacza cały plik IN_FILE przeczytany, przechodzę do zamknięcia plików
 je finished_correctly                      

 
write_Ss:                                   ; zapisuje przeczytany bajt 's' lub 'S'
 mov rdi, r9                                ; deskryptor pliku OUT_FILE
 mov rax, SYS_WRITE
 lea rsi, [buffer_read+r12*1]               ; w [buffer_read+r12*1] jest 's' lub 'S'
 mov rdx, 1                                 ; zapisywany jest jeden bajt
 syscall

 cmp rax, 0xfffffffffffff000                ; jeżeli wystąpił bład to w rax jest wartość od -1 do -4095
 ja error                                   ; jeżeli wystąpił błąd to kończę program z wartością 1



 xor r11, r11                               ; rozpoczynam liczenie bajtów bez 's' lub 'S' od nowa, ponieważ ostatnim przeczytanym bajtem było 's' lub 'S'
 inc r12                                    ; przejście do analizy kolejnego bajtu z buffer_read
 cmp r12, r13                               ; sprawdzenie, czy cały buffer_read zaanalizowany
 je read_and_write_loop                     ; przejście do czytania kolejnej porcji znaków
 jne buffer_loop                            ; przejście do analizy kolejnego bajtu buffer_read
 
finished_correctly:                         ; jeżeli program zakończył się poprawnie to r13 == 0
 xor r13, r13                                 
 jmp close_out_FILE                         ; przejście do zamknięcia plików OUT_FILE i IN_FILE
 
error:                                      ; jeżeli wystąpił błąd w trakcie działania programu to r13 == 1
 mov r13, 1

 

close_out_FILE:                             ; zamykanie pliku OUT_FILE
 cmp r9, 0                                  ; r9 < 0 oznacza, że plik OUT_FILE nie został poprawnie otwarty, wtedy go nie zamykamy
 jl close_in_FILE 
 mov rax, SYS_CLOSE                         ; w innym wypadku zamykamy plik OUT_FILE
 mov rdi, r9                                ; deskryptor pliku OUT_FILE
 syscall

 cmp rax, 0xfffffffffffff000                ; jeżeli wystąpił bład to w rax jest wartość od -1 do -4095
 jna close_in_FILE                          ; jeżeli błąd nie wystąpił to przechodzę do zamknięcia pliku IN_FILE
 mov r13, 1                                 ; jeżeli wystąpił błąd to r13 == 1 oraz przechodzę do zamknięcia pliku IN_FILE
 
close_in_FILE:
 cmp r8, 0                                  ; r8 < 0 oznacza, że plik IN_FILE nie został poprawnie otwarty, wtedy go nie zamykamy
 jl exit                                    ; przechodzimy do zakończenia programu
 mov rax, SYS_CLOSE                         ; zamykanie pliku IN_FILE
 mov rdi, r8                                ; deskryptor pliku IN_FILE
 syscall

 cmp rax, 0xfffffffffffff000                ; jeżeli wystąpił bład to w rax jest wartość od -1 do -4095
 jna exit                                   ; jeżeli błąd nie wystąpił to przechodzę do zakończenia programu
 mov r13, 1                                 ; jeżeli błąd wystąpił to r13 == 1




exit:                                       ; zakończenie programu 
 mov rdi, r13                               ; r13 do tej pory pamiętało kod błędu, kopiujemy go do rdi aby zwrócić
 mov rax, SYS_EXIT                          ; funkcja systemowa do zamykania programu
 pop r13                                    ; przywracamy pierwotną wartość do r13 oraz r12
 pop r12                                          
 syscall                                    ; kończymy program
      
