/*
 * Copyright (C) 2006-2012 Christopho, Solarus - http://www.solarus-games.org
 *
 * Solarus Quest Editor is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Zelda: Mystery of Solarus DX is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
package org.solarus.editor.gui.edit_entities;

import javax.swing.*;
import org.solarus.editor.*;
import org.solarus.editor.entities.*;
import org.solarus.editor.gui.*;
import org.solarus.editor.map_editor_actions.*;

/**
 * A component to edit a shop item.
 */
public class EditShopItemComponent extends EditEntityComponent {

    // specific fields of this type of entity
    private TreasureChooser treasureField;
    private NumberChooser priceField;
    private JTextField dialogIdField;

    /**
     * Constructor.
     * @param map the map
     * @param entity the entity to edit
     */
    public EditShopItemComponent(Map map, MapEntity entity) {
        super(map, entity);
    }

    /**
     * Creates the specific fields for this kind of entity.
     */
    protected void createSpecificFields() {

        // treasure content
        treasureField = new TreasureChooser(true, false);
        addField("Treasure", treasureField);

        // price
        priceField = new NumberChooser(10, 1, 999);
        addField("Price", priceField);

        // description dialog
        dialogIdField = new JTextField(15);
        addField("Description dialog id", dialogIdField);
    }

    /**
     * Updates the information displayed in the fields.
     */
    public void update() {
        super.update(); // update the common fields

        ShopItem shopItem = (ShopItem) entity;

        treasureField.setTreasure(
                shopItem.getProperty("treasure_name"),
                shopItem.getIntegerProperty("treasure_variant"),
                shopItem.getIntegerProperty("treasure_savegame_variable"));
        priceField.setNumber(shopItem.getIntegerProperty("price"));
        dialogIdField.setText(shopItem.getProperty("dialog"));
    }

    /**
     * Returns the specific part of the action made on the entity.
     * @return the specific part of the action made on the entity
     */
    protected ActionEditEntitySpecific getSpecificAction() {

        String treasureName = treasureField.getTreasure().getItemName();
        Integer treasureVariant = treasureField.getTreasure().getVariant();
        Integer treasureSavegameVariable = treasureField.getTreasure().getSavegameVariable();

        return new ActionEditEntitySpecific(entity,
                treasureName,
                treasureVariant == null ? null : Integer.toString(treasureVariant),
                treasureSavegameVariable == null ? null : Integer.toString(treasureSavegameVariable),
                Integer.toString(priceField.getNumber()),
                dialogIdField.getText());
    }
}
