package zsdx.map_editor_actions.edit_entities;

import zsdx.*;

/**
 * Editing the properties specific to a chest.
 */
public class ActionEditChest extends MapEditorAction {

    private Chest chest;

    private boolean bigChestBefore;
    private boolean bigChestAfter;

    private TreasureContent contentBefore;
    private TreasureContent contentAfter;
    
    private int amountBefore;
    private int amountAfter;
    
    private int savegameIndexBefore;
    private int savegameIndexAfter;
    
    /**
     * Constructor.
     * @param map the map
     * @param chest the chest edited
     * @param bigChest true if this is a big chest
     * @param content content of the treasure
     * @param amout amout of this content
     * @param savegameIndex index where the chest state is saved
     */
    public ActionEditChest(Map map, Chest chest, boolean bigChest, TreasureContent content, int amount, int savegameIndex) {
	super(map);
	
	this.chest = chest;
	
	this.bigChestBefore = chest.isBigChest();
	this.contentBefore = chest.getContent();
	this.amountBefore = chest.getAmount();
	this.savegameIndexBefore = chest.getSavegameIndex();

	this.bigChestAfter = bigChest;
	this.contentAfter = content;
	this.amountAfter = amount;
	this.savegameIndexAfter = savegameIndex;
    }


    /**
     * Executes the action.
     */
    public void execute() throws ZSDXException {
	chest.setBigChest(bigChestAfter);
	chest.setContent(contentAfter);
	chest.setAmount(amountAfter);
	chest.setSavegameIndex(savegameIndexAfter);
    }
    
    /**
     * Undoes the action.
     */
    public void undo() throws ZSDXException {
	chest.setBigChest(bigChestBefore);
	chest.setContent(contentBefore);
	chest.setAmount(amountBefore);
	chest.setSavegameIndex(savegameIndexBefore);
    }
}
