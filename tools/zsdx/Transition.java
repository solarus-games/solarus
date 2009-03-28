package zsdx;

/**
 * Indicates a type of transition between two maps.
 */
public enum Transition {

    IMMEDIATE,
    FADE,
    SCROLLING;

    public static final String[] humanNames = {"Immediate", "Fade in/out", "Scrolling"};

    public int getId() {
	return ordinal();
    }

    public static Transition get(int id) {
	return values()[id];
    }
}
