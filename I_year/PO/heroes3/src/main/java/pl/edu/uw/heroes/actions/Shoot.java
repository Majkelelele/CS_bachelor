package pl.edu.uw.heroes.actions;

import pl.edu.uw.heroes.simulation.GameState;
import pl.edu.uw.heroes.units.Unit;

public class Shoot extends UnitAction{

    Unit unitShooted;

    public Shoot(Unit unit, Unit unitAimed) {
        super(unit);
        this.unitShooted = unitAimed;
    }

    private int calculateDamage() {
        Atack shoot = new Atack(unit, unitShooted);
        return ((int) Math.floor(shoot.calculateBaseDamage()));
    }
    @Override
    public void execute(GameState gameState) {
        unit.doAtack(unitShooted, calculateDamage(), gameState);
        unit.getHero().oneArrowUsed();
    }

    @Override
    public String toString() {
        return "Unit " + unit + " shoots " + unitShooted + " with damage " + calculateDamage();
    }
}
