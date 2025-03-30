public class Odejmowanie extends Wyrazenie{

    private final Wyrazenie exp1;
    private final Wyrazenie exp2;

    Odejmowanie(Wyrazenie exp1, Wyrazenie exp2) {
        this.exp1 = exp1;
        this.exp2 = exp2;
    }
    @Override
    public int zwrocWartosc(Zmienna[] zmienne) {
        int x = 0;
        try {
            x = this.exp1.zwrocWartosc(zmienne) - this.exp2.zwrocWartosc(zmienne);
        }
        catch(ZmiennaNieIstniejeWyjatek e) {
            System.out.println(e.Message());
            System.out.println("Blad we wyrazeniu: " + this.wypisz());
            System.exit(-1);
        }
        return x;
    }

    @Override
    public String wypisz() {
        return this.exp1.wypisz() + "-" + this.exp2.wypisz();
    }
}
