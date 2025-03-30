package pl.edu.uw.heroes.units;

import lombok.Getter;
import lombok.Setter;
import lombok.ToString;
import pl.edu.uw.heroes.actions.Atack;
import pl.edu.uw.heroes.actions.CounterAtack;
import pl.edu.uw.heroes.hero.Hero;
import pl.edu.uw.heroes.players.Player;
import pl.edu.uw.heroes.board.Field;
import pl.edu.uw.heroes.simulation.GameState;


@ToString
public class Unit{

    @Getter
    public final Player owner;

    @Getter
    private final Hero hero;
   public boolean isFlying() {
       return this.hero.isFlying();
   }

    @Getter
    private int heroesCount;

    @Getter
    private int weakestHeroHealth;

    @Setter
    @Getter
    private boolean isDefendingMode;

    @Getter
    private boolean hasCounterAtacked;

    public int getSpeed() {
        return this.hero.getSpeed();
    }

    private boolean hasWaitedInThisRound = false;

    @Getter
    private Field field;

    public Unit(Player owner, Hero hero, int count) {
        this.owner = owner;
        this.hero = hero;
        this.heroesCount = count;
        this.weakestHeroHealth = this.hero.getHealth();
        this.field = null;
        this.isDefendingMode = false;
        this.hasCounterAtacked = false;
    }
    public boolean canWait() {
        return !hasWaitedInThisRound;
    }
    public void doWait() {
        hasWaitedInThisRound = true;
    }
    public void doDefend() {this.setDefendingMode(true);
    }
    private int getEntireHealth() {
        return this.weakestHeroHealth + Math.max(0, (this.heroesCount - 1)*this.hero.getHealth());
    }
    public void obtainDamage(int damageStrength, GameState state, Unit unitAttacking) {
        int entireHealth = this.getEntireHealth();
        if(entireHealth <= damageStrength) {
            this.field.getUnit().getOwner().loseUnit(this);          //zabijamy cały oddział
            state.getUnits().remove(this);
        }
        else {
           if(weakestHeroHealth > damageStrength) {     //zmniejszamy tylko zdrowie najsłabszej jednostki
               weakestHeroHealth -= damageStrength;
           }
           else {
               heroesCount--;        //zabijamy hero z najmiejszym zdrowiem
               damageStrength -= weakestHeroHealth;
               heroesCount -= Math.floorDiv(damageStrength,hero.getHealth());
               weakestHeroHealth = hero.getHealth();
               weakestHeroHealth -= Math.floorMod(damageStrength,hero.getHealth());
           }
            if(!this.hasCounterAtacked && !unitAttacking.getHero().isShooting()) {
                this.hasCounterAtacked = true;
                CounterAtack counterAtack = new CounterAtack(this, unitAttacking);
                counterAtack.execute(state);
            }
        }
    }
    public void doAtack(Unit unitAtacked, int damagePower, GameState state) {
        unitAtacked.obtainDamage(damagePower, state, this);
    }
    public void doMove(Field destination) {
        if (field != null)
            field.setUnit(null);
        field = destination;
        destination.setUnit(this);
    }
    public void resetRound() {
        hasWaitedInThisRound = false;
        isDefendingMode = false;
        this.hasCounterAtacked = false;
    }
}
