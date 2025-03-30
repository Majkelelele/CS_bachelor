package pl.edu.uw.heroes.board;

public record Position(int height, int width) {
    public boolean areEqual(Position other) {
        return height == other.height && width == other.width;
    }
}
