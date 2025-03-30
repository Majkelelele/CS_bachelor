package pl.edu.uw.heroes.hero;


import pl.edu.uw.heroes.board.Field;
import lombok.Getter;
import pl.edu.uw.heroes.Additional.Pair;
import pl.edu.uw.heroes.board.Board;
import pl.edu.uw.heroes.units.Unit;

import java.util.Collection;

public class Hero {
    @Getter
    protected int atack;

    @Getter
    protected int defense;

    @Getter
    protected int arrowsCount;

    @Getter
    protected int health;

    @Getter
    protected int speed;

    @Getter
    protected boolean isFlying;

    @Getter
    protected boolean isAtackingArea;


    public boolean isShooting() {
        return arrowsCount > 0;
    }

    public double additionalDamageReductions(Field field) {return 1;}

    public void oneArrowUsed(){}

    public Collection<Unit> returnAdditionalUnitsToAtack(Field field, Field fieldToMove, Board board) {return null;}

    @Getter
    protected  Pair damage;

}
