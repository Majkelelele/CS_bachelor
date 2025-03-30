package pl.edu.uw.heroes.actions;

import pl.edu.uw.heroes.board.Board;
import pl.edu.uw.heroes.board.Field;
import pl.edu.uw.heroes.simulation.GameState;
import pl.edu.uw.heroes.units.Unit;

import java.util.LinkedList;

public class AtackArea extends UnitAction{

    LinkedList<Unit> unitsToAtack = new LinkedList<>();
    public AtackArea(Unit unit, Unit unitReceivingDamage, Board board) {
        super(unit);
        unitsToAtack.add(unitReceivingDamage);
        for (Unit unitAdd : unit.getHero().returnAdditionalUnitsToAtack(unit.getField(),unitReceivingDamage.getField(),board)) {
            unitsToAtack.add(unitAdd);
        }
    }
    @Override
    public void execute(GameState gameState) {
        for(Unit unitAttacked : unitsToAtack) {
            Atack atack = new Atack(unit,unitAttacked);
            atack.execute(gameState);
        }
        System.out.println(this.toString());
    }

    @Override
    public String toString() {
        return "Unit " + unit + " atacks units: " + unitsToAtack.toString();
    }

}
