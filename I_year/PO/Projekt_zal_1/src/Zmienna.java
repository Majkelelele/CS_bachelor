public class Zmienna extends Wyrazenie {
    private char nazwa;
    private int w;

    public Zmienna(int val, char name) {

            this.nazwa = name;
            this.w = val;

    }

    public int zwrocWartosc(Zmienna[] zmienne) throws ZmiennaNieIstniejeWyjatek{
        if(zmienne[(int) this.nazwa - 97] == null) {
            throw new ZmiennaNieIstniejeWyjatek(this.nazwa);
        }
        else {
            return zmienne[(int) this.nazwa - 97].Wartosc();
        }
    }

    public int Wartosc() {
        return this.w;
    }
    public char zwrocNazwe() {
        return this.nazwa;
    }
    public void UstawWartosc(int x) {
        this.w = x;
    }
    @Override
    public String wypisz() {
        return String.valueOf(this.nazwa);
    }
}
