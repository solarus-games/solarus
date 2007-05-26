package tileset_editor;

import java.awt.*;
import java.util.*;

/**
 * This class encapsulates a tile and the graphical components to view it.
 */
public class TileViewer {

    /**
     * The tile observed.
     */
    private Tile tile;

    /**
     * A graphical component to set the tile's animation properties.
     */
    private AnimationView animationView;

    /**
     * The 16*16 icon of the tile.
     */
    private TileIcon icon;

    /**
     * Constructor.
     */
    public TileViewer(Tile tile, Tileset tileset) {
	this.tile = tile;
	icon = new TileIcon(tile, tileset);
	animationView = new AnimationView(tile);
    }

    /**
     * Returns the tile's icon.
     * @return the tile's 16*16 icon
     */
    public TileIcon getIcon() {
	return icon;
    }

    /**
     * Returns a component representing the tile's coordinates in the tileset.
     * @return the tile's coordinates
     */
    public String getCoordinatesView() {
	// TODO: return 4 text fields, add 2 radio buttons if the tile is animated
	Rectangle positionInTileset = tile.getPositionInTileset();
	StringBuffer buff = new StringBuffer();
	buff.append('(');
	buff.append(positionInTileset.x);
	buff.append(',');
	buff.append(positionInTileset.y);
	buff.append(") -> (");
	buff.append(positionInTileset.x + positionInTileset.width);
	buff.append(',');
	buff.append(positionInTileset.y + positionInTileset.height);
	buff.append(')');

	return new String(buff);
    }

    /**
     * Returns a graphical component to set the tile's animation type.
     * @return the tile's animation view
     */
    public AnimationView getAnimationView() {
	return animationView;
    }
}
