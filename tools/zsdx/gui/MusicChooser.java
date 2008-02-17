package zsdx.gui;

import zsdx.Music;
import zsdx.ResourceDatabase;

/**
 * A combo box component to select a music from the resource database.
 */
public class MusicChooser extends ResourceChooser {

    /**
     * Constructor.
     */
    public MusicChooser() {
	super(ResourceDatabase.RESOURCE_MUSIC, false);
    }
    
    /**
     * Reloads the list. Two special elements (no music and no change)
     * are added at the beginning of the list.
     */
    protected void buildList() {
	
	addItem(new KeyValue(Music.noneId, Music.noneName));
	addItem(new KeyValue(Music.unchangedId, Music.unchangedName));

	super.buildList();
    }
}
