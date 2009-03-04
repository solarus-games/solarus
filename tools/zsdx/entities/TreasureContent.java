package zsdx.entities;

import java.util.*;

/**
 * All possible contents of a treasure.
 */
public enum TreasureContent {
    NOTHING           (-1, false),

    FEATHER           (0, false),
    BOW               (2, false),
    BOW_AND_ARROWS    (3, false),
    BOOMERANG         (4, false),
    LAMP              (5, false),
    HOOK_SHOT         (6, false),
    BOTTLE            (7, false),
    WATER             (8, false),
    RED_POTION        (9, false),

    GREEN_POTION      (10, false),
    BLUE_POTION       (11, false),
    FAIRY_IN_BOTTLE   (12, false),
    PEGASUS_SHOES     (13, false),
    MYSTIC_MIRROR     (14, false),
    CANE_OF_SOMARIA   (15, false),
    APPLES            (16, true),
    PAINS_AU_CHOCOLAT (17, true),
    CROISSANTS        (18, true),
    ROCK_KEY          (19, false),

    RED_KEY           (20, false),
    CLAY_KEY          (21, false),
    APPLE_PIE         (22, false),
    GOLDEN_BARS       (23, false),
    EDELWEISS         (24, false),
    BONE_KEY          (25, false),
    FLIPPERS          (26, false),    
    MAGIC_CAPE        (27, false),
    IRON_KEY          (28, false),
    STONE_KEY         (29, false),

    WOODEN_KEY        (30, false),
    ICE_KEY           (31, false),
    IRON_GLOVE        (32, false),
    GOLDEN_GLOVE      (33, false),
    FIRE_STONE        (34, false),

    WORLD_MAP         (50, false),
    LARGE_RUPEE_BAG   (52, false),
    HUGE_RUPEE_BAG    (53, false),
    SMALL_BOMB_BAG    (54, false),
    LARGE_BOMB_BAG    (55, false),
    HUGE_BOMB_BAG     (56, false),
    LARGE_QUIVER      (58, false),
    HUGE_QUIVER       (59, false),

    BLUE_TUNIC        (61, false),
    RED_TUNIC         (62, false),
    SHIELD_1          (63, false),
    SHIELD_2          (64, false),
    SHIELD_3          (65, false),
    SWORD_1           (66, false),
    SWORD_2           (67, false),
    SWORD_3           (68, false),
    SWORD_4           (69, false),

    MAP               (70, false),
    COMPASS           (71, false),
    SMALL_KEY         (72, false),
    BIG_KEY           (73, false),
    BOSS_KEY          (74, false),
    PIECE_OF_HEART    (79, false),

    HEART_CONTAINER   (80, false),
    BOMBS             (82, true),
    ARROWS            (83, true),
    HEARTS            (84, true),
    SMALL_MAGIC       (85, false),
    BIG_MAGIC         (86, false),
    GREEN_RUPEES      (87, true),
    BLUE_RUPEES       (88, true),
    RED_RUPEES        (89, true);

    private final int index;
    private final boolean hasAmount;

    public static final String[] humanNames = {
	"Nothing / Script",
	"Feather",
	"Bow",
	"Bow and Arrows",
	"Boomerang",
	"Lamp",
	"Hook shot",
	"Bottle",
	"Water in bottle",
	"Red potion",

	"Green potion",
	"Blue potion",
	"Fairy in bottle",
	"Pegasus Shoes",
	"Mystic Mirror",
	"Cane of Somaria",
	"Apples",
	"Pains au chocolat",
	"Croissants",
	"Rock key",

	"Red key",
	"Clay key",
	"Apple pie",
	"Golden bars",
	"Edelweiss",
	"Bone key",
	"Flippers",
	"Magic cape",
	"Iron key",
	"Stone key",

	"Wooden key",
	"Ice key",
	"Iron glove",
	"Golden glove",
	"Fire stone",

	"World map",
	"Large rupee bag",
	"Huge rupee bag",
	"Small bomb bag",
	"Large bomb bag",
	"Huge bomb bag",
	"Large quiver",
	"Huge quiver",

	"Blue tunic",
	"Red tunic",
	"Shield 1",
	"Shield 2",
	"Shield 3",
	"Sword 1",
	"Sword 2",
	"Sword 3",
	"Sword 4",

	"Map",
	"Compass",
	"Small key",
	"Big key",
	"Boss key",
	"Piece of heart",

	"Heart container",
	"Bombs",
	"Arrows",
	"Hearts",
	"Small magic",
	"Big magic",
	"Green rupees",
	"Blue rupees",
	"Red rupees"
    };

    /**
     * Constructor.
     * @param index index of this content
     * @param hasAmount true if this content has an amount value
     */
    private TreasureContent(int index, boolean hasAmount) {
	this.index = index;
	this.hasAmount = hasAmount;
    }

    /**
     * Returns the treasure content with the specified index.
     * @param index index of the content to get
     * @return the treasure content with this index
     * @throws NoSuchElementException if the index is incorrect
     */
    public static TreasureContent get(int index) throws NoSuchElementException {
	for (TreasureContent t: values()) {
	    if (t.getIndex() == index) {
		return t;
	    }
	}
	
	throw new NoSuchElementException("Unknown treasure index: " + index);
    }

    /**
     * Returns the index of this content.
     * @return the index
     */
    public int getIndex() {
	return index;
    }

    /**
     * Returns true if this content has an amount value.
     * @return true if this content has an amount value
     */
    public boolean hasAmount() {
	return hasAmount;
    }

    /**
     * Returns whether this content can only exist inside a dungeon.
     * Note that a small key can exist outside a dungeon.
     * @return true if the content is the map, the compass, the big key or the boss key
     */
    public boolean mustBeInDungeon() {
	return this == MAP || this == COMPASS || this == BIG_KEY || this == BOSS_KEY;
    }
}
