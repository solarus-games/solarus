package zsdx.entities;

/**
 * This interface represents a non-static map entity, such as a chest, a door, a switch, etc.
 * Dynamic tiles are considered as dynamic entities too.
 * Every class implementing this interface should be a subclass of MapEntity.
 */
public interface DynamicEntity {

    public boolean hasName();
    public String getName();
}
