package pl.edu.uw.heroes.simulation;

import pl.edu.uw.heroes.actions.*;
import pl.edu.uw.heroes.board.BFSCalculator;
import pl.edu.uw.heroes.units.Unit;

import java.util.ArrayList;
import java.util.Collection;

public class ActionGenerator {

    BFSCalculator bfsCalculator;


    public ActionGenerator(BFSCalculator calc) {
        this.bfsCalculator = calc;
    }

    public Collection<Action> generateactionsForUnit(Unit unit, GameState state) {
        Collection<Action> actions = new ArrayList<>();
        actions.add(new Defend(unit));
        if (unit.canWait())
            actions.add(new Wait(unit));
        bfsCalculator.calculatePossibleMoves(unit.getField()).forEach(field -> actions.add(new Move(unit, field)));
        if(!unit.getHero().isAtackingArea()) {
            moveAndAttackAction(actions,unit);
            atackNeighboursAction(actions,unit);
        }
        else {
            atackArea(actions,unit,state);
        }

        shootAction(actions,unit,state);
        return actions;
    }

    private boolean shootingPossibility(Unit unit) {
        return unit.getHero().isShooting() &&
                unit.getField().getEnemyNeighbors(unit.getOwner()).size() == 0 &&
                unit.getHero().getArrowsCount() > 0;
    }

    public void moveAndAttackAction(Collection<Action> actions, Unit unit) {
        bfsCalculator.calculatePossibleMoves(unit.getField()).forEach(
                field -> field.getEnemyNeighbors(unit.getOwner()).forEach(
                        field1 -> {
                            actions.add(new MoveAndAtack(unit,new Move(unit,field),new Atack(unit, field1.getUnit())));
                        }

                )
        );
    }
    public void atackNeighboursAction(Collection<Action> actions, Unit unit) {
        unit.getField().getEnemyNeighbors(unit.getOwner()).forEach(
                field -> {
                    actions.add(new Atack(unit,field.getUnit()));
                }
        );
    }

    public void shootAction(Collection<Action> actions, Unit unit, GameState state) {
        if(shootingPossibility(unit)) {
            if(unit.getOwner().equals(state.getPlayerLeft())) {
                state.getPlayerRight().getUnits().forEach(
                        unitShooted -> actions.add(new Shoot(unit, unitShooted))
                );
            }
            else {
                state.getPlayerLeft().getUnits().forEach(
                        unitShooted -> actions.add(new Shoot(unit, unitShooted))
                );
            }
        }
    }
    public void atackArea(Collection<Action> actions, Unit unit, GameState state) {
        unit.getField().getEnemyNeighbors(unit.getOwner()).forEach(
                field -> {
                    actions.add(new AtackArea(unit,field.getUnit(), state.getBoard()));
                }
        );
    }
}
