import java.util.ArrayList;

import java.util.LinkedList;
import java.util.List;

import java.util.concurrent.BrokenBarrierException;
import java.util.concurrent.Callable;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.CyclicBarrier;
import java.util.concurrent.Semaphore;

import java.util.function.IntBinaryOperator;


public class MatrixRowSumsConcurrent {

    private static final int NUM_ROWS = 10;
    private static final int NUM_COLUMNS = 3;
    private static final int VERY_LARGE_ROW_NUMBER = 20;
    private static ConcurrentHashMap<Integer, Integer> map = new ConcurrentHashMap<>();

    private static class Matrix {

        private final int numRows;
        private final int numColumns;
        private final IntBinaryOperator definition;
        private static final CyclicBarrier barrier = new CyclicBarrier(NUM_COLUMNS+1);
        private static Semaphore [] semafor = new Semaphore [NUM_ROWS];
        static {
            for(int i = 0; i < NUM_ROWS; i++) {
            semafor[i] = new Semaphore(1);
            }   
        }
        private static volatile int res[] = new int[NUM_ROWS];


        public Matrix(int numRows, int numColumns, IntBinaryOperator definition) {
            this.numRows = numRows;
            this.numColumns = numColumns;
            this.definition = definition;
        }

        public int[] rowSums() {
            int[] rowSums = new int[NUM_ROWS];
            for (int row = 0; row < NUM_ROWS; ++row) {
                int sum = 0;
                for (int column = 0; column < NUM_COLUMNS; ++column) {
                    sum += definition.applyAsInt(row, column);
                }
                rowSums[row] = sum;
            }
            return rowSums;
        }


        public int[] rowSumsConcurrent() throws InterruptedException {
            int[] rowSums = new int[NUM_ROWS];
            for(int i = 0; i < numRows; i++) {
                map.put(i,0);
            }
            List<Thread> threads = new ArrayList<>();
            for(int startingPoint = 0; startingPoint < NUM_ROWS; startingPoint+=VERY_LARGE_ROW_NUMBER){
                int endingPoint = Math.min(NUM_ROWS, startingPoint + VERY_LARGE_ROW_NUMBER);
                for (int columnNo = 0; columnNo < numColumns; ++columnNo) {
                    Thread t = new Thread(new PerColumnDefinitionApplier(columnNo, startingPoint, endingPoint, Thread.currentThread()));
                    threads.add(t);
                }
            }
            
            for (Thread t : threads) {
                t.start();
            }

            try {
                for (Thread t : threads) {
                    t.join();
                }
            }
            catch ( InterruptedException e) {
                for(Thread t : threads) t.interrupt();
                System.out.println("threads interupted in JOIN");
            }    
            for(int i = 0; i < numRows; i++) {
                rowSums[i] = map.get(i);
            }
            return rowSums;
        }

        private class PerColumnDefinitionApplier implements Runnable {

            private final int myColumnNo;
            private final int startingPoint;
            private final int endingPoint;
            private final Thread mainThread;

            private PerColumnDefinitionApplier(
                    int myColumnNo,
                    int startingPoint,
                    int endingPoint,
                    Thread mainThread
            ) {
                this.myColumnNo = myColumnNo;
                this.mainThread = mainThread;
                this.endingPoint = endingPoint;
                this.startingPoint = startingPoint;
            }

            @Override
            public void run() {
                
                    for(int currentRow = startingPoint; currentRow < endingPoint; currentRow++) {
                        int result = definition.applyAsInt(currentRow,myColumnNo);
                        if(Thread.interrupted()) {
                            mainThread.interrupt();
                        }
                        map.put(currentRow,map.get(currentRow)+result);
                    }
                
            }

        }

        public int[] rowSumsConcurrent_2() throws Exception {
            
            LinkedList<Thread> threadsList = new LinkedList<>();
            for(int k = 0; k < NUM_COLUMNS; k++) {
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
                for(int currentRow = 0; currentRow < NUM_ROWS; currentRow++) {
                    int result = definition.applyAsInt(currentRow,columnNum);
                    if(Thread.interrupted()) {
                        return;
                    }
                    try {
                        semafor[currentRow].acquire();
                        res[currentRow] += result;
                        semafor[currentRow].release();
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
        Matrix matrix = new Matrix(NUM_ROWS, NUM_COLUMNS, (row, column) -> {
            // long computations
            int a = 2 * column + 1;
            int cellId = column + row * NUM_COLUMNS;
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
            timeIt("concurrent execution with semaphore", "should take about 3.5s to complete, "
                + "if thread are synchronized after every row calculations", () -> matrix.rowSumsConcurrent_2());
        }
        catch (Exception e) {
            System.out.println("Program stopped");
        }

        timeIt("concurrent execution with HASHMAP", "should take about 3.5s to complete, "
                + "if thread are synchronized after every row calculations", () -> matrix.rowSumsConcurrent());
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