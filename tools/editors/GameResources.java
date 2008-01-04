package editors;

import java.io.*;
import java.util.*;

/**
 * This class contains every game resource. Each resource is associated with an id.
 * This class contains the information of the file game.zsd.
 */
public class GameResources {

    private TreeMap<String, String> musics;

    private TreeMap<Integer, String> maps;

    private TreeMap<Integer, String> tilesets;

    /**
     * Creates the game resources from the file game.zsd.
     */
    public GameResources() {

    }

    public String getMusicName(String musicID) {
	return musics.get(musicID);
    }

    public String getMapName(int mapID) {
	return maps.get(mapID);
    }

    public String getTilesetName(int tilesetID) {
	return tilesets.get(tilesetID);
    }

}
