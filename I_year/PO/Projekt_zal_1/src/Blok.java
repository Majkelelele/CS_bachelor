import java.util.ArrayList;

public class Blok extends Instrukcja{

    private ArrayList<Instrukcja> instrukcje = new ArrayList<>();

    //kazda zmienna w bloku lezy na swoim odpowiednim miejscu -> a - 0, b - 1, .. z - 25
    //miejsca odpowiadaja wartosci (int) 'nazwa zmiennej' - 97;
    private Zmienna[] zmienne;
    private ArrayList<Deklaracja> deklaracje = new ArrayList<>();

    public ArrayList<Zmienna[]> listaZmiennych = new ArrayList<>();

    public Boolean zadeklarowane;


    public Blok() {
        this.zadeklarowane = false;
        this.zmienne = new Zmienna[26];
    }


    @Override
    public void drukujInstrukcje() {
        System.out.println("Obecna instrukcja to: " + this.drukujNazweKlasy());
        System.out.println("Jej instrukcje to: ");
        for(Instrukcja instrukcja: this.instrukcje) {
           System.out.println(instrukcja.drukujNazweKlasy());
        }
    }
    //podczas deklaracji zmiennej dodawana jest automatycznie instrukcja przypisania ktora przypisuje dana zmienna na wartosc podana w deklaracji.
    //jest to konieczne aby wracac do zadeklarowanej wartosci przy petli for, omijajac ponowne deklaracje.
    public void odpalDeklaracje(int[] array, ArrayList<Zmienna[]> listaZmiennych) {
        for (Deklaracja deklaracja : this.deklaracje) {
            try{
                deklaracja.Deklaruj(this.zmienne,array,listaZmiennych);
            }
            catch (ZmiennaJuzIstniejeWyjatek e) {
                System.out.println("Zmienna '" + deklaracja.zwrocNazwe() + "' juz istnieje" + e);
            }
        }
        this.zadeklarowane = true;
    }

    public void odpal(Zmienna[] zmienne, int[] array, ArrayList<Zmienna[]> listaZmiennych){
        if(array[0] == array[1]) {
            this.drukujInstrukcje();
            this.wczytajPolecenia(array,this.listaZmiennych);
        }
        array[1]++;
        this.zarzadDeklaracjami(array,listaZmiennych);
        this.dziedziczZmienne(zmienne);
        this.listaZmiennych = this.kopiujListeZmiennych(listaZmiennych);
        this.listaZmiennych.add(0,this.zmienne);
       for(int i = 0; i < this.instrukcje.size(); i++) {
           this.instrukcje.get(i).odpal(this.zmienne,array,this.listaZmiennych);
       }
    }

    public void zarzadDeklaracjami(int[] array, ArrayList<Zmienna[]> listaZmiennych) {
        if(this.zadeklarowane == false) {
            this.odpalDeklaracje(array,listaZmiennych);
        }
        else { //w petli for, sprowadza wartosci zmiennych do tych zadeklarowanych, bez ponownej deklaracji.
            for (Deklaracja deklaracja: this.deklaracje) {
                try {
                    this.zmienne[(int) deklaracja.zwrocNazwe() - 97].UstawWartosc(deklaracja.zwrocWyrazenie().zwrocWartosc(zmienne));
                }
                catch (ZmiennaNieIstniejeWyjatek e) {
                    //nic tu sie nie stanie poniewaz juz wczesniej obliczalismy wartosc wyrazenia przy deklaracji zmiennej
                }
            }
        }
    }

    public ArrayList<Instrukcja> zwrocInstrukcje() {
        return this.instrukcje;
    }

    public void dodajDeklaracje(Deklaracja d) {
        this.deklaracje.add(d);
    }

    public void dodajInstrukcje(Instrukcja i) {
        this.instrukcje.add(i);
    }

    //metoda odpowiada za poprawne dziedziczenie zmiennych (zmienne) od bloku ojca.
    public void dziedziczZmienne(Zmienna[] zmienne) {
        for(int i = 0; i < zmienne.length; i++) {
            if(zmienne[i] != null && this.zmienne[i] == null) {
                this.zmienne[i] = zmienne[i];
            }
        }
    }
    public ArrayList<Zmienna[]> zwrocListeZmiennych() {
        return this.listaZmiennych;
    }
    public void wypiszZmienne() {
        for(Zmienna zmienna: this.zmienne) {
            if(zmienna != null) {
                System.out.println("Wartosc zmiennej: " + zmienna.zwrocNazwe() + " to " + zmienna.Wartosc());
            }
        }
    }


}
