package pl.edu.uw.heroes.hero;

import pl.edu.uw.heroes.Additional.Pair;

public class Halberdier extends Hero{

    public Halberdier() {
        this.atack = 6;
        this.defense = 5;
        this.arrowsCount = 0;
        this.health = 10;
        this.damage = new Pair(2,3);
        this.speed = 5;
        this.isAtackingArea = false;
        this.isFlying = false;
    }
}
