package zsdx.map_editor_actions.edit_entities;

import zsdx.*;
import zsdx.entities.*;

/**
 * Editing the properties specific to an interactive entity.
 */
public class ActionEditInteractiveEntity extends MapEditorAction {

    private InteractiveEntity entity;

    private String spriteBefore;
    private String spriteAfter;

    private String messageBefore;
    private String messageAfter;

    /**
     * Constructor.
     * @param map the map
     * @param entity the interactive entity edited
     * @param sprite the sprite to display for the entity
     * @param message the message to show when interacting with the entity
     */
    public ActionEditInteractiveEntity(Map map, InteractiveEntity entity,
	    String sprite, String message) {
	super(map);

	this.entity = entity;

	this.spriteBefore = entity.getSprite();
	this.messageBefore = entity.getMessageId();

	this.spriteAfter = sprite;
	this.messageAfter = message;
    }


    /**
     * Executes the action.
     */
    public void execute() throws ZSDXException {
	entity.setSprite(spriteAfter);
	entity.setMessageId(messageAfter);
    }
    
    /**
     * Undoes the action.
     */
    public void undo() throws ZSDXException {
	entity.setSprite(spriteBefore);
	entity.setMessageId(messageBefore);
    }
}
