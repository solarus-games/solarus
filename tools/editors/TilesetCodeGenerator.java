package editors;

import java.io.*;
import java.awt.*;

/**
 * This class provides functionnalities to generate
 * the C++ code corresponding to a tileset.
 */
public class TilesetCodeGenerator {

    private Tileset tileset;       // the tileset
    private String name;           // the tileset's name
    private String upperCaseName;  // the upper case tileset's name
    private String zsdxRootPath;   // the root directory of ZSDX
    private String headerFileName; // name of the header file (without the path)

    /**
     * Symbolic constants for the type of obstacle in the generated code.
     */
    private static final String[] obstacleConstants = {
	"OBSTACLE_NONE",
	"OBSTACLE",
	"OBSTACLE_TOP_RIGHT",
	"OBSTACLE_TOP_LEFT",
	"OBSTACLE_BOTTOM_LEFT",
	"OBSTACLE_BOTTOM_RIGHT"
    };

    /**
     * Symbolic constants for the animation sequence in the generated code.
     */
    private static final String[] animationSequenceConstants = {
	"ANIMATION_SEQUENCE_012",
	"ANIMATION_SEQUENCE_0121"
    };

    /**
     * Constructor.
     */
    public TilesetCodeGenerator() {
    }

    /**
     * Generates the code corresponding to a tileset.
     * @param tileset the tileset
     */
    public void generate(Tileset tileset) throws IOException {
	this.tileset = tileset;
	zsdxRootPath = Configuration.getInstance().getZsdxRootPath();
	name = tileset.getName();
	upperCaseName = name.toUpperCase();
	
	// generate the header file
	generateHeaderFile();

	// generate the source file
	generateSourceFile();

	// update include/TilesetList.inc.h
	updateTilesetsEnumFile();

	// update src/TilesetsCreation.inc.cpp
	updateTilesetsCreationFile();
    }

    /**
     * Generates the header file, for example include/tilesets/TilesetHouse.h
     */
    private void generateHeaderFile() throws IOException {

	headerFileName = "Tileset" + name + ".h";
	String fileName = zsdxRootPath + File.separator + "include" +
	    File.separator + "tilesets" + File.separator + headerFileName;

	PrintWriter out = new PrintWriter(new BufferedWriter(new FileWriter(fileName)));
	
	out.println("#ifndef ZSDX_TILESETS_" + upperCaseName + "_H");
	out.println("#define ZSDX_TILESETS_" + upperCaseName + "_H");
	out.println();
	out.println("#include \"Tileset.h\"");
	out.println();
	out.println("/*");
	out.println(" * Tileset generated automatically by the tileset editor.");
	out.println(" */");
	out.println("class Tileset" + name + ": public Tileset {");
	out.println();
	out.println(" public:");
	out.println("  Tileset" + name + "(void) { }");
	out.println("  inline ~Tileset" + name + "(void) { }");
	out.println();
	out.println("  void load(void);");
	out.println("};");
	out.println();
	out.println("#endif");

	out.close();
    }

    /**
     * Generates the source file, for example src/tilesets/TilesetHouse.cpp
     */
    private void generateSourceFile() throws IOException {

	String fileName = zsdxRootPath + File.separator + "src" +
	    File.separator + "tilesets" + File.separator + "Tileset" +
	    name + ".cpp";

	PrintWriter out = new PrintWriter(new BufferedWriter(new FileWriter(fileName)));
	
	out.println("/*");
	out.println(" * Tileset generated automatically by the tileset editor.");
	out.println(" */");
	out.println();
	out.println("#include <SDL/SDL.h>");
	out.println("#include \"tilesets/" + headerFileName + "\"");
	out.println("#include \"simple_tile_image.h\"");
	out.println("#include \"animated_tile_image.h\"");
	out.println();
	out.println("/**");
	out.println(" * Loads the tileset.");
	out.println(" */");
	out.println("void Tileset" + name + "::load(void) {");
	out.println("  load_tileset_image(\"images/tilesets/" + name + ".png\");");
	out.println("  SDL_Rect position_in_tileset;");

	// create the local variable positions_in_tileset[] only if there is at least one animated tile
	boolean found = false;
	for (int i = 0; i < tileset.getNbTiles() && !found; i++) {
	    if (tileset.getTile(i).isAnimated()) {
		out.println("  SDL_Rect positions_in_tileset[3]; // for animated tiles");
		found = true;
	    }
	}

	out.println();

	// create each tile
	for (int i = 0; i < tileset.getNbTiles(); i++) {
	    Tile tile = tileset.getTile(i);
	    Rectangle positionInTileset = tile.getPositionInTileset();
	    int x = positionInTileset.x;
	    int y = positionInTileset.y;
	    int width = positionInTileset.width;
	    int height = positionInTileset.height;

	    String obstacleConstant = obstacleConstants[tile.getObstacle()];

	    out.println("  // tile " + i);
	    if (!tile.isAnimated()) {
		// simple tile
		out.println("  position_in_tileset.x = " + x + ";");
		out.println("  position_in_tileset.y = " + y + ";");
		out.println("  position_in_tileset.w = " + width + ";");
		out.println("  position_in_tileset.h = " + height + ";");
		out.println("  tiles[nb_tiles++] = new SimpleTileImage(position_in_tileset, " + obstacleConstant + ");");
		
	    }
	    else {
		// animated tile
		int dx, dy;
		if (tile.getAnimationSeparation() == Tile.ANIMATION_SEPARATION_HORIZONTAL) {
		    width = width / 3;
		    dx = width;
		    dy = 0;
		}
		else {
		    height = height / 3;
		    dx = 0;
		    dy = height;
		}

		String animationSequenceConstant = animationSequenceConstants[tile.getAnimationSequence()];
	    
		// generate the three images
		out.println("  positions_in_tileset[0].x = " + x + ";");
		out.println("  positions_in_tileset[0].y = " + y + ";");
		out.println("  positions_in_tileset[0].w = " + width + ";");
		out.println("  positions_in_tileset[0].h = " + height + ";");
		out.println();

		x += dx; 
		y += dy;
		out.println("  positions_in_tileset[1].x = " + x + ";");
		out.println("  positions_in_tileset[1].y = " + y + ";");
		out.println("  positions_in_tileset[1].w = " + width + ";");
		out.println("  positions_in_tileset[1].h = " + height + ";");
		out.println();

		x += dx; 
		y += dy;
		out.println("  positions_in_tileset[2].x = " + x + ";");
		out.println("  positions_in_tileset[2].y = " + y + ";");
		out.println("  positions_in_tileset[2].w = " + width + ";");
		out.println("  positions_in_tileset[2].h = " + height + ";");
		out.println();
		out.println("  tiles[nb_tiles++] = new AnimatedTileImage(positions_in_tileset, " + animationSequenceConstant + ", " + obstacleConstant + ");");

	    }
	}
	
	out.println("}");
	out.close();
    }

    /**
     * Updates (if needed) the file include/enum_tilesets.inc.h.
     * This file contains the declaration of the tilesets symbolic constants.
     */
    private void updateTilesetsEnumFile() throws IOException {
	String fileName = zsdxRootPath + File.separator + "include" +
	    File.separator + "TilesetList.inc.h";
	
	// we have to put the following line into the file unless it is already present
	String lineWanted = "TILESET_" + upperCaseName + ",";

	FileTools.ensureFileHasLine(fileName, lineWanted);
    }
    
    /** 
     * Updates (if needed) the file src/creation_tilesets.inc.cpp.
     * This file contains the creation of each tileset objects.
     */
    private void updateTilesetsCreationFile() throws IOException {
	String fileName = zsdxRootPath + File.separator + "src" +
	    File.separator + "TilesetsCreation.inc.cpp";
	
	// we have to put the following line into the file unless it is already present
	String lineWanted = "tilesets[TILESET_" + upperCaseName + "] = new Tileset" + name + "();";
	
	FileTools.ensureFileHasLine(fileName, lineWanted);
    }
}
