package editors.map_editor_actions;

import java.awt.Rectangle;
import editors.*;

/**
 * Bringing some tiles to back.
 */
public class ActionResizeTile extends MapEditorAction {

    private TileOnMap tile;
    private Rectangle positionBefore;
    private Rectangle positionAfter;
    
    /**
     * Constructor.
     * @param map the map
     * @param tile the tile to resize
     * @param position the new tile size (this object is not modified by this class)
     */
    public ActionResizeTile(Map map, TileOnMap tile, Rectangle position) {
	super(map);
	
	this.tile = tile;
	this.positionBefore = new Rectangle(tile.getPositionInMap());
	this.positionAfter = new Rectangle(position);
    }

    /**
     * Executes the action.
     */
    public void execute() throws MapException {
	map.setTilePosition(tile, positionAfter);
    }

    /**
     * Undoes the action.
     */
    public void undo() throws MapException {
	map.setTilePosition(tile, positionBefore);
    }

}
