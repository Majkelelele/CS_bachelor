package pl.edu.uw.heroes.hero;

import pl.edu.uw.heroes.Additional.Pair;
import pl.edu.uw.heroes.board.Direction;
import pl.edu.uw.heroes.board.Field;
import java.util.*;
import pl.edu.uw.heroes.board.Board;
import pl.edu.uw.heroes.units.Unit;

public class Dragon extends Hero{
    public Dragon() {
        this.atack = 12;
        this.defense = 12;
        this.arrowsCount = 0;
        this.health = 35;
        this.damage = new Pair(7,10);
        this.speed = 6;
        this.isFlying = false;
        this.isAtackingArea = true;
    }

    @Override
    public Collection<Unit> returnAdditionalUnitsToAtack(Field field, Field fieldToMove, Board board) {
        LinkedList<Unit> fieldsToAtack = new LinkedList<>();
        for(Direction direction : Direction.values()) {
            if (direction.move(fieldToMove.getPosition()).areEqual(field.getPosition())) {
                Field neighbor = board.getNeighbour(direction.opposite().move(fieldToMove.getPosition()));
                if (neighbor != null && !neighbor.isEmpty()) {
                    fieldsToAtack.add(neighbor.getUnit());
                }
            }
        }
        return fieldsToAtack;
    }

}
