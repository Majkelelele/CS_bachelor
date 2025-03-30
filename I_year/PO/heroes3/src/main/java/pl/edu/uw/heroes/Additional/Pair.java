package pl.edu.uw.heroes.Additional;

import lombok.Getter;
import lombok.ToString;

public class Pair {
    @Getter
    private int max;
    @Getter
    private int min;

    public Pair(int min, int max) {
        this.max = max;
        this.min = min;
    }
    public int chooseDamage() {
        return getRandomNumber(min,max);
    }
    public int getRandomNumber(int min, int max) {
        return (int) ((Math.random() * (max - min)) + min);
    }

}
