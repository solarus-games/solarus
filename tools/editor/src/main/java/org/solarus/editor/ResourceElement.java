package org.solarus.editor;

/**
 * Stores the id of an element from a resource, its name and its
 * resource type.
 */
public class ResourceElement {

    public final ResourceType type;
    public final String id;
    public final String name;

    public ResourceElement(ResourceType type, String id)
            throws QuestEditorException {
        this.type = type;
        this.id = id;
        this.name = "<html>" + Project.getResource(type).getElementName(id)
                    + "<font style=\"color:gray;\">(" + id + ")</font></html>";
    }

    public String toString() {
        return name;
    }
}
