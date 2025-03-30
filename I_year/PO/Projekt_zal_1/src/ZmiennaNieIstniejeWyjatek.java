public class ZmiennaNieIstniejeWyjatek extends Exception{
    private char a;
    public ZmiennaNieIstniejeWyjatek(char a) {
        this.a = a;
    }
    public String Message() {
        return "Zmienna: " + this.a + " nie istnieje";
    }
}
