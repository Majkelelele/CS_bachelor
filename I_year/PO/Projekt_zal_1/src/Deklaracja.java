import java.util.ArrayList;

public class Deklaracja extends Instrukcja{
    private Wyrazenie w;
    private char nazwa;
    public Deklaracja(Wyrazenie w, char nazwa) {
        this.w = w;
        this.nazwa = nazwa;
    }

    @Override
    public void odpal(Zmienna[] zmienne, int[] array, ArrayList<Zmienna[]> listaZmiennych) {}

    public void Deklaruj(Zmienna[] zmienne, int[] array, ArrayList<Zmienna[]> listaZmiennych) throws ZmiennaJuzIstniejeWyjatek{
        if(array[0] == array[1]) {
            this.drukujInstrukcje();
            this.wczytajPolecenia(array,listaZmiennych);
        }
        array[1]++;
        if(zmienne[(int) nazwa -97] != null) {
            throw new ZmiennaJuzIstniejeWyjatek();
        }
        else {
            try {
                zmienne[(int) nazwa -97] = new Zmienna(this.w.zwrocWartosc(zmienne), this.nazwa);
            }
            catch (ArithmeticException e) {
                System.out.println("blad wyliczenia przy deklaracji zmiennej: '" + this.nazwa + "' " + e);
                System.exit(-1);
            }
            catch (ZmiennaNieIstniejeWyjatek e) {
                System.out.println("Blad w deklaracji");
                System.out.println(e.Message());
                System.exit(-1);
            }
        }
    }

    @Override
    public void drukujInstrukcje() {
        System.out.println("Obecny (ukryty) krok to deklaracja: " + this.nazwa + " = " + this.w.wypisz());
    }

    public char zwrocNazwe() {
        return this.nazwa;
    }
    public Wyrazenie zwrocWyrazenie() {
        return this.w;
    }

}
