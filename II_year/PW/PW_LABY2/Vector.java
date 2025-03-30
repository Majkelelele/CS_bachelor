import java.util.Arrays;
import java.util.LinkedList;
import java.util.Random;

public class Vector {

    private static final int SUM_CHUNK_LENGTH = 10;
    private static final int DOT_CHUNK_LENGTH = 10;


    private final int[] elements;
    
    public Vector(int length) {
        this.elements = new int[length];
    }

    public Vector(int[] elements) {
        this.elements = Arrays.copyOf(elements, elements.length);
    }
    
    public int getValue(int i) {
        return elements[i];
    }
    public void setValue(int i, int value) {
        elements[i] = value;
    } 
    public int getSize() {
        return this.elements.length;
    } 

    public Vector sum(Vector other) throws InterruptedException {
        if (this.elements.length != other.elements.length) {
            throw new IllegalArgumentException("different lengths of summed vectors");
        }
        Vector result = new Vector(this.elements.length);
        LinkedList<Thread> threads = new LinkedList<>();
        for(int i = 0; i < this.elements.length; i = i+10) {
            Runnable r = new Summer(this, other, i, Math.min(SUM_CHUNK_LENGTH, this.elements.length-i), result);
            Thread t = new Thread(r);
            t.start();
            threads.push(t);
        }
        for(Thread t : threads) {
             try {
                t.join();
            }
            catch (InterruptedException e) {
                for(Thread T: threads) {
                    T.interrupt();
                }
            }
        }

        return result;
    }

    private static class Summer implements Runnable {

        private final Vector leftVec;
        private final Vector rightVec;
        private final int startPosIncl;
        private final int len;
        private Vector resVec;

        public Summer(Vector leftVec, Vector rightVec, int startPosIncl, int len, Vector resVec) {
            this.resVec = resVec;
            this.leftVec = leftVec;
            this.rightVec = rightVec;
            this.startPosIncl = startPosIncl;
            this.len = len;
        }

        @Override
        public void run() {
            for(int i = 0; i < len; i++) {
                int position = i + startPosIncl;
                int value = leftVec.getValue(position) + rightVec.getValue(position);
                resVec.setValue(position, value);
                if (Thread.interrupted()) {
                    System.out.println("Thread: " + Thread.currentThread().getName() + " interupted");
                    return;
                }
            }
        }

    }

    public int dot(Vector other) throws InterruptedException {
        if (this.elements.length != other.elements.length) {
            throw new IllegalArgumentException("different lengths of dotted vectors");
        }
        int result = 0;
        int j = 0;
        int[] resultTab = new int[(elements.length + DOT_CHUNK_LENGTH - 1)/DOT_CHUNK_LENGTH];
        LinkedList<Thread> threads = new LinkedList<>();
        for(int i = 0; i < this.elements.length; i = i+10) {
            Runnable r = new Dotter(other,this,i,Math.min(DOT_CHUNK_LENGTH, this.elements.length-i),resultTab,j);
            Thread t = new Thread(r);
            t.start();
            threads.push(t);
            j++;            
        }

        for(Thread t : threads) {   
            try {
                t.join();
            }
            catch (InterruptedException e) {
                for(Thread T: threads) {
                    T.interrupt();
                }
            }
        }
        for(int i = 0; i < resultTab.length; i++) result += resultTab[i];
        return result;
    }

    private static class Dotter implements Runnable {

        private final Vector leftVec;
        private final Vector rightVec;
        private final int startPosInc;
        private final int len;
        private final int[] resVec;
        private final int resPos;
        

        public Dotter(Vector leftVec, Vector rightVect, int startPosInc, int len, int[] resVec, int resPos) {
            this.leftVec = leftVec;
            this.rightVec = rightVect;
            this.startPosInc = startPosInc;
            this.len = len;
            this.resVec = resVec;
            this.resPos = resPos;
            
        }

        @Override
        public void run() {
            int sum = 0;
             for(int i = 0; i < len; i++) {
                int position = i + this.startPosInc;
                sum += leftVec.getValue(position)*rightVec.getValue(position);
                if (Thread.interrupted()) {
                    System.out.println("Thread: " + Thread.currentThread().getName() + " interupted");
                    return;
                }
            }       
            resVec[resPos] = sum;
        }
    }

    @Override
    public boolean equals(Object obj) {
        if (! (obj instanceof Vector)) {
            return false;
        }
        Vector other = (Vector)obj;
        return Arrays.equals(this.elements, other.elements);
    }

    @Override
    public int hashCode() {
        return Arrays.hashCode(this.elements);
    }

    @Override
    public String toString() {
        StringBuilder s = new StringBuilder("[");
        for (int i = 0; i < this.elements.length; ++i) {
            if (i > 0) {
                s.append(", ");
            }
            s.append(this.elements[i]);
        }
        s.append("]");
        return s.toString();
    }

    // ----------------------- TESTS -----------------------
    
    private static final Random RANDOM = new Random();

    private static Vector generateRandomVector(int length) {
        int[] a = new int[length];
        for (int i = 0; i < length; ++i) {
            a[i] = RANDOM.nextInt(10);
        }
        return new Vector(a);
    }

    private final Vector sumSequential(Vector other) {
        if (this.elements.length != other.elements.length) {
            throw new IllegalArgumentException("different lengths of summed vectors");
        }
        Vector result = new Vector(this.elements.length);
        for (int i = 0; i < result.elements.length; ++i) {
            result.elements[i] = this.elements[i] + other.elements[i];
        }
        return result;
    }
    
    private final int dotSequential(Vector other) {
        if (this.elements.length != other.elements.length) {
            throw new IllegalArgumentException("different lengths of dotted vectors");
        }
        int result = 0;
        for (int i = 0; i < this.elements.length; ++i) {
            result += this.elements[i] * other.elements[i];
        }
        return result;
    }

    public static void main(String[] args) {
        try {
            Vector a = generateRandomVector(33);
            System.out.println(a);
            Vector b = generateRandomVector(33);
            System.out.println(b);
            Vector c = a.sum(b);
            System.out.println(c);
            assert(c.equals(a.sumSequential(b)));
            int d = a.dot(b);
            System.out.println(d);
            System.out.println(a.dotSequential(b));
            assert(d == a.dotSequential(b));
        } catch (InterruptedException e) {
            Thread.currentThread().interrupt();
            System.err.println("computations interrupted");
        }
    }

}