package zsdx.gui;

import javax.swing.*;
import zsdx.*;

/**
 * A dialog box to set the configuration.
 */
public class ConfigurationDialog extends OkCancelDialog {

    // the component 
    private ConfigurationPanel configurationPanel;
    
    /**
     * Constructor.
     */
    public ConfigurationDialog() {
	super("ZSDX configuration", true);
    }
    
    /**
     * Creates and returns the component to show in the dialog box.
     * @return the component to show in the dialog box
     */
    public JComponent createComponent() {
	configurationPanel = new ConfigurationPanel();
	configurationPanel.setBorder(BorderFactory.createTitledBorder("Configuration"));
	return configurationPanel;
    }
    
    /**
     * Takes into account the modifications made by
     * the user (i.e. updates the ZSDX root path).
     */
    public void applyModifications() throws ZSDXException {
	Configuration.setZsdxRootPath(configurationPanel.getZsdxRootPathEntered());
    }
}
