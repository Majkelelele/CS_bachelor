import java.util.ArrayList;

public class Program {

    private Blok blok;


    public Program(Blok blok) {
        this.blok = blok;
    }

    public void wykonaj() {
        int[] array = {-1,0};
        this.blok.odpal(new Zmienna[26],array,new ArrayList<>());
        this.blok.wypiszZmienne();
    }

    //mozliwe jest wypisanie wartosciowania bloku glownego po zakonczeniu dzialania program: "d 0"
    public void wykonajDebugger() {
        int[] array = {0,0};
        this.blok.odpal(new Zmienna[26],array,new ArrayList<>());
        this.blok.wypiszZmienne();
        System.out.println("Mozesz jescze zobaczyc zmienne obecnego bloku! Aby zakonczyc uzyj jednej z komend: c, s, e");
        this.blok.wczytajPolecenia(array,this.blok.zwrocListeZmiennych());
    }
}
