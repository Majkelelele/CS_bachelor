package pl.edu.uw.heroes.actions;

import pl.edu.uw.heroes.simulation.GameState;
import pl.edu.uw.heroes.units.Unit;

public class CounterAtack extends UnitAction {

    private Atack counterAtack;
    private Unit unitAtacked;
    public CounterAtack(Unit unit, Unit unitAtacking) {
        super(unit);
        this.unitAtacked = unitAtacking;
        this.counterAtack = new Atack(unit, unitAtacking);
    }

    @Override
    public void execute(GameState gameState) {
        System.out.println(this.toString());
        counterAtack.execute(gameState);
    }

    @Override
    public String toString() {
        return "Unit " + unit.getHero().getClass().getSimpleName() + " counteratacks " + unitAtacked.getHero().getClass().getSimpleName() + " with damage " + counterAtack.getDamage();
    }
}
