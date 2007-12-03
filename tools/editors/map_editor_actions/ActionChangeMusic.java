package editors.map_editor_actions;

import editors.*;

/**
 * Changing the background music of the map.
 */
public class ActionChangeMusic extends MapEditorAction {

    private String musicBefore;
    private String musicAfter;
    
    /**
     * Constructor.
     * @param map the map
     * @param music the name of the music, i.e. a music file name,
     * Map.musicNone or Map.musicUnchanged
     */
    public ActionChangeMusic(Map map, String music) {
	super(map);

	this.musicBefore = map.getMusic();
	this.musicAfter = music;
    }

    /**
     * Executes the action.
     */
    public void execute() throws MapException {
	map.setMusic(musicAfter);
    }

    /**
     * Undoes the action.
     */
    public void undo() throws MapException {
	map.setMusic(musicBefore);
    }

}
