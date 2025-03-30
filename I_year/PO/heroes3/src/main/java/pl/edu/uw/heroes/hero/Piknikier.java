package pl.edu.uw.heroes.hero;

import pl.edu.uw.heroes.Additional.Pair;

public class Piknikier extends Hero {


    public Piknikier() {
        this.atack = 4;
        this.defense = 5;
        this.arrowsCount = 0;
        this.health = 10;
        this.damage = new Pair(1,3);
        this.speed = 4;
        this.isFlying = false;
        this.isAtackingArea = false;
    }
}
