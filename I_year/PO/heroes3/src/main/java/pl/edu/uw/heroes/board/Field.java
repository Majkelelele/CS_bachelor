package pl.edu.uw.heroes.board;

import lombok.Getter;
import lombok.Setter;
import pl.edu.uw.heroes.players.Player;
import pl.edu.uw.heroes.units.Unit;

import java.util.*;

public class Field {

    @Getter
    private final Position position;

    private final Map<Direction, Field> neighbors = new HashMap<>();

    @Getter
    @Setter
    private Unit unit;

    public Field(Position position) {
        this.position = position;
    }

    public boolean isEmpty() {
        return unit == null;
    }

    public void addNeighbor(Direction direction, Field field) {
        neighbors.put(direction, field);
    }

    public Collection<Field> getNeighbors() {
        return neighbors.values();
    }

    public Collection<Field> getEnemyNeighbors (Player player){
        LinkedList<Field> enemies = new LinkedList<Field>();
        this.getNeighbors().forEach(
                field -> {
                    if(!field.isEmpty() && !field.getUnit().getOwner().equals(player)) {
                        enemies.add(field);
                    }
                }
        );
        return enemies;
    }

    public void clearField() {
        this.unit = null;
    }


    // Obliczamy w ile ruchów można dojść od this.field do other.
    //wyobraźmy sobie, że idziemy od pola bardziej na lewo, więc będziemy musieli się poruszać w prawo, prawo-dół, prawo-góra
    public int calculateDistance(Field other) {
        int distance = 0;
        int absoluteHeightDifference = Math.abs(this.position.height() - other.position.height());
        distance += absoluteHeightDifference;                                                                       //najpierw idziemy w górę lub dół
        int widthRemainingToBeWalked = Math.abs(this.position.width() - other.position.width());
        widthRemainingToBeWalked -= Math.floorDiv(absoluteHeightDifference,2);                                      // za każdym razem kiedy przechodzimy z pola o height % 2 == 0 do pola o height nieparzystym
        if(widthRemainingToBeWalked > 0 && this.position.height() % 2 != other.position.height() % 2)               // za darmo zbliżamy sie o jeden width do pola szukanego, jeżeli oba pola są na polach o tej samej
            widthRemainingToBeWalked--;                                                                             // parzystości to takich przejść będzie floor(abs(height1-height2)/2) jeżeli są na polach o różnych parzystościach
        return distance + widthRemainingToBeWalked;                                                                 // to takich przejść będzie  floor(abs(height1-height2)/2) + 1 dlatego odejmuje dodatkowy jeden ruch.
    }

    @Override
    public String toString() {
        return position.toString();
    }
}
