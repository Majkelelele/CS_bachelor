package PW_LABY1.src;

// import java.util.concurrent.ThreadLocalRandom;

public class manyThreads {
    public static final int threadsCount = 100;
    public static int currentCount = 0;

    private static class NewThread implements Runnable {


        @Override
        public void run() {
            currentCount++;
            if(currentCount <= threadsCount) {
                Runnable r = new NewThread();
                Thread t = new Thread(r,String.valueOf(currentCount));
                t.start();
                int i = 0;
                while(i < 100000) {
                    i++;
                }
                System.out.println(Thread.currentThread().getName());
            }
        }
    }


    public static void main(String args[]) {
        Runnable r = new NewThread();
        Thread t = new Thread(r,String.valueOf(currentCount));
        System.out.println(Thread.currentThread().getName());
        t.start();
    }
}
