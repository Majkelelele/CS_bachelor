package pl.edu.uw.heroes.hero;

import pl.edu.uw.heroes.Additional.Pair;

public class Crusader extends Hero{
    public Crusader() {
        this.atack = 12;
        this.defense = 12;
        this.arrowsCount = 0;
        this.health = 35;
        this.damage = new Pair(7,10);
        this.speed = 6;
        this.isFlying = false;
        this.isAtackingArea = false;
    }
}
