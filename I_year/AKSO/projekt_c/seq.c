/*
Projekt implementuje dynamicznie ładowaną bibliotekę obsługującą zbiór ciągów z relacją równoważności.
Elementami zbioru ciągów są niepuste ciągi, których elementami są liczby -1,0,1,2. W implementacji ciąg reprezentowany jest jako napis.
Ciągi mogą należeć do różnych klas abstrakcji, które mają nazwy.

Michał Zmyślony
24.04.2023
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
// #include "seq.h"


/*
Struktura danych która, przetrzymuje ciągi. Jest to drzewo trie. Pola tej struktury oznaczają odpowiednio:
w - wartość danego elementu {0,1,2}
son_0,son_1,son_2 - wskaźniki na kolejny element ciągu będący 0,1 lub 2 i w wyjątkowym przypadku -1;
class - wskaźnik na klase abstrakcji do której należy ciąg
*/
struct seq{
    int w;
    struct seq *son_0;
    struct seq *son_1;
    struct seq *son_2;
    char *class;
    
};
typedef struct seq seq_t;

/*
Funckja tworzy nowy węzeł drzewa o wartości w oraz klasie abstrakcji - word.
Zwraca stworzony węzeł.
*/
seq_t *New_node(int w, char *word){
    seq_t *node = (seq_t *)malloc(sizeof(seq_t));
    if(node == NULL) {
        errno = ENOMEM;
        return NULL;
    }
    else {
        node->son_0 = NULL;
        node->son_1 = NULL;
        node->son_2 = NULL;
        node->w = w;
        if(word == NULL) node->class = NULL;
        else {
            char *class = malloc(strlen(word) + 1);
            if(class == NULL) {
                free(node);
                errno = ENOMEM;
                return NULL;
            }
            strcpy(class,word);
            node->class = class;
        }
        return node;
    }
}

/*
Funkcja rozpoczyna strukturę trie zaczynająć od węzła o wartości -1, który nie ma klasy abstrakcji.
Jest to jedyny węzeł o wartości innej od {0,1,2}.
Zwraca węzeł.
*/
seq_t * seq_new() {
    return New_node(-1,NULL);
}

/*
Funkcja powoduje, że jakikolwiek podciąg należący do drzewa o korzeniu k oraz do klasy abstrakcji word przestaje na nią wskazywać.
Nic nie zwraca.
*/
void walk(seq_t *k, char* word) {
	if(k != NULL) {
		if(k->class == word) {
            k->class = NULL;
        }
        walk(k->son_0,word);
        walk(k->son_1,word);
        walk(k->son_2,word);
	}
}

/*
Funkcja przechodzi po drzewie k oraz zwalnia pamięc zaalokowaną na wszystkie klasy abstrakcji do których należy jakikolwiek podciąg drzewa k.
Funkcja nic nie zwraca.
*/
void seq_delete_classes(seq_t *k, seq_t *act) {
    if(act != NULL) {
        if(act->class != NULL) {
        	char *temp = act->class;
            walk(k,act->class);
            free(temp);
        }
        seq_delete_classes(k, act->son_0);
        seq_delete_classes(k, act->son_1);
        seq_delete_classes(k, act->son_2);
    }
}

/*
Funkcja seq_delete usuwa zbiór ciągów i zwalnia całą używaną przez niego pamięć. Nic nie robi, jeśli zostanie wywołana ze wskaźnikiem NULL. 
Po wykonaniu tej funkcji przekazany jej wskaźnik staje się nieważny.
Nic nie zwraca.
*/
void seq_delete(seq_t *p) {
    if(p != NULL) {
        if(p->w == -1) {
            seq_delete_classes(p,p);
        }
        seq_delete(p->son_0);
        seq_delete(p->son_1);
        seq_delete(p->son_2);
        free(p);
    }
}

/*
Funkcja konwertuje słowo word do tablicy int tab. Zwraca tablicę oraz aktualizuje ilość elementów w tablicy jako *last;
*/
int *convert_to_int(char const *word, int *last) {
    for(int i = 0; word != NULL && word[i] != '\0'; i++) {
        if(word[i] != '2' && word[i] != '1' && word[i] != '0'){
            errno = EINVAL;
            return NULL;
        }
    }
    int size = (*last);
    int *tab = (int *)malloc(sizeof(size));
    if(tab == NULL) {
        errno = ENOMEM;
        return NULL;
    }
    else {
        tab[0] = -1;
        for (int i = 0; word != NULL && word[i] != '\0'; i++) {
            if(i >= size) {
                int *temp = (int *)realloc(tab,sizeof(int)*(size_t)size*2);
                if(temp == NULL) {
                    free(tab);
                    errno = ENOMEM;
                    return NULL;
                }
                tab = temp;
                size = size*2; 
            }
        tab[i] = (word[i] - 48);
        if(tab[i]  < 0 || tab[i] > 2) {
            tab[0] = -1;
        }
        (*last) = i;
        }
        if(tab[0] != -1) {
            (*last) += 1;
        }
        else {
            errno = EINVAL;
        }
    }
    return tab;
}

void memory_allocation_error(int last_son, seq_t *last) {
    if(last_son == 0) {
        seq_delete(last->son_0);
        last->son_0 = NULL;
    }
    else if(last_son == 1) {
        seq_delete(last->son_1);           
        last->son_1 = NULL;
    }
    else {
        seq_delete(last->son_2);            
        last->son_2 = NULL;
    }
}

/*
Funckja dodaje do zbioru ciągów ciąg reprezentowany w tablicy tab. Wartość *check po zakończeniu działania funkcji oznacza odpowiednio:
-1 - nastąpił błąd alokacji pamięci, cały ciąg reprezentowany w tab jest usuwany.
0 - zbiór ciągów się nie zmienił.
1 - został dodany conajmniej jeden ciąg.
*/
void seq_add_temp(seq_t *p, int *tab, int i, int max, int *check, seq_t *last, int last_son) {
    if(i < max && (*check) != -1) {
        if(tab[i] == 0) {
            if(p->son_0 == NULL) {
                p->son_0 = New_node(0,NULL);
                if(p != NULL && p->son_0 == NULL) {
                    errno = ENOMEM;
                    (*check) = -1;
                }
                if((*check) == 0) {
                    last_son = 0;
                    (*check) = 1;
                }
            }
            else {
                last = p->son_0;
            }
            seq_add_temp(p->son_0,tab,i+1,max,check,last,last_son);
        }
        else if(tab[i] == 1) {
            if(p->son_1 == NULL) {
                p->son_1 = New_node(1,NULL);
                if(p != NULL && p->son_1 == NULL) {
                    errno = ENOMEM;
                    (*check) = -1;
                }
                if((*check) == 0) {
                    last_son = 1;
                    (*check) = 1;
                }
            }
            else {
                last = p->son_1;
            }
            seq_add_temp(p->son_1,tab,i+1,max,check,last,last_son);
        }
        else {
            if(p->son_2 == NULL) {
                p->son_2 = New_node(2,NULL);
                if(p != NULL && p->son_2 == NULL) {
                    errno = ENOMEM;
                    (*check) = -1;
                }
                if((*check) == 0) {                   
                    last_son = 2;
                    (*check) = 1;
                }
            }
            else {
                last = p->son_2;
            }
            seq_add_temp(p->son_2,tab,i+1,max,check,last,last_son);
        }
    }
    else if((*check) == -1) {
        memory_allocation_error(last_son,last);
    }   
}

/*
Funkcja dodaje do zbioru ciągów p podany ciąg s i wszystkie niepuste podciągi będące jego prefiksem. Zwraca odpowiednio:
1 – jeśli co najmniej jeden nowy ciąg został dodany do zbioru;
0 – jeśli zbiór ciągów się nie zmienił;
-1 – jeśli któryś z parametrów jest niepoprawny lub wystąpił błąd alokowania pamięci; funkcja ustawia wtedy errno odpowiednio na EINVAL lub ENOMEM.
*/
int seq_add(seq_t *p, char const *s) {
    if(p != NULL && s != NULL) {
        int last = 1;
        int *tab = convert_to_int(s, &last);
        if(tab == NULL || tab[0] == -1) {
        	free(tab);
            return -1;
        }
        int check = 0;
        seq_t *last_correct = p;
        int last_son = -1;
        seq_add_temp(p,tab,0,last, &check,last_correct,last_son);
        free(tab);
        return check;
    }
    else {
    	errno = EINVAL;
        return -1;
    }
    
}

/*
Funkcja sprawdza, czy ciąg reprezentowany w tablicy tab, należy do zbioru ciągów.
Jeżeli tak to: zwraca 1, ustawia *act na dany ciąg a *parent na rodzica tego ciągu.
Jeżeli nie to: zwraca 0, ustawia *act na NULL a *parent na ciąg reprezentujący najdłuższą istniejącą część ciagu tab;
*/
int find_seq(seq_t **act, int *tab, int max,  seq_t **parent) {
    for(int i = 1; i < max && (*act) != NULL; i++) {
        if(tab[i] == 0) {
            (*parent) = (*act);
            (*act) = (*act)->son_0;
        }
        else if(tab[i] == 1) {
            (*parent) = (*act);
            (*act) = (*act)->son_1;
        }
        else{
            (*parent) = (*act);
            (*act) = (*act)->son_2;
        }
    }
    free(tab);
    return (*act) != NULL;    
}

/*
Fukcja sprawdza czy w drzewie k istnieje ciąg należący do klasy class, nie wliczając poddrzewa act;
Zwraca 1 jeżeli tak, oraz 0 jeżeli nie.
*/
int check_if_not_alone(seq_t *k, seq_t *act, char *class) {
    if(k != NULL && k != act) {
        return k->class == class || check_if_not_alone(k->son_0,act,class) || 
        check_if_not_alone(k->son_1,act,class) || check_if_not_alone(k->son_2,act,class);
    }
    else {
        return 0;
    }
}

/*
Drzewo small_k jest poddrzewem drzewa k
Funkcja usuwa(zwalnia pamięć) wszystkie klasy do których należą podciągi drzewa small_k ale nie inne należą inne ciągi drzewa k.
Nic nie zwraca
*/
void seq_remove_delete_classes(seq_t *k, seq_t *act, seq_t *small_k) {
    if(act != NULL) {
        if(act->class != NULL && !check_if_not_alone(k,small_k,act->class)) {
            char *temp = act->class;
            walk(k,act->class);
            free(temp);
        }
        seq_remove_delete_classes(k,act->son_0, small_k);
        seq_remove_delete_classes(k,act->son_1, small_k);
        seq_remove_delete_classes(k,act->son_2, small_k);
    }
}

/*
Drzewa act jest poddrzewem drzewa k.
Usuwa całe drzewo act oraz klasy abstrakcji do których należą ciągi drzewa act ale nie należą inne podciągi drzewa k.
Nic nie zwraca.
*/
void seq_remove_delete(seq_t *k, seq_t *act) {
    if(act != NULL) {
        if(act->w == -1) {
            seq_remove_delete_classes(k,act,act);
        }
        seq_remove_delete(k,act->son_0);
        seq_remove_delete(k,act->son_1);
        seq_remove_delete(k,act->son_2);
        free(act);
    }
}

/*
Funkcja usuwa ze zbioru ciągów podany ciąg i wszystkie ciągi, których jest on prefiksem. Zwraca odpowiednio:
1 – jeśli co najmniej jeden ciąg został usunięty ze zbioru.
0 – jeśli zbiór ciągów się nie zmienił.
-1 – jeśli któryś z parametrów jest niepoprawny; funkcja ustawia wtedy errno na EINVAL.
*/
int seq_remove(seq_t *p, char const *s) {
    if(p == NULL) {
        errno = EINVAL;
        return -1;
    }
    int last = 1;
    int *tab = convert_to_int(s, &last);
    if(tab == NULL || tab[0] == -1) {
        free(tab);
        return -1;
    }
    seq_t *parent = p;
    seq_t *act;
    if(tab[0] == 0) {
        act = p->son_0;
    }
    if(tab[0] == 1) {
        act = p->son_1;
    }
    if(tab[0] == 2) {
        act = p->son_2;
    }
    int check = find_seq(&act,tab,last,&parent);
    if(act != NULL) {
        if(act->w == 0) {
            parent->son_0 = NULL;
        }
        if(act->w == 1) {
            parent->son_1 = NULL;
        }
        if(act->w == 2) {
            parent->son_2 = NULL;
        }
    }
    if(act != NULL) {
        act->w = -1;
        seq_remove_delete(p,act);
    }
    return check;
}

/*
Funkcja sprawdza, czy podany ciąg należy do zbioru ciągów. Zwraca odpowiednio:
1 – jeśli ciąg należy do zbioru ciągów.
0 – jeśli ciąg nie należy do zbioru ciągów.
-1 – jeśli któryś z parametrów jest niepoprawny; funkcja ustawia wtedy errno na EINVAL.
*/
int seq_valid(seq_t *p, char const *s) {
    if(p == NULL) {
        errno = EINVAL;
        return -1;
    }
    int last = 1;
    int *tab = convert_to_int(s,&last);
    if(tab == NULL || tab[0] == -1) {
    	free(tab);
        return -1;
    }
    seq_t *parent = p;
    seq_t *act;
    if(tab[0] == 0) {
        act = p->son_0;
    }
    if(tab[0] == 1) {
        act = p->son_1;
    }
    if(tab[0] == 2) {
        act = p->son_2;
    }
    return find_seq(&act,tab,last,&parent);
}

/*
Funkcja nadaje nową klasę abstrakcji new_class wszystkich podciągom drzewa p, które należą do klasy old_class.
Nic nie zwraca.
*/
void walk_reclass(seq_t *p, char *new_class, char *old_class) {
    if(p != NULL) {
        if(p->class == old_class) {
            p->class = new_class;
        }
        walk_reclass(p->son_0, new_class, old_class);
        walk_reclass(p->son_1, new_class, old_class);
        walk_reclass(p->son_2, new_class, old_class);
    }
}

/*
Funkcja ustawia lub zmienia nazwę klasy abstrakcji, do której należy podany ciąg. Zwraca odpowiednio:
1 – jeśli nazwa klasy abstrakcji została przypisana lub zmieniona;
0 – jeśli ciąg nie należy do zbioru ciągów lub nazwa klasy abstrakcji nie została zmieniona;
-1 – jeśli któryś z parametrów jest niepoprawny lub wystąpił błąd alokowania pamięci; funkcja ustawia wtedy errno odpowiednio na EINVAL lub ENOMEM
*/
int seq_set_name(seq_t *p, char const *s, char const *n) {
    if(p == NULL || n == NULL || s == NULL || n[0] == '\0') {
        errno = EINVAL;
        return -1;
    }
    int last = 1;
    int *tab = convert_to_int(s,&last);
    if(tab == NULL || tab[0] == -1) {       
        free(tab);
        return -1;
    }
    if(!seq_valid(p,s)) {
    	free(tab);
        return 0;
    }
    seq_t *parent = p;
    seq_t *act;
     if(tab[0] == 0) {
         act = p->son_0;
     }
     if(tab[0] == 1) {
         act = p->son_1;
     }
     if(tab[0] == 2) {
         act = p->son_2;
     }
    find_seq(&act,tab,last,&parent);
    if(act->class != NULL) {
        if((strcmp(n,act->class) == 0)) {
            return 0;
        }
    }
    char *class = (char *) malloc(strlen(n)+1);
    if(class == NULL) {
        free(class);
        errno = ENOMEM;
        return -1;
    }
    if (strcpy(class, n) == NULL) {
        free(class);
        return -1;
    }
    if(act->class == NULL) {
        act->class = class;
    }
    else {
        char *temp = act->class;
        walk_reclass(p,class,act->class);
        free(temp);
    }
    return 1;
}

/*
Funkcja seq_get_name daje wskaźnik na napis zawierający nazwę klasy abstrakcji, do której należy podany ciąg. Zwraca odpowiednio:
wskaźnik na napis zawierający nazwę lub,
NULL– jeśli ciąg nie należy do zbioru ciągów lub klasa abstrakcji zawierająca ten ciąg nie ma przypisanej nazwy; funkcja ustawia wtedy errno na 0.
NULL– jeśli któryś z parametrów jest niepoprawny; funkcja ustawia wtedy errno na EINVAL.
*/
char const *seq_get_name(seq_t *p, char const *s) {
    int last = 1;
    int *tab = convert_to_int(s,&last);
    if(p == NULL || tab == NULL || tab[0] == -1) {
        if(p == NULL) {
            errno = EINVAL;
        }
        free(tab);
        return NULL;
    }
    if(!seq_valid(p,s)) {
        errno = 0;
        free(tab);
        return NULL;
    }
    seq_t *parent = p;
    seq_t *act;
    if(tab[0] == 0) {
        act = p->son_0;
    }
    if(tab[0] == 1) {
        act = p->son_1;
    }
    if(tab[0] == 2) {
        act = p->son_2;
    }
    find_seq(&act,tab,last,&parent);
    if(act->class == NULL || strlen(act->class) == 0) {
        errno = 0;
        return NULL;
    }
    return act->class;
}


/*
Funkcja zarządza przypadkiem gdy ciąg seq należy do jakieś klasy abstrakcji, natomiast ciąg seq_1 nie należy. Stwarza nową klasę abstrakcji oraz zmienia klasy abstrakcji wedle założeń programu.
Zwraca odpowiednio:
-1 - jeżeli wystąpił błąd alokacji pamięci
1 - jeżeli nowa klasa została stworzona.
*/
int seq_equiv_one_null(seq_t *p, seq_t *seq, seq_t *seq_1) {
    char *old_class = seq->class;
    char *new_class = (char *) malloc(sizeof(char)*sizeof(old_class));
    if(new_class == NULL) {
        errno = ENOMEM;
        return -1;
    }
    strcpy(new_class,old_class);
    walk_reclass(p,new_class,old_class);
    seq_1->class = new_class;
    free(old_class);
    return 1;
}

/*
Funkcja zarząda klasami abstrakcji dwóch istniejących ciągów seq_1 oraz seq_2 należacych do różnych klas abstrakcji. 
Nadaje klasy abstrakcji według założeń programu opisanych w funckji poniżej - seq_equiv. Zwraca odpowiednio:
-1 - jeżeli wystapił błąd alokacji pamięci
1 - jeżeli powstała nowa klasa abstrakcji
*/
int seq_create_new_class(seq_t *p, seq_t *seq_1, seq_t *seq_2) {
    char *new_class;
    char *old_class;
    if(seq_1->class == NULL) {
        return seq_equiv_one_null(p,seq_2,seq_1);
    }
    else if(seq_2->class == NULL) {
        return seq_equiv_one_null(p,seq_1,seq_2);
    }
    else if(strcmp(seq_1->class,seq_2->class) == 0) {
        new_class = (char *) malloc(sizeof(char)*sizeof(seq_1->class));
        if(new_class == NULL) {
            errno = ENOMEM;
            return -1;
        }
        strcpy(new_class,seq_1->class);
        old_class = seq_1->class;
        walk_reclass(p,new_class,old_class);
        free(old_class);
        old_class = seq_2->class;
        walk_reclass(p,new_class,old_class);
        free(old_class);
        return 1;
    }
    else {
        char *class_1 = seq_1->class;
        char *class_2 = seq_2->class;
        new_class = (char *) malloc(strlen(class_1) + strlen(class_2) + 1);
        if(new_class == NULL) {
            errno = ENOMEM;
            return -1;
        }
        strcpy(new_class,class_1);
        strcat(new_class,class_2);
        walk_reclass(p,new_class,class_1);
        free(class_1);
        walk_reclass(p,new_class,class_2);
        free(class_2);
        return 1;
    }
}

/*
Funkcja seq_equiv łączy w jedną klasę abstrakcji klasy abstrakcji reprezentowane przez podane ciągi. 
Jeśli obie klasy abstrakcji nie mają przypisanej nazwy, to nowa klasa abstrakcji też nie ma przypisanej nazwy. 
Jeśli dokładnie jedna z klas abstrakcji ma przypisaną nazwę, to nowa klasa abstrakcji dostaje tę nazwę. 
Jeśli obie klasy abstrakcji mają przypisane różne nazwy, to nazwa nowej klasy abstrakcji powstaje przez połączenie tych nazw. 
Jeśli obie klasy abstrakcji mają przypisane taką same nazwę, to ta nazwa pozostaje nazwą nowej klasy abstrakcji. Zwraca odpowiednio:
1 – jeśli powstała nowa klasa abstrakcji;
0 – jeśli nie powstała nowa klasa abstrakcji, bo podane ciągi należą już do tej samej klasy abstrakcji lub któryś z nich nie należy do zbioru ciągów;
-1 – jeśli któryś z parametrów jest niepoprawny lub wystąpił błąd alokowania pamięci; funkcja ustawia wtedy errno odpowiednio na EINVAL lub ENOMEM.
*/
int seq_equiv(seq_t *p, char const *s1, char const *s2) {
    if(p == NULL || s1 == NULL || s2 == NULL || 0) {
        errno = EINVAL;
        return -1;
    }
    int last = 1;
    for(int i = 0; s1 != NULL && s1[i] != '\0'; i++) {
        if(s1[i] != '2' && s1[i] != '1' && s1[i] != '0'){
            errno = EINVAL;
            return -1;
        }
    }
    for(int i = 0; s2 != NULL && s2[i] != '\0'; i++) {
        if(s2[i] != '2' && s2[i] != '1' && s2[i] != '0'){
            errno = EINVAL;
            return -1;
        }
    }
    int *tab = convert_to_int(s1,&last);
    int last_1 = 1;
    int *tab_1 = convert_to_int(s2,&last_1);
    if(tab == NULL || tab[0] == -1 || tab_1 == NULL || tab_1[0] == -1) {
    	free(tab);
    	free(tab_1);
        return -1;
    }
    if(!seq_valid(p,s1) || !seq_valid(p,s2)) {
    	free(tab);
        free(tab_1);
        return 0;
    }
    seq_t *parent_1 = p;
    seq_t *act_1;
    if(tab[0] == 0) {
        act_1 = p->son_0;
    }
    if(tab[0] == 1) {
        act_1 = p->son_1;
    }
    if(tab[0] == 2) {
        act_1 = p->son_2;
    }
    find_seq(&act_1,tab,last,&parent_1);
    seq_t *parent_2 = p;
    seq_t *act_2;
    if(tab_1[0] == 0) {
        act_2 = p->son_0;
    }
    if(tab_1[0] == 1) {
        act_2 = p->son_1;
    }
    if(tab_1[0] == 2) {
        act_2 = p->son_2;
    }
    find_seq(&act_2,tab_1,last_1,&parent_2);
    if(act_1->class == act_2->class && 1) {
        if(act_1->class == NULL && act_1 != act_2) {
            char *class = (char *)malloc(strlen("") + 1);
            if(class == NULL) {
            	errno = ENOMEM;
            	return -1;
            }
            if (strcpy(class, "") == NULL) {
                free(class);
                return -1;
            }
            act_1->class = class;
            act_2->class = class;
            return 1;
        }
        return 0;
    }
    return seq_create_new_class(p,act_1,act_2);
}