package pl.edu.uw.heroes.units;

import lombok.Getter;
import lombok.ToString;
import pl.edu.uw.heroes.players.Player;
import pl.edu.uw.heroes.board.Field;


@ToString
public class ExampleUnit implements Unit {

    @Getter
    private final Player owner;

    @Getter
    private final boolean isFlying;

    @Getter
    private final int speed;

    private boolean hasWaitedInThisRound = false;

    @Getter
    private Field field = null;

    public ExampleUnit(Player owner, boolean isFlying, int speed) {
        this.owner = owner;
        this.isFlying = isFlying;
        this.speed = speed;
    }


    @Override
    public boolean canWait() {
        return !hasWaitedInThisRound;
    }

    @Override
    public void doWait() {
        hasWaitedInThisRound = true;
    }

    @Override
    public void doDefend() {
    }

    @Override
    public void doMove(Field destination) {
        if (field != null)
            field.setUnit(null);
        field = destination;
        destination.setUnit(this);
    }

    @Override
    public void resetRound() {
        hasWaitedInThisRound = false;
    }
}
