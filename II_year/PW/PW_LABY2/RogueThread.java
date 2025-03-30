
public class RogueThread {

    private static class Rogue implements Runnable {

        @Override
        public void run() {
            while (true) {
                System.out.println("Never gonna stop!");
               try {
                if(Thread.interrupted()) return;
                Thread.sleep(200);
            }    catch (InterruptedException e) {
                    
                }
            }
        }

    }

    public static void main(String[] args) {
        Thread t = new Thread(new Rogue());
        t.start();
        try {
            Thread.sleep(2000);
        } catch (InterruptedException e) {
            Thread.currentThread().interrupt();
            System.out.println("Main thread interrupted during sleep");
        }
        System.err.println("Trying to stop the bad guy");
        t.interrupt();
        System.err.println("Oops, it didn't work...");
    }
}
