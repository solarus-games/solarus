package org.solarus.editor;

/**
 * Enumeration to identify the different kinds of resources of a quest.
 */
public enum ResourceType {

    MAP("Map"),
    TILESET("Tileset"),
    MUSIC("Music"),
    SPRITE("Sprite"),
    SOUND("Sound"),
    ITEM("Item"),
    ENEMY("Enemy"),
    LANGUAGE("Language");

    /**
     * The human-readable name of the resource type.
     */
    private String name;

    /**
     * Creates a new resource type.
     * @param name name of the resource type
     */
    private ResourceType(String name) {
        this.name = name;
    }

    /**
     * Returns the value with the specified id.
     * @param id id of the value to get
     * @return the value with this id
     * @throws QuestEditorException if there is no resource type with this id
     */
    public static ResourceType get(int id) throws QuestEditorException {

        try {
            return values()[id];
        }
        catch (ArrayIndexOutOfBoundsException ex) {
            throw new QuestEditorException("Unknown resource type '" + id + "'");
        }
    }

    /**
     * Returns the id of this value.
     * @return the id
     */
    public int getId() {
        return ordinal();
    }

    /**
     * Returns the human-readable name of the resource type
     * This name will be used in the GUI in the ResourceChooser.
     * @return the name of the resource type
     */
    public String getName() {
        return name;
    }
}
