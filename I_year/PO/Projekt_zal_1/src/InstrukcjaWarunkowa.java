import java.util.ArrayList;

public class InstrukcjaWarunkowa extends Instrukcja{
    final private Wyrazenie exp1;

    final private Wyrazenie exp2;

    private ArrayList<Instrukcja> instrukcje = new ArrayList<>();

    final private String znak;

    public InstrukcjaWarunkowa(Wyrazenie exp1, Wyrazenie exp2, String znak) {
        this.exp1 = exp1;
        this.exp2 = exp2;
        this.znak = znak;
    }
    @Override
    public void odpal(Zmienna[] zmienne, int[] array, ArrayList<Zmienna[]> listaZmiennych) {
        if(array[0] == array[1]) {
            this.drukujInstrukcje();
            this.wczytajPolecenia(array,listaZmiennych);
        }
        array[1]++;
        int x = 0;
        int y = 0;
        try {
            x = this.exp1.zwrocWartosc(zmienne);
            y = this.exp2.zwrocWartosc(zmienne);
        }
        catch (ArithmeticException e) {
            System.out.println("Blad wyliczenia wyrazenia " + e);
            this.nastapilBlad(listaZmiennych);
        }
        catch (ZmiennaNieIstniejeWyjatek e) {
            System.out.println(e.Message());
            this.drukujInstrukcje();
            System.exit(-1);
        }
        if(sprawdzWarunek(x,y)) {
            for(int i = 0; i < this.instrukcje.size(); i++) {
                this.instrukcje.get(i).odpal(zmienne,array,listaZmiennych);
            }
        }
    }
    @Override
    public void drukujInstrukcje() {
       System.out.println("Obecna instrukcja to: " + this.drukujNazweKlasy() + ": (" + this.exp1.wypisz() + this.znak + this.exp2.wypisz() + ")");
    }
    public Boolean sprawdzWarunek(int x, int y) {
        switch (this.znak) {
            case "=": return x == y;
            case "<": return x < y;
            case ">": return x > y;
            case "<=": return x <= y;
            case ">=": return x >= y;
            default: return x != y;
        }
    }
    public void dodajInstrukcje(Instrukcja i) {
        this.instrukcje.add(i);
    }
}
