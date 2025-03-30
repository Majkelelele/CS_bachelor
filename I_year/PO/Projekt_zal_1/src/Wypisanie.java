import java.util.ArrayList;

public class Wypisanie extends Instrukcja{

    private final Wyrazenie wyr;

    public Wypisanie(Wyrazenie wyr) {
        this.wyr = wyr;
    }
    @Override
    public void odpal(Zmienna[] zmienne, int[] array, ArrayList<Zmienna[]> listaZmiennych) {
        if(array[0] == array[1]) {
            this.drukujInstrukcje();
            this.wczytajPolecenia(array,listaZmiennych);
        }
        array[1]++;
        int x = 0;
        try {
            x = this.wyr.zwrocWartosc(zmienne);
        }
        catch (ArithmeticException e) {
            this.nastapilBlad(listaZmiennych);
        }
        catch (ZmiennaNieIstniejeWyjatek e) {
            System.out.println(e.Message());
            this.drukujInstrukcje();
            System.exit(-1);
        }

            System.out.println("wartosc wyrazenia: " + x);
    }
    @Override
    public void drukujInstrukcje() {
        System.out.println("Obecna instrukcja to: " + this.drukujNazweKlasy() + " " + this.wyr.wypisz());
    }
}
