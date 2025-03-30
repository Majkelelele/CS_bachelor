import java.util.LinkedList;
import java.util.concurrent.BrokenBarrierException;
import java.util.concurrent.Callable;
import java.util.concurrent.CyclicBarrier;
import java.util.concurrent.Semaphore;
import java.util.function.IntBinaryOperator;

public class MatrixRowSums {

    private static final int ROWS = 3;
    private static final int COLUMNS = 10;
    
    private static final CyclicBarrier barrier = new CyclicBarrier(COLUMNS+1);
    private static Semaphore semafor = new Semaphore(1);
    private static volatile int res[] = new int[ROWS];

    private static class Matrix {

        private final int rows;
        private final int columns;
        private final IntBinaryOperator definition;

        public Matrix(int rows, int columns, IntBinaryOperator definition) {
            this.rows = rows;
            this.columns = columns;
            this.definition = definition;
        }

        public int[] rowSums() {
            int[] rowSums = new int[rows];
            for (int row = 0; row < rows; ++row) {
                int sum = 0;
                for (int column = 0; column < columns; ++column) {
                    sum += definition.applyAsInt(row, column);
                }
                rowSums[row] = sum;
            }
            return rowSums;
        }

        public int[] rowSumsConcurrent() throws Exception {
            
            LinkedList<Thread> threadsList = new LinkedList<>();
            for(int k = 0; k < COLUMNS; k++) {
                    Runnable r = new rowSummer(k);
                    Thread t = new Thread(r);
                    threadsList.push(t);
            }
            for(Thread t : threadsList) t.start();
            try {
                barrier.await();
            }
            catch (InterruptedException | BrokenBarrierException e) {
                Thread t = Thread.currentThread();
                t.interrupt();
                System.err.println(t.getName() + " interrupted");
            }
            return res;
        }
        private class rowSummer implements Runnable {
            private int columnNum;

            public rowSummer(int num) {
                this.columnNum = num;
            }

            @Override
            public void run() {
                for(int currentRow = 0; currentRow < ROWS; currentRow++) {
                    int result = definition.applyAsInt(currentRow,columnNum);
                    if(Thread.interrupted()) {
                        return;
                    }
                    try {
                        semafor.acquire();
                        res[currentRow] += result;
                        semafor.release();
                    }
                    catch (InterruptedException e) {
                        Thread t = Thread.currentThread();
                        t.interrupt();
                        System.err.println(t.getName() + " interrupted");
                    }
                }
                try {
                        barrier.await();
                    }
                    catch (InterruptedException | BrokenBarrierException e) {
                        Thread t = Thread.currentThread();
                        t.interrupt();
                        System.err.println(t.getName() + " interrupted");
                    }
                
               
            }   
        }
    }

    public static void main(String[] args) throws Exception {
        Matrix matrix = new Matrix(ROWS, COLUMNS, (row, column) -> {
            // long computations
            int a = 2 * column + 1;
            int cellId = column + row * COLUMNS;
            try {
                // different cells computations in rows takes different time to complete
                // and hence some thread will wait for others
                // but nevertheless there should be substantial gain from concurrent solutions
                Thread.sleep((1000 - (cellId % 13) * 1000 / 12));
            } catch (InterruptedException e) {
                Thread t = Thread.currentThread();
                t.interrupt();
                System.err.println(t.getName() + " interrupted");
            }
            return (row + 1) * (a % 4 - 2) * a;
        });

        timeIt("sequential execution", "should take about 17s to complete", () -> matrix.rowSums());

        // concurrent computations
        try {
            timeIt("concurrent execution", "should take about 3.5s to complete, "
                + "if thread are synchronized after every row calculations", () -> matrix.rowSumsConcurrent());
        }
        catch (Exception e) {
            System.out.println("Program stopped");
        }
        
    }

    public static void timeIt(String name, String note, Callable<int[]> func) throws Exception {
        System.out.println("Running " + name + " (" + note + ")...");
        long startTime = System.currentTimeMillis();
        int[] rowSums = func.call();
        long usedTime = System.currentTimeMillis() - startTime; // in milliseconds
        System.out.println(name + " took: " + usedTime / 100 / 10. + "s");
        System.out.println("Result:");
        for (int i = 0; i < rowSums.length; i++) {
            System.out.println(i + " -> " + rowSums[i]);
        }
    }

}

