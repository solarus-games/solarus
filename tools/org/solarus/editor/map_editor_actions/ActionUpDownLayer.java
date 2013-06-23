/*
 * Copyright (C) 2006-2013 Christopho, Solarus - http://www.solarus-games.org
 *
 * Solarus Quest Editor is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Solarus Quest Editor is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
package org.solarus.editor.map_editor_actions;

import java.util.Collection;
import java.util.LinkedList;
import org.solarus.editor.Map;
import org.solarus.editor.MapEditorAction;
import org.solarus.editor.QuestEditorException;
import org.solarus.editor.entities.Layer;
import org.solarus.editor.entities.MapEntity;

/**
 * Changing the layer of some entities to the superior or inferior level
 */
public class ActionUpDownLayer extends MapEditorAction {

    private LinkedList<MapEntity> entities;
    private Layer[] layersBefore;
    private Layer[] layersAfter;

    /**
     * Constructor.
     * @param map the map
     * @param entities the entities for which the layer will be changed
     * @param up if the layer after is superior or not than the layer before
     */
    public ActionUpDownLayer(Map map, Collection<MapEntity> entities, boolean up) {
        super(map);
        this.entities = new LinkedList<MapEntity>(entities);

        this.layersAfter = new Layer[entities.size()];
        this.layersBefore = new Layer[entities.size()];

        int i = 0;
        for (MapEntity entity : entities) {
            this.layersBefore[i] = entity.getLayer();
            this.layersAfter[i] = getLayerAfter(entity.getLayer(), up);
            i++;
        }
    }

    public final Layer getLayerAfter(Layer before, boolean up) {
        switch (before) {
            case LOW:
                if (up) {
                    return Layer.INTERMEDIATE;
                }
            case INTERMEDIATE:
                if (up) {
                    return Layer.HIGH;
                } else {
                    return Layer.LOW;
                }

            case HIGH:
                if (!up) {
                    return Layer.INTERMEDIATE;
                }
            default:
                return before;

        }
    }

    @Override
    public void execute() throws QuestEditorException {
        for (int i = 0; i < entities.size(); i++) {
            map.setEntityLayer(entities.get(i), layersAfter[i]);
        }
    }

    @Override
    public void undo() throws QuestEditorException {
        int i = 0;
        for (MapEntity entity: entities) {
            map.setEntityLayer(entity, layersBefore[i]);
            i++;
        }
    }
}
