package pl.edu.uw.heroes.players;

import pl.edu.uw.heroes.actions.Action;
import pl.edu.uw.heroes.units.Unit;

import java.util.Collection;

public interface Player {

    Collection<Unit> getUnits();

    Action chooseAction(Collection<Action> actions);

    void loseUnit(Unit unit);

    boolean allUnitsDead();
}
