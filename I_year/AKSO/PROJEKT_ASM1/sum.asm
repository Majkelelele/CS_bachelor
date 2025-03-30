;program napisany w assemblerze, implementuje funkcje void sum(int64_t *x, size_t n);
;Argumentami tej funkcji są wskaźnik x na niepustą tablicę 64-bitowych liczb całkowitych 
;w reprezentacji uzupełnieniowej do dwójki oraz rozmiar n tej tablicy.
;Działanie funkcji zdefiniowane jest przez poniższy pseudokod:
;y = 0;
;for (i = 0; i < n; ++i)
; y += x[i] * (2^floor(64 * i * i / n));
;x[0, ..., n-1] = y;
;Michał Zmyślony 19.05.2023

section .text

global sum

sum:                                            ;główna funkcja, w rdi mamy wskaźnik na tablice, a w rsi rozmiar tablicy
 mov r9, 1                                      ;w r9 trzymany będzie index tablicy, który obecnie przetwarzamy. 
 cmp rsi, r9                                    ;sprawdzamy czy długość tablicy > 1
 jz .exit                                       ;jeśli nie to nic nie robimy

.loop:                                          ;główna pętla programy, dla każdej komórki i, n>i>1 algorytm jest taki sam
 mov rax, r9                                    ;wartość rejestru rax ustawiam na index obecny = r9, w rax obliczamy wartość i^2*64/n
 mul r9                                         ;rax*index, w rax obecnie index^2
 shl rax, 0x6                                   ;index^2 mnoże razy 64
 xor rdx, rdx                                   ;zeruje rdx, aby uniknąć błędów
 div rsi                                        ;dzielimy i^2*64/n, otrzymaliśmy floor(64 * i * i / n)

 mov r8, 0x40                                   ;przenosimy 64 do r8 
 xor rdx, rdx                                   ;unikamy potencjalnych błedów
 div r8                                         ;dzielimy floor(64 * i * i / n)/64, w rax otrzymujemy wynik a w rdx otrzymujemy reszte

 mov rcx, rdx                                   ;do rcx przenosimy wartość przesunięcia bitowego floor(64 * i * i / n) mod 64
 mov r8, [rdi + 8*r9]                           ;przenosimy obecnie przeglądaną komórke x[i]
 shl r8, cl                                     ;mnożymy przez 2^cl -> cl == floor(64 * i * i / n) (mod 64)

 mov rcx, 0x40          
 sub rcx, rdx                                   ;64 - floor(64 * i * i / n) (mod 64), obliczamy przesunięcie upper x[i]

 mov rdx, [rdi + 8*r9]                          ;przenosze wartość x[i] do rdx
 mov qword [rdi + 8*r9], 0  
 mov r10, 0x8000000000000000                    ;przenosze 2^63 do r10
 test [rdi + 8*r9 - 8], r10                     ;sprawdzam czy bit znaku w dotychczasowej sumie jest ustawiony
 jnz  .negative                                 ;dotychczasowa suma ujemna
 jmp .non_negative                              ;dotychczasowa suma >= 0

.negative:                                      ;obecna suma ujemna
 mov qword [rdi + 8*r9], 0xFFFFFFFFFFFFFFFF     ;ustawiam wszystkie bity w x[i] na 1 aby 'przedłużyć' liczbe bez zmiany wartości

.non_negative:                                  ;obecna suma nieujemna
 sar rdx, cl                                    ;przesunięcie upper x[i] o 64 - floor(64 * i * i / n) (mod 64)
 add [rdi + 8*rax], r8                          ;dodaje lower x[i] do sumy
 adc [rdi + 8*rax + 8], rdx                     ;dodaje upper x[i] do sumy razem z możliwym bitem dodatkowym z wcześniejszego dodawania

.end_loop:                             
 inc r9                                         ;zwiększam index
 cmp rsi, r9                                    ;sprawdzam czy już wszystkie komórki przejrzane
 jz .exit                                       ;jeśli tak to kończe program
 jmp .loop                                      ;jeśli nie to powtarzam algorytm dla kolejnej komórki   

.exit:
 ret