package pl.edu.uw.heroes.actions;

import lombok.Getter;
import pl.edu.uw.heroes.board.Field;
import pl.edu.uw.heroes.simulation.GameState;
import pl.edu.uw.heroes.units.Unit;

public class Atack extends UnitAction{
    private Unit unitAtacked;

    @Getter
    private int damage;
    public Atack(Unit unit, Unit unitAtacked) {
        super(unit);
        this.unitAtacked = unitAtacked;
        this.damage = ((int) Math.floor(this.calculateBaseDamage() * this.adjustForDamageBonus() *
                this.adjustForDamageReduction() * unit.getHero().additionalDamageReductions(unit.getField())));
    }

    @Override
    public void execute(GameState gameState) {
        this.unit.doAtack(this.unitAtacked, damage, gameState);
    }

    private double adjustForDamageBonus() {
       double bonus = 1;
       if(unitAtacked.getHero().getDefense() < unit.getHero().getAtack()) {
           bonus += Math.min(3,0.05*(unit.getHero().getAtack() - unitAtacked.getHero().getDefense()));
       }
       return bonus;
    }

    private double adjustForDamageReduction() {
        double bonus = 1;
        if(unitAtacked.getHero().getDefense() > unit.getHero().getAtack()){
            bonus -= Math.min(0.7, 0.025*(unitAtacked.getHero().getDefense() - unit.getHero().getAtack()));
        }
        return bonus;
    }

    public double calculateBaseDamage() {
        int heroCount = this.unit.getHeroesCount();
        int damage = 0;
        if(heroCount < 10) {
            for(int i = 0; i < heroCount; i++){
                damage += unit.getHero().getDamage().chooseDamage();
            }
        }
        else {
            for(int i = 0; i < 10; i++) damage += unit.getHero().getDamage().chooseDamage();
            damage *=  unit.getHeroesCount()/10;
        }
        return damage*unit.getHero().additionalDamageReductions(unit.getField());
    }




    @Override
    public String toString() {
        return "Unit " + unit + " atacks " + unitAtacked + " with damage " + damage;
    }
}
