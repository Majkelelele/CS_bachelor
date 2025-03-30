#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>


#ifndef WIERSZE
# define WIERSZE 22
#endif

#ifndef KOLUMNY
# define KOLUMNY 80
#endif

struct Lista{
    int w;
    int k;
    int stan;
    int liczba_sasiadow;
    struct Lista* nast_w;
    struct Lista* nast_k;
    struct Lista* poprz_k;
    struct Lista* poprz_w;
};
typedef struct Lista Lista_new;


void zwolnij_liste(Lista_new *l){
    while(l != NULL) {
        Lista_new *pom = l->nast_w;
        while(l != NULL) {
            Lista_new *pom_2 = l->nast_k;
            free(l);
            l = NULL;
            l = pom_2;
        }
        l = pom;
    }
}



void Drukuj_Plansze(Lista_new *l) {
    Lista_new *pom = l;
    while(pom != NULL) {
        Lista_new *pom_1 = pom;
        while(pom_1 != NULL) {
            printf("%d,%d,%d,%d",pom_1->w,pom_1->k,pom_1->stan,pom_1->liczba_sasiadow);
            pom_1 = pom_1->nast_k;
            printf("=>");
        }
        printf("NULL\n");
        printf("||\n");
        pom = pom->nast_w;
    }
    printf("NULL\n");
}

void czytaj_input(int **A, int *n) {
    int *plansza = NULL;
    int c;
    int i;
    int rozmiar = 0;
    for(i = 0; ((c = getchar()) != EOF); i++) {
        if(i == rozmiar){
            rozmiar = rozmiar*2 + 2;
            plansza = realloc(plansza, (size_t)rozmiar*sizeof(*plansza));
        }
        if(c >= '0' && c <= '9'){
            ungetc(c,stdin);
            scanf("%d", &c);
        }
        else if(c == '-') {
            ungetc(c,stdin);
            scanf("%d",&c);
        }
        else{
            c = (-1)*c;
        }
        plansza[i] = c;
    }
    *A = plansza;
    *n = i;
}

void Dodaj_pomiedzy_kolumnami(Lista_new *poprz, Lista_new *nast, int w, int k, int stan, Lista_new *nast_wiersz, Lista_new *poprz_wiersz){
    Lista_new *akt = malloc(sizeof(Lista_new));
    akt->nast_k = NULL;
    akt->nast_w = NULL;
    akt->poprz_k = NULL;
    akt->poprz_w = NULL;
    akt->liczba_sasiadow = 0;
    if(poprz != NULL) {
        poprz->nast_k = akt;
    }
    akt->poprz_k = poprz;
    akt->nast_k = nast;
    akt->nast_w = nast_wiersz;
    akt->poprz_w = poprz_wiersz;
    akt->k = k;
    akt->w = w;
    akt->stan = stan;
    if(nast != NULL) {
        nast->poprz_k = akt;
    }
    if(nast_wiersz != NULL) {
        nast_wiersz->poprz_w = akt;
    }
    if(poprz_wiersz != NULL) {
        poprz_wiersz->nast_w = akt;
    }
}

void dodaj_pomiedzy_wierszami(Lista_new *nast, Lista_new *poprz, int w, int stan, int k, Lista_new *pop_k, Lista_new *nast_k) {
    Lista_new *akt = malloc(sizeof(Lista_new));
    akt->nast_k = NULL;
    akt->nast_w = NULL;
    akt->poprz_k = NULL;
    akt->poprz_w = NULL;
    akt->liczba_sasiadow = 0;
    akt->k = k;
    akt->stan = stan;
    akt->w = w;
    akt->poprz_k = pop_k;
    akt->nast_k = nast_k;
    akt->nast_w = nast;
    akt->poprz_w = poprz;
    if(poprz != NULL) {
        poprz->nast_w = akt;
    }
    if(nast != NULL) {
        nast->poprz_w = akt;
    }
    if(pop_k != NULL) {
        pop_k->nast_k = akt;
    }
    if(nast_k != NULL) {
        nast_k->poprz_k = akt;
    }
}

Lista_new * wiersz_sasiadow(Lista_new *akt, Lista_new *poprz) {
    while(poprz->nast_k != NULL && poprz->nast_k->k <= akt->k - 1) {
        poprz = poprz->nast_k;
    }
    if(poprz->k != akt->k - 1) {
        Dodaj_pomiedzy_kolumnami(poprz,poprz->nast_k,poprz->w,akt->k - 1,0,NULL,NULL);
        poprz = poprz->nast_k;
    }
    if(poprz->nast_k == NULL || poprz->nast_k->k > akt->k) {
        Dodaj_pomiedzy_kolumnami(poprz,poprz->nast_k,poprz->w,akt->k,0,NULL,NULL);
    }
    poprz = poprz->nast_k;
    if(poprz->nast_k == NULL || poprz->nast_k->k > akt->k + 1) {
        Dodaj_pomiedzy_kolumnami(poprz,poprz->nast_k,poprz->w,akt->k+1,0,NULL,NULL);
    }
    return poprz;
}

void Dodaj_niezywych_sasiadow_v2(Lista_new *l) {
    if(l != NULL) {
        Lista_new *akt = l;
        while(akt != NULL && akt->stan != 1) {
            akt = akt->nast_k;
        }
        if(akt != NULL) {
            Lista_new *poprz = l->poprz_w;
            Lista_new *nast = l->nast_w;
            if(poprz == NULL || poprz->w != akt->w - 1) {
                dodaj_pomiedzy_wierszami(l,poprz,akt->w - 1, 0, akt->k - 1, NULL, NULL);
                poprz = l->poprz_w;
            }
            if(nast == NULL || nast->w != akt->w + 1) {
                dodaj_pomiedzy_wierszami(nast,l,akt->w+1,0,akt->k-1,NULL,NULL);
                nast = l->nast_w;
            }
            if(poprz->k > akt->k - 1) {
                if(poprz != NULL) {
                    dodaj_pomiedzy_wierszami(l,poprz->poprz_w,poprz->w,0,akt->k-1,NULL,poprz);
                    poprz = poprz->poprz_k;
                }
            }
            if(nast->k > akt->k-1) {
                if(nast != NULL) {
                    dodaj_pomiedzy_wierszami(nast->nast_w,l,nast->w,0,akt->k-1,NULL,nast);
                    nast = nast->poprz_k;
                }
            }
            while(akt != NULL) {
                if(akt->stan == 1) {
                    poprz = wiersz_sasiadow(akt,poprz);
                    if(akt->poprz_k == NULL) {
                        Dodaj_pomiedzy_kolumnami(akt->poprz_k,akt,akt->w,akt->k-1,0,NULL,NULL);
                        akt->poprz_k->nast_w = akt->nast_w;
                        akt->poprz_k->poprz_w = akt->poprz_w;
                        if(akt->poprz_w != NULL) {
                            akt->poprz_w->nast_w = akt->poprz_k;
                        }
                        if(akt->nast_w != NULL) {
                            akt->nast_w->poprz_w = akt->poprz_k;
                        }
                        akt->nast_w = NULL;
                        akt->poprz_w = NULL;
                    }
                    else if(akt->poprz_k->k < akt->k-1) {
                        Dodaj_pomiedzy_kolumnami(akt->poprz_k,akt,akt->w,akt->k-1,0,NULL,NULL);
                    }
                    if(akt->nast_k == NULL || akt->nast_k->k > akt->k+1) {
                        Dodaj_pomiedzy_kolumnami(akt,akt->nast_k,akt->w,akt->k+1,0,NULL,NULL);
                    }
                    nast = wiersz_sasiadow(akt,nast);
                }
                akt = akt->nast_k;
            }
        }
        while(l->poprz_k != NULL) {
            l = l->poprz_k;
        }
        Dodaj_niezywych_sasiadow_v2(l->nast_w);
    }
}



Lista_new * Wpisz_Plansze_poczatkowa_v2(int *input, int *j) {
    Lista_new atrapa;
    Lista_new *poprz_w = NULL;
    Lista_new *akt_w = &atrapa;
    Lista_new *poprz_k = NULL;
    Lista_new *akt_k = NULL;
    int pierwszy = 1;
    int w = 0;
    int i = *j;
    int k;
    while(!(input[i] == -1*'/' && input[i+1] == -1*'\n')){
        if(input[i] == -1*'/') {
            i++;
            poprz_w = akt_w;
            w = input[i];
            pierwszy = 1;
        }
        else if(input[i] != -1*' ' && input[i] != -1*'\n') {
            k = input[i];
            akt_k = malloc(sizeof(Lista_new));
            akt_k->nast_k = NULL;
            akt_k->nast_w = NULL;
            akt_k->poprz_k = NULL;
            akt_k->poprz_w = NULL;
            if(pierwszy) {
                poprz_k = NULL;
                poprz_w->nast_w = akt_k;
                akt_w = akt_k;
                akt_w->poprz_w = poprz_w;

            }
            else {
                poprz_k->nast_k = akt_k;
            }
            if(!pierwszy) {
                akt_k->nast_w = NULL;
                akt_k->poprz_w = NULL;
            }
            if(pierwszy) {
                pierwszy = 0;
            }
            akt_k->k = k;
            akt_k->w = w;
            akt_k->stan = 1;
            akt_k->nast_k = NULL;
            akt_k->poprz_k = poprz_k;
            poprz_k = akt_k;
        }
        i++;
    }
    akt_w->nast_w = NULL;
    atrapa.nast_w->poprz_w = NULL;
    *j = i + 2;
    return atrapa.nast_w;
}

void Wypisz_startowy_format(Lista_new *l) {
    while(l != NULL) {
        Lista_new *pom = l;
        int pierwszy = 1;
        while(pom != NULL) {
            if(pom->stan == 1) {
                if(pierwszy) {
                    pierwszy = 0;
                    printf("/%d %d", pom->w,pom->k);
                }
                else{
                    printf(" %d", pom->k);
                }
            }
            pom = pom->nast_k;
        }
        if(!pierwszy) {
            printf("\n");
        }
        l = l->nast_w;
    }
    printf("/\n");

}

void wypisz_okno(Lista_new *l, int w, int k, int akt_w) {
    int max_w = w + WIERSZE - 1;
    int max_k = k + KOLUMNY - 1;
    Lista_new *pom = NULL;
    int akt_k = k;
    if(akt_w <= max_w){
        if(l != NULL) {
            if(l->w == akt_w) {
                pom = l;
                while(pom != NULL && akt_k <= max_k) {
                    if(pom->k >= k) {
                        if(pom->k == akt_k) {
                            if(pom->stan == 1) {
                                printf("0");
                            }
                            else{
                                printf(".");
                            }
                            pom = pom->nast_k;
                        }
                        else{
                            printf(".");
                        }
                        akt_k++;
                    }
                    else {
                        pom = pom->nast_k;
                    }
                }
                if(pom == NULL) {
                    while(akt_k <= max_k) {
                        printf(".");
                        akt_k++;
                    }
                }
                printf("\n");
                wypisz_okno(l->nast_w,w,k,akt_w+1);
            }
            else if(l->w > akt_w){
                for(int i = k; i <= max_k; i++) {
                    printf(".");
                }
                printf("\n");
                wypisz_okno(l,w,k,akt_w+1);
            }
            else {
                wypisz_okno(l->nast_w,w,k,akt_w);
            }

        }
        else{
            for(int i = k; i <= max_k; i++) {
            printf(".");
            }
            printf("\n");
            wypisz_okno(l,w,k,akt_w+1);
        }
    }
    else {
        for(int i = k; i <= max_k; i++) {
        printf("=");
        }
        printf("\n");
    }
}

void sprawdzenie_sasiadow_wyzej_nizej(Lista_new *akt, Lista_new *poprz) {
    while(poprz != NULL && poprz->k < akt->k - 1) {
        poprz = poprz->nast_k;
    }
    if(poprz != NULL) {
                if(poprz->k >= akt->k - 1 && poprz->k <= akt->k + 1) {
                    if(poprz->stan == 1) {
                        akt->liczba_sasiadow++;
                    }
                }
                if(poprz->nast_k != NULL) {
                    if(poprz->nast_k->k >= akt->k - 1 && poprz->nast_k->k <= akt->k + 1) {
                        if(poprz->nast_k->stan == 1) {
                            akt->liczba_sasiadow++;
                        }
                    }
                    if(poprz->nast_k->nast_k != NULL) {
                        if(poprz->nast_k->nast_k->k >= akt->k - 1 && poprz->nast_k->nast_k->k <= akt->k + 1) {
                            if(poprz->nast_k->nast_k->stan == 1) {
                                akt->liczba_sasiadow++;
                            }
                        }
                    }
                }

            }
}

void Uzupelnij_liczbe_sasiadow(Lista_new *l) {
    if(l != NULL) {
        Lista_new *poprz = l->poprz_w;
        Lista_new *nast = l->nast_w;
        Lista_new *akt = l;
        while(akt != NULL) {
            akt->liczba_sasiadow = 0;
            sprawdzenie_sasiadow_wyzej_nizej(akt,poprz);
            if(akt->poprz_k != NULL && akt->poprz_k->k == akt->k - 1 && akt->poprz_k->stan == 1) {
                akt->liczba_sasiadow++;
            }
            if(akt->nast_k != NULL && akt->nast_k->k == akt->k + 1 && akt->nast_k->stan == 1) {
                akt->liczba_sasiadow++;
            }
            sprawdzenie_sasiadow_wyzej_nizej(akt,nast);
            akt = akt->nast_k;
        }
        Uzupelnij_liczbe_sasiadow(l->nast_w);
    }
}

void Zmien_stan(Lista_new *l) {
    while(l != NULL) {
        Lista_new *pom = l;
        while(pom != NULL) {
            if(pom->stan == 0) {
                if(pom->liczba_sasiadow == 3) {
                    pom->stan = 1;
                }
            }
            else if(pom->liczba_sasiadow != 2 && pom->liczba_sasiadow != 3) {
                pom->stan = 0;
            }
            pom = pom->nast_k;
        }
        l = l->nast_w;
    }
}

Lista_new * Usun_niepotrzebnych_sasiadow(Lista_new *l) {
    Lista_new *zap = NULL;
    while(l != NULL) {
        Lista_new *pom = l;
        Lista_new *nastepny = l->nast_w;
        while(pom != NULL && (pom->poprz_k != NULL || pom->nast_k != NULL)) {
            if(pom->stan == 0 && pom->liczba_sasiadow == 0) {
                Lista_new *poprz = pom->poprz_k;
                Lista_new *nast = pom->nast_k;
                if(poprz == NULL) {
                        nast->poprz_k = poprz;
                        nast->nast_w = pom->nast_w;
                        nast->poprz_w = pom->poprz_w;
                        if(pom->poprz_w != NULL) {
                        pom->poprz_w->nast_w = nast;
                        }
                        if(pom->nast_w != NULL) {
                            pom->nast_w->poprz_w = nast;
                        }
                        free(pom);
                        pom = NULL;
                        pom = nast;
                }
                else {
                    poprz->nast_k = nast;
                    if(nast != NULL) {
                        nast->poprz_k = poprz;
                    }
                    free(pom);
                    pom = NULL;
                    pom = nast;
                }
            }
            else {
                if(zap == NULL) {
                    zap = pom;
                }
                pom = pom->nast_k;
            }
        }
            l = nastepny;
    }
    while(zap != NULL && zap->poprz_k != NULL) {
        zap = zap->poprz_k;
    }
    while(zap != NULL && zap->poprz_w != NULL) {
        zap = zap->poprz_w;
    }
    return zap;
}

Lista_new * Usun_niepotrzebnych_sasiadow_v2(Lista_new *l) {
    Lista_new *zap = NULL;
    while(l != NULL) {
        Lista_new *pom = l;
        Lista_new *nastepny = l->nast_w;
        while(pom != NULL) {
            if(pom->stan == 0 && pom->liczba_sasiadow == 0) {
                Lista_new *poprz = pom->poprz_k;
                Lista_new *nast = pom->nast_k;
                if(poprz == NULL) {
                        if(nast != NULL) {
                            nast->poprz_k = NULL;
                            nast->nast_w = pom->nast_w;
                            nast->poprz_w = pom->poprz_w;
                            if(pom->poprz_w != NULL) {
                                pom->poprz_w->nast_w = nast;
                            }
                            if(pom->nast_w != NULL) {
                                pom->nast_w->poprz_w = nast;
                            }
                            free(pom);
                            pom = NULL;
                        }
                        else {
                            Lista_new *pop_w = pom->poprz_w;
                            Lista_new *nast_w = pom->nast_w;
                            if(pop_w != NULL) {
                                pop_w->nast_w = nast_w;
                            }
                            if(nast_w != NULL) {
                                nast_w->poprz_w = pop_w;
                            }
                        free(pom);
                        }
                        pom = nast;
                }
                else {
                    poprz->nast_k = nast;
                    if(nast != NULL) {
                        nast->poprz_k = poprz;
                    }
                    free(pom);
                    pom = NULL;
                    pom = nast;
                }
            }
            else {
                if(zap == NULL) {
                    zap = pom;
                }
                pom = pom->nast_k;
            }
        }
            l = nastepny;
    }
    while(zap != NULL && zap->poprz_k != NULL) {
        zap = zap->poprz_k;
    }
    while(zap != NULL && zap->poprz_w != NULL) {
        zap = zap->poprz_w;
    }
    return zap;
}

/*Lista_new * Generacja(int n, Lista_new *Plansza, int w, int k) {
    if(n > 0) {
        Dodaj_niezywych_sasiadow_v2(Plansza);
        while(Plansza->poprz_w != NULL) {
            Plansza = Plansza->poprz_w;
        }
        Uzupelnij_liczbe_sasiadow(Plansza);
        Zmien_stan(Plansza);
        Plansza = Usun_niepotrzebnych_sasiadow(Plansza);
        Generacja(n-1,Plansza,w,k);
    }
    else {
        wypisz_okno(Plansza,w,k,w);
    }
    if(n == 0) {
        return Plansza;
    }

}*/

Lista_new * Generacja(int n, Lista_new *Plansza,int w, int k, int *generacja) {
    while(n > 0){
        Dodaj_niezywych_sasiadow_v2(Plansza);
        while(Plansza->poprz_k != NULL) {
            Plansza = Plansza->poprz_k;
        }
        while(Plansza->poprz_w != NULL) {
            Plansza = Plansza->poprz_w;
        }
        Uzupelnij_liczbe_sasiadow(Plansza);
        Zmien_stan(Plansza);
        Plansza = Usun_niepotrzebnych_sasiadow_v2(Plansza);
        n--;
    }
    (*generacja)++;
        wypisz_okno(Plansza,w,k,w);
        return Plansza;
}



void symulacja() {
    int *input;
    int n;
    int i = 0;
    int generacja = 0;
    int okno_w = 1;
    int okno_k = 1;
    czytaj_input(&input,&n);
    Lista_new *Plansza = Wpisz_Plansze_poczatkowa_v2(input,&i);
    wypisz_okno(Plansza,1,1,1);

    while(input[i] != -1*'.') {
        if(input[i+1] == -1*' ') {
            okno_w = input[i];
            okno_k = input[i+2];
            i += 4;
            wypisz_okno(Plansza,okno_w,okno_k,okno_w);
        }
        else if(input[i] == 0) {
            Wypisz_startowy_format(Plansza);
            i += 2;
            wypisz_okno(Plansza,okno_w,okno_k,okno_w);
        }
        else if(input[i] == -1*'\n') {
            Plansza = Generacja(1,Plansza,okno_w,okno_k,&generacja);
            i++;
        }
        else {
            Plansza = Generacja(input[i],Plansza,okno_w,okno_k,&generacja);
            i += 2;
        }
    }
    zwolnij_liste(Plansza);
    free(input);
}



int main()
{

    symulacja();

    return 0;
}
