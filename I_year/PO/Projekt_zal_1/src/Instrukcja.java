import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.Scanner;

public abstract class Instrukcja {

    public abstract void odpal(Zmienna[] zmienne, int [] array, ArrayList<Zmienna[]> listaZmiennych);

    //metoda wczytuje polecenie debuggera oraz podejmuje odpowiednie dzialanie wzgledem komendy.
    public void wczytajPolecenia(int[] array, ArrayList<Zmienna[]> listaZmiennych) {
        System.out.println("Podaj komende debugerra: ");
        Scanner scanner = new Scanner(System.in);
        String[] tokens = scanner.nextLine().split(" ");
        List<String> lista = Arrays.asList(tokens);
        String  ilosc = "0";
        String znak = "0";
        for(int i = 0; i < lista.size(); i++) {
            if(!lista.get(i).equals(" ")) {
                if(znak.equals("0")) {
                    znak = lista.get(i);
                }
                else {
                    ilosc = lista.get(i);
                }
            }
        }
        if(Integer.parseInt(ilosc) < 0) {
            System.out.println("Podaj poprawna instukcje debuggera: "); {
                this.wczytajPolecenia(array,listaZmiennych);
            }
        }
        else {
            switch (znak) {
                case "c":
                    array[0] = -1;
                    array[1] = 1;
                    break;
                case "s":
                    if(Integer.parseInt(ilosc) == 0) {
                        System.out.println("Wraz z komenda 'step', podaj dodatnia liczbe calkowita");
                        this.wczytajPolecenia(array,listaZmiennych);
                    }
                    else {
                        array[0] = Integer.parseInt(ilosc);
                        array[1] = 0;
                        break;
                    }
                case "d":
                    wypiszWartosciowanie(listaZmiennych,Integer.parseInt(ilosc));
                    System.out.println("Podaj kolejna komende: ");
                    this.wczytajPolecenia(array,listaZmiennych);
                    break;
                case "e":
                    System.exit(0);
                default:
                    System.out.println("Zla komenda. Podaj nowa");
                    this.wczytajPolecenia(array,listaZmiennych);
            }
        }
    }

    public String drukujNazweKlasy() {
        return this.getClass().getSimpleName();
    }

    public abstract void drukujInstrukcje();

    public void wypiszWartosciowanie(ArrayList<Zmienna[]> listaZmiennych, int x) {
        if(listaZmiennych.size() <= x) {
            System.out.println("Podana liczba jest za duza!");
        }
        else {
            for(Zmienna z: listaZmiennych.get(x)) {
                if(z != null) {
                    System.out.println("Nazwa zmiennej: " + z.zwrocNazwe());
                    System.out.println("Wartosc zmiennej: " + z.Wartosc());
                }
            }
        }
    }

    //metoda kopiuje liste tablic zmiennych.
     public ArrayList<Zmienna[]> kopiujListeZmiennych(ArrayList<Zmienna[]> listaZmiennych) {
         ArrayList<Zmienna[]> listaZmiennych_2 = new ArrayList<>();
         for (Zmienna[] zmienna : listaZmiennych) {
             Zmienna[] copy = Arrays.copyOf(zmienna, zmienna.length);
             listaZmiennych_2.add(copy);
         }
         return listaZmiennych_2;
     }

     public void nastapilBlad(ArrayList<Zmienna[]> listaZmiennych) {
        System.out.println("Zmienne obecnego bloku: ");
         this.wypiszWartosciowanie(listaZmiennych,0);
         System.out.println("Instrukcja ktora wywolala blad to:");
         this.drukujInstrukcje();
         System.exit(-1);
     }
}
