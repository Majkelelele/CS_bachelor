package laby_6;
import java.util.LinkedList;

public class BlockingQueue<T> {
    private LinkedList<T> list;
    private int capacity;
    public BlockingQueue(int capacity) {
        this.list = new LinkedList<T>();
        this.capacity = capacity;
    }

    public synchronized T take() throws InterruptedException {
        while (this.list.size() == 0) {
            wait();
        }
        T element = this.list.poll();
        notifyAll();
        return element;
        
    }

    public synchronized void put(T item) throws InterruptedException {
        while (this.list.size() == capacity) {
            wait();
        }
        this.list.addLast(item);
        notifyAll();
    }

    public synchronized int getSize() {
        return list.size();
    }

    public int getCapacity() {
        return this.capacity;
    }
}