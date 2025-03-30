package pl.edu.uw.heroes.players;

import lombok.AllArgsConstructor;
import lombok.Getter;
import pl.edu.uw.heroes.actions.Action;
import pl.edu.uw.heroes.units.Unit;

import java.util.ArrayList;
import java.util.Collection;
import java.util.Random;

@AllArgsConstructor
public class ExamplePlayer implements Player {

    private final String name;

    @Getter
    private final Collection<Unit> units = new ArrayList<>();

    private final Random random = new Random();

    @Override
    public Action chooseAction(Collection<Action> actions) {
        //System.out.println("Available actions for " + name + ": ");
        //actions.forEach(System.out::println);
        return actions.stream()
                .skip(random.nextInt(actions.size()))
                .findFirst()
                .get();
    }

    @Override
    public String toString() {
        return name;
    }

    @Override
    public boolean allUnitsDead() {
        return units.size() == 0;
    }

    @Override
    public void loseUnit(Unit unit) {
        units.remove(unit);
        unit.getField().clearField();
    }
}
