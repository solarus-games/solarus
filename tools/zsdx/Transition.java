package zsdx;

/**
 * Indicates a type of transition between two maps.
 */
public enum Transition {

    IMMEDIATE,
    FADE;

    public static final String[] humanNames = {"Immediate", "Fade in/out"};

    public int getId() {
	return ordinal();
    }

    public static Transition get(int id) {
	return values()[id];
    }
}
