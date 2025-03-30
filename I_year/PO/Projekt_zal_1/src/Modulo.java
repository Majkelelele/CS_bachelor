public class Modulo extends Wyrazenie{

    private final Wyrazenie exp1;
    private final Wyrazenie exp2;

    public Modulo(Wyrazenie w1, Wyrazenie w2) {
        this.exp1 = w1;
        this.exp2 = w2;
    }
    @Override
    public int zwrocWartosc(Zmienna[] zmienne) {
        int x = 0;
        int y = 0;
        try {
            x = this.exp1.zwrocWartosc(zmienne);
            y = this.exp2.zwrocWartosc(zmienne);
        }
        catch (ZmiennaNieIstniejeWyjatek e) {
            System.out.println(e.Message());
            System.out.println("Blad we wyrazeniu: " + this.wypisz());
            System.exit(-1);
        }
        int val = 0;
        if(y == 0) {
            throw new ArithmeticException();
        }
        else {
            val = x%y;
        }
        return val;
    }
    @Override
    public String wypisz() {
        return this.exp1.wypisz() + "%" + this.exp2.wypisz();
    }
}

