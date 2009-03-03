package zsdx.entities;

/**
 * Interface for the subtype enumeration of an entity type.
 * Every enumeration implementing this interface must also define:
 * - the static method public static Subtype get(int id)
 *   which converts an integer into a value of the enumeration, and
 * - the static field public static final String[] humanNames
 *   which define a human readable name for each element of the enumeration.
 * The value with id zero is considered as the default value when a new entity
 * is created.
 */
public interface EntitySubtype {

    /**
     * Returns the id corresponding to the enumeration value.
     * @return the id
     */
    public int getId();
}
