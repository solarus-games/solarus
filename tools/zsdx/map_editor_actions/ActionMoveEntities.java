package zsdx.map_editor_actions;

import java.util.List;
import java.util.LinkedList;

import zsdx.*;

/**
 * Moving some tiles on the map.
 */
public class ActionMoveEntities extends MapEditorAction {

    private List<MapEntity> entitiesToMove;
    private int dx;
    private int dy;

    /**
     * Creates an action to move one entity.
     * @param map the map
     * @param entityToMove the entity to move
     * @param dx number of pixels to move on x
     * @param dy number of pixels to move on y
     */
    public ActionMoveEntities(Map map, MapEntity entityToMove, int dx, int dy) {
	super(map);
	this.entitiesToMove = new LinkedList<MapEntity>();
	this.entitiesToMove.add(entityToMove);
	this.dx = dx;
	this.dy = dy;
    }

    /**
     * Creates an action to move several entities.
     * @param map the map
     * @param entitiesToMove the entities to move
     * @param dx number of pixels to move on x
     * @param dy number of pixels to move on y
     */
    public ActionMoveEntities(Map map, List<MapEntity> entitiesToMove, int dx, int dy) {
	super(map);
	this.entitiesToMove = new LinkedList<MapEntity>(entitiesToMove);
	this.dx = dx;
	this.dy = dy;
    }

    /**
     * Executes the action.
     */
    public void execute() throws MapException {

	map.moveEntities(entitiesToMove, dx, dy);
    }

    /**
     * Undoes the action.
     */
    public void undo() throws MapException {

	map.moveEntities(entitiesToMove, -dx, -dy);
    }

}
