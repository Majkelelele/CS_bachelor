package cp2023.solution;
//class used to wrap boolean, used when using recursion algorithms
public class BooleanWrapper {
    private boolean value;
    public BooleanWrapper(boolean value) {
        this.value = value;
    }
    public boolean getValue() {
        return value;
    }
    public void changeValue(boolean newValue) {
        value = newValue;
    }
}
