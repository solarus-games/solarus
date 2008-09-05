package zsdx;

import java.util.NoSuchElementException;

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
    MAGIC_CAPE        (16, false),
    IRON_GLOVE        (17, false),
    GOLDEN_GLOVE      (18, false),
    FIRE_STONE        (19, false),

    APPLES            (20, false),
    PAINS_AU_CHOCOLAT (21, false),
    CROISSANTS        (22, false),
    APPLE_PIE         (23, false),
    GOLDEN_BARS       (24, false),
    EDELWEISS         (25, false),
    BONE_KEY          (26, false),
    FLIPPERS          (27, false),
    RED_KEY           (28, false),

    CLAY_KEY          (30, false),
    ROCK_KEY          (31, false),
    IRON_KEY          (32, false),
    STONE_KEY         (33, false),
    WOODEN_KEY        (34, false),
    ICE_KEY           (35, false),

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
    
    /**
     * Constructor.
     * @param index index of this content
     * @param hasAmount true if this content has an amount value
     */
    TreasureContent(int index, boolean hasAmount) {
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
}
