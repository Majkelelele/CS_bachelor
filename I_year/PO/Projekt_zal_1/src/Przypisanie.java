import java.util.ArrayList;

public class Przypisanie extends Instrukcja{

    final private char nazwa;

    final private Wyrazenie exp;

    public Przypisanie(char nazwa, Wyrazenie wyr) {
        this.exp = wyr;
        this.nazwa = nazwa;
    }

    @Override
    public void odpal(Zmienna[] zmienne, int[] array, ArrayList<Zmienna[]> listaZmiennych) {
        if(array[0] == array[1]) {
            this.drukujInstrukcje();
            this.wczytajPolecenia(array,listaZmiennych);
        }
        array[1]++;
        try {
            int val = this.exp.zwrocWartosc(zmienne);
            zmienne[(int) this.nazwa - 97].UstawWartosc(val);
        }
        catch (ArithmeticException e) {
            System.out.println("Blad wyliczenia wartosci wyrazenia: " + e);
        }
        catch (ArrayIndexOutOfBoundsException e) {
            System.out.println("Nazwa zmiennej bledna " + e);
        }
        catch (NullPointerException e) {
            System.out.println("Zmienna: " + this.nazwa + " nie istnieje " + e);
        }
        catch (ZmiennaNieIstniejeWyjatek e) {
            System.out.println(e.Message());
            this.drukujInstrukcje();
            System.exit(-1);
        }
        catch (Exception e) {
            this.nastapilBlad(listaZmiennych);
        }
    }
    @Override
    public void drukujInstrukcje() {
       System.out.println("Obecna instrukcja to: " + this.drukujNazweKlasy() + " " + this.nazwa + " := " + this.exp.wypisz());
    }
}
