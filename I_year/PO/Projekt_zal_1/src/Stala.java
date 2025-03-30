public class Stala extends Wyrazenie {

    private final int val;

    public Stala(int val) {
        this.val = val;
    }
    @Override
    public int zwrocWartosc(Zmienna[] zmienne) {
        return this.val;
    }

    @Override
    public String wypisz() {
        return String.valueOf(this.val);
    }
}
