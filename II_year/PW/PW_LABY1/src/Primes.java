package PW_LABY1.src;

public class Primes {

    private static final int MAX_NUMBER_TO_CHECK = 10000;

    private static volatile boolean isPrime;


    private static class FindDivisor implements Runnable {

        private final int n;
        private final int start;
        private final int step;
        private final int end;

        public FindDivisor(int n, int start, int step, int end) {
            this.n = n;
            this.start = start;
            this.step = step;
            this.end = end;
        }

        @Override
        public void run() {
            int current = start;
            while(current < end && isPrime) {
                if(n%current==0){
                    isPrime = false;
                }
                current += step;
            }
        }
    }

    public static boolean isPrime(int n) {
        isPrime = true;
        if(n < 30) {
            isPrime = n==2 || n==3 || n==5 || n==7 || n==11 || n==13 || n==17 || n==19 || n==23 || n==29;
        }
        else {
            isPrime = n%2!=0 && n%3!=0 && n%5!=0 && n%7!=0 && n%11!=0 && n%13!=0 && n%17!=0
                    && n%19!=0 && n%23!=0 && n%29!=0;
            if(isPrime) {
                int[] nums = {31,37,41,43,47,49,53,59};
                Thread[] threads = new Thread[8];
                for(int i = 0; i < 8; i++) {
                    Runnable r = new FindDivisor(n,nums[i],30,n);
                    Thread t = new Thread(r, String.valueOf(i));
                    t.start();
                    // threads[i] = t;
                    try{
                        t.join();
                    }
                    catch(InterruptedException e) {
                        
                    }
                    
                }
                // for(Thread thread : threads) {
                //     while(thread.isAlive()) {

                //     }
                // }
            }
        }
        return isPrime;
    }

    public static void main(String[] args) {
        // A sample test.
        int primesCount = 0;
        for (int i = 2; i <= MAX_NUMBER_TO_CHECK; ++i) {
            if (isPrime(i)) {
                ++primesCount;
            }
        }
        System.out.println(primesCount);
        assert(primesCount == 1229);
    }

}