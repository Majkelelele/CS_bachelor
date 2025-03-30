package pl.edu.uw.heroes.actions;

import pl.edu.uw.heroes.simulation.GameState;
import pl.edu.uw.heroes.units.Unit;

public class MoveAndAtack extends UnitAction{
    private Atack atack;
    private Move move;
    public MoveAndAtack(Unit unit, Move move, Atack atack) {
        super(unit);
        this.move = move;
        this.atack = atack;
    }

    @Override
    public void execute(GameState gameState) {
       this.move.execute(gameState);
       this.atack.execute(gameState);
    }

    @Override
    public String toString() {
        return this.move.toString() + " and \n"  + this.atack.toString();
    }
}

