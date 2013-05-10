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
package org.solarus.editor.gui;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;

import java.util.*;
import org.solarus.editor.*;
import org.solarus.editor.Map;
import org.solarus.editor.entities.*;

/**
 * A component to set the view settings of the map editor,
 * i.e. what entities are displayed and how.
 */
public class MapViewSettingsPanel extends JPanel implements Observer {

    /**
     * The options visualised by this component.
     */
    private MapViewSettings settings;

    private JCheckBox showLowLayerCheckBox;
    private JCheckBox showIntermediateLayerCheckBox;
    private JCheckBox showHighLayerCheckBox;
    private JCheckBox showTransparencyCheckBox;
    private JCheckBox showGridCheckBox;
    private ZoomChooser zoomChooser;

    /**
     * Constructor.
     * @param settings The map view settings to visualise with this component.
     */
    public MapViewSettingsPanel(MapViewSettings settings) {
        super(new BorderLayout());

        this.settings = settings;

        zoomChooser = new ZoomChooser();
        JPanel boxesPanel = new JPanel(new GridLayout(2, 3));

        showLowLayerCheckBox = new JCheckBox("Show low layer");
        showLowLayerCheckBox.addItemListener(new ItemListenerLayer(Layer.LOW));
        boxesPanel.add(showLowLayerCheckBox);

        showIntermediateLayerCheckBox = new JCheckBox("Show intermediate layer");
        showIntermediateLayerCheckBox.addItemListener(new ItemListenerLayer(Layer.INTERMEDIATE));
        boxesPanel.add(showIntermediateLayerCheckBox);

        showHighLayerCheckBox = new JCheckBox("Show high layer");
        showHighLayerCheckBox.setSelected(settings.getShowLayer(Layer.HIGH));
        showHighLayerCheckBox.addItemListener(new ItemListenerLayer(Layer.HIGH));
        boxesPanel.add(showHighLayerCheckBox);

        showTransparencyCheckBox = new JCheckBox("Show transparency");
        showTransparencyCheckBox.addItemListener(new ItemListenerTransparency());
        boxesPanel.add(showTransparencyCheckBox);
        
        showGridCheckBox = new JCheckBox("Show grid");
        showGridCheckBox.addItemListener(new ItemListenerGrid());
        boxesPanel.add(showGridCheckBox);

        add(zoomChooser, BorderLayout.WEST);
        add(boxesPanel, BorderLayout.CENTER);

        update(settings, null);
        settings.addObserver(this);
    }

    /**
     * Called when settings have changed.
     */
    @Override
    public void update(Observable model, Object obj) {

        if (model instanceof MapViewSettings) {
            showLowLayerCheckBox.setSelected(settings.getShowLayer(Layer.LOW));
            showIntermediateLayerCheckBox.setSelected(settings.getShowLayer(Layer.INTERMEDIATE));
            showTransparencyCheckBox.setSelected(settings.getShowTransparency());
            showGridCheckBox.setSelected(settings.getShowGrid());
            zoomChooser.update();
        }
    }

    /**
     * Listener invoked when the state of a layer checkbox has changed.
     */
    private class ItemListenerLayer implements ItemListener {

        /**
         * The layer controlled by this checkbox.
         */
        private Layer layer;

        /**
         * Constructor.
         * @param layer the layer controlled by the checkbox.
         */
        public ItemListenerLayer(Layer layer) {
            this.layer = layer;
        }

        /**
         * Method invoked when the user clicks on the checkbox.
         */
        public void itemStateChanged(ItemEvent itemEvent) {

            // get the new checkbox state
            boolean show = (itemEvent.getStateChange() == ItemEvent.SELECTED);

            // update the options
            settings.setShowLayer(layer, show);
        }
    }

    /**
     * Listener invoked when the state of the grid checkbox has changed.
     */
    private class ItemListenerGrid implements ItemListener {
    	/**
    	 * Constructor.
    	 */
    	public ItemListenerGrid() {
    		
    	}
        /**
         * Method invoked when the user clicks on the checkbox.
         */
        public void itemStateChanged(ItemEvent itemEvent) {

            // get the new checkbox state
            boolean show = (itemEvent.getStateChange() == ItemEvent.SELECTED);

            // update the options
            settings.setShowGrid(show);
        }

    }
    /**
     * Listener invoked when the state of the transparency checkbox has changed.
     */
    private class ItemListenerTransparency implements ItemListener {

        /**
         * Constructor.
         */
        public ItemListenerTransparency() {
        }

        /**
         * Method invoked when the user clicks on the checkbox.
         */
        public void itemStateChanged(ItemEvent itemEvent) {

            // get the new checkbox state
            boolean show = (itemEvent.getStateChange() == ItemEvent.SELECTED);

            // update the options
            settings.setShowTransparency(show);
        }
    }

    /**
     * A component to select the zoom applied to the map view.
     */
    private class ZoomChooser extends JPanel implements ChangeListener {

        private final double[] zooms = {0.25, 0.5, 1, 2};

        private JSlider slider;

        public ZoomChooser() {
            super();

            setBorder(BorderFactory.createTitledBorder("Zoom"));

            slider = new JSlider(0, 3);
            add(slider, BorderLayout.CENTER);

            Hashtable<Integer, JLabel> labelTable = new Hashtable<Integer, JLabel>();
            labelTable.put(0, new JLabel("25%"));
            labelTable.put(1, new JLabel("50%"));
            labelTable.put(2, new JLabel("100%"));
            labelTable.put(3, new JLabel("200%"));
            slider.setLabelTable(labelTable);

            slider.setMajorTickSpacing(10);
            slider.setMinorTickSpacing(1);
            slider.setPaintTicks(true);
            slider.setPaintLabels(true);
            slider.setSnapToTicks(true);

            slider.addChangeListener(this);
        }

        public void update() {
            double zoom = settings.getZoom();
            for (int i = 0; i < zooms.length; i++) {
                if (zooms[i] == zoom) {
                    slider.setValue(i);
                }
            }
        }

        @Override
        public void stateChanged(ChangeEvent ev) {
            settings.setZoom(zooms[slider.getValue()]);
        }
    }
}

