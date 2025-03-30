public class Main {
 //przykladowy program napisany przy pomocy konstruktorow.
    public static void main(String[] args) {
     Blok blokGlowny = new Blok();

     Stala trzydziesci = new Stala(30);

     Deklaracja dek1 = new Deklaracja(trzydziesci,'n');

     blokGlowny.dodajDeklaracje(dek1);

     Zmienna k = new Zmienna(0,'k');

     Stala Jeden = new Stala(1);

     Odejmowanie od = new Odejmowanie(new Zmienna(0,'n'),Jeden);

     PetlaFor FOR = new PetlaFor(k,od);

     Blok blok1 = new Blok();

     Deklaracja dek_p = new Deklaracja(Jeden,'p');

     blok1.dodajDeklaracje(dek_p);

     Stala dwa = new Stala(2);

     Przypisanie przyp_k = new Przypisanie('k',new Dodawanie(new Zmienna(0,'k'),dwa));

     blok1.dodajInstrukcje(przyp_k);

     Zmienna i = new Zmienna(0,'i');

     PetlaFor FOR_1 = new PetlaFor(i,new Odejmowanie(new Zmienna(0,'k'),dwa));

     blok1.dodajInstrukcje(FOR_1);

     Przypisanie przyp_i = new Przypisanie('i',new Dodawanie(new Zmienna(0,'i'),dwa));

     FOR_1.dodajInstrukcje(przyp_i);

     Stala zero = new Stala(0);

     Przypisanie przp_p = new Przypisanie('p',zero);

     Modulo mod = new Modulo(k,i);

     InstrukcjaWarunkowa IF = new InstrukcjaWarunkowa(mod,zero,"=");

     IF.dodajInstrukcje(przp_p);

     FOR_1.dodajInstrukcje(IF);

     InstrukcjaWarunkowa IF_1 = new InstrukcjaWarunkowa(new Zmienna(0,'p'), Jeden,"=");

     blok1.dodajInstrukcje(IF_1);

     Wypisanie wyp = new Wypisanie(k);

     IF_1.dodajInstrukcje(wyp);

     FOR.dodajInstrukcje(blok1);

     blokGlowny.dodajInstrukcje(FOR);




     Program program = new Program(blokGlowny);

     //zrobienie program.Wykonaj(); powoduje wywolanie bez debuggera.
     program.wykonajDebugger();
    }
}
