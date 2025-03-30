package pl.edu.uw.heroes.hero;

import pl.edu.uw.heroes.Additional.Pair;
import pl.edu.uw.heroes.board.Field;

public class Archer extends Hero{

    public Archer() {
        this.atack = 6;
        this.defense = 3;
        this.arrowsCount = 12;
        this.health = 10;
        this.damage = new Pair(2,3);
        this.speed = 4;
        this.isFlying = false;
        this.isAtackingArea = false;
    }

    @Override
    public double additionalDamageReductions(Field field) {
        if(field.getEnemyNeighbors(field.getUnit().getOwner()).size() > 0 ) {
            return 0.5;
        }
        return 1;
    }

    @Override
    public void oneArrowUsed() {
        this.arrowsCount--;
    }
}
