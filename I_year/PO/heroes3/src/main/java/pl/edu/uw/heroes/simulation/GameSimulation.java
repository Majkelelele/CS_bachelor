package pl.edu.uw.heroes.simulation;

import lombok.AllArgsConstructor;
import pl.edu.uw.heroes.actions.*;
import pl.edu.uw.heroes.board.BFSCalculator;
import pl.edu.uw.heroes.units.Unit;

import java.util.ArrayList;
import java.util.Collection;
import java.util.Queue;
import java.util.stream.Stream;

@AllArgsConstructor
public class GameSimulation {

    private final GameState state;

    private final BFSCalculator bfsCalculator = new BFSCalculator();

    private final ActionGenerator actionGenerator = new ActionGenerator(bfsCalculator);

    public void simulateGame() {
        while(!state.getPlayerLeft().allUnitsDead() && !state.getPlayerRight().allUnitsDead()) {
                this.executeOneAction();
        }
        if(state.getPlayerLeft().allUnitsDead()) {
            System.out.println("Player " + state.getPlayerRight().toString() + " has won!");
        }
        else System.out.println("Player " + state.getPlayerLeft().toString() + " has won!");
    }

    public void executeOneAction() {
        Queue<Unit> units = state.getUnits();
        if (units.isEmpty())
            prepareNextRound();

        Unit unit = units.poll();

        Collection<Action> actions = actionGenerator.generateactionsForUnit(unit, state);
        Action action = unit.getOwner().chooseAction(actions);
        System.out.println(unit.getOwner() + " chose " + action);
        action.execute(state);
        state.getExecutedActions().add(action);
    }

    private void prepareNextRound() {
        Queue<Unit> units = state.getUnits();
        Stream.concat(
                        state.getPlayerLeft().getUnits().stream(),
                        state.getPlayerRight().getUnits().stream()
                )
                .peek(Unit::resetRound)
                .sorted((u1, u2) -> Integer.compare(u2.getSpeed(), u1.getSpeed()))
                .forEach(units::add);
        if (units.isEmpty())
            throw new IllegalStateException("No units on board!");
    }
}
