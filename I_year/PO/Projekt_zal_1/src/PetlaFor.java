import java.util.ArrayList;

public class PetlaFor extends Blok{

    private final Wyrazenie exp;
    public final Zmienna zm;

    private ArrayList<Deklaracja> deklaracje = new ArrayList<>();

    private Zmienna[] zmienne;

    public PetlaFor(Zmienna zm, Wyrazenie exp) {
        this.zm = zm;
        this.exp = exp;
        this.zmienne = new Zmienna[26];
        this.zmienne[(int) this.zm.zwrocNazwe() - 97] = this.zm;

    }

    @Override
    public void odpal(Zmienna[] zmienne, int[] array, ArrayList<Zmienna[]> listaZmiennych) {
        int x = 0;
        try {
            x = this.exp.zwrocWartosc(zmienne);
        }
        catch (ArithmeticException e) {
            System.out.println("Blad wyliczenia wyraznie w petli for " + e);
            this.nastapilBlad(listaZmiennych);
        }
        catch (ZmiennaNieIstniejeWyjatek e) {
            System.out.println(e.Message());
            this.drukujInstrukcje();
            System.exit(-1);
        }
        this.dziedziczZmienne(zmienne);
        this.listaZmiennych = this.kopiujListeZmiennych(listaZmiennych);
        this.listaZmiennych.add(0,this.zmienne);
        if(array[0] == array[1]) {
            this.drukujInstrukcje();
            this.wczytajPolecenia(array,this.listaZmiennych);
        }
        array[1]++;
        for(int i = 0; i < x; i++) {
            this.zm.UstawWartosc(i);
            for(Instrukcja instrukcja: this.zwrocInstrukcje()) {
                instrukcja.odpal(this.zmienne,array,this.listaZmiennych);
            }
        }
    }
    @Override
    public void dziedziczZmienne(Zmienna[] zmienne) {
        for(int i = 0; i < zmienne.length; i++) {
            if(zmienne[i] != null && this.zmienne[i] == null) {
                this.zmienne[i] = zmienne[i];
            }
        }
    }
    @Override
    public void drukujInstrukcje() {
        System.out.println("Obecna instrukcja to: " + this.drukujNazweKlasy() + " " + this.zm.wypisz() + ": " + this.exp.wypisz());
    }
}

