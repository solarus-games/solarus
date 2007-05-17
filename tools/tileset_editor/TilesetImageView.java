package tileset_editor;

import javax.swing.*;
import java.awt.*;

/**
 * This component shows the tileset image and allow the user to select the tiles.
 */
public class TilesetImageView extends JScrollPane {

    /**
     * Constructor.
     */
    public TilesetImageView() {
	super(new JLabel("image"));
    }

}
