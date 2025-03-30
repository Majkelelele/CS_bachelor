package pl.edu.uw.heroes;

import pl.edu.uw.heroes.board.Board;
import pl.edu.uw.heroes.board.Field;
import pl.edu.uw.heroes.board.Position;
import pl.edu.uw.heroes.hero.*;
import pl.edu.uw.heroes.players.ExamplePlayer;
import pl.edu.uw.heroes.players.Player;
import pl.edu.uw.heroes.simulation.GameSimulation;
import pl.edu.uw.heroes.simulation.GameState;
import pl.edu.uw.heroes.units.Unit;

public class Main {
 //przykladowa rozgrywka, gracze wybierają losowo ruchy ze wszystkich możliwych
    public static void main(String[] args) {
        Player playerLeft = new ExamplePlayer("Adam");
        Unit playerLeftUnit = new Unit(playerLeft, new Dragon(),8);
        Unit playerLeftUnit1 = new Unit(playerLeft,new Crusader(), 12);
        Unit playerLeftUnit2 = new Unit(playerLeft, new Archer(), 1);

        playerLeft.getUnits().add(playerLeftUnit);
        playerLeft.getUnits().add(playerLeftUnit1);
        playerLeft.getUnits().add(playerLeftUnit2);

        Player playerRight = new ExamplePlayer("Wiktor");
        Unit playerRightUnit = new Unit(playerRight,  new Crusader(),10);
        Unit playerRightUnit1 = new Unit(playerRight, new Archer(), 22);
        Unit playerRightUnit2 = new Unit(playerRight, new Crusader(), 1);

        playerRight.getUnits().add(playerRightUnit);
        playerRight.getUnits().add(playerRightUnit1);
        playerRight.getUnits().add(playerRightUnit2);


        GameState gameState = new GameState(playerLeft, playerRight, new Board(10, 10));

        playerLeftUnit.doMove(gameState.getBoard().getField(new Position(0, 0)));
        playerLeftUnit1.doMove(gameState.getBoard().getField(new Position(1, 0)));
        playerLeftUnit2.doMove(gameState.getBoard().getField(new Position(4, 0)));

        playerRightUnit.doMove(gameState.getBoard().getField(new Position(0, 9)));
        playerRightUnit1.doMove(gameState.getBoard().getField(new Position(2, 9)));
        playerRightUnit2.doMove(gameState.getBoard().getField(new Position(9, 9)));

        GameSimulation gameSimulation = new GameSimulation(gameState);

        gameSimulation.simulateGame();


    }
}
