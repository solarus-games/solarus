package zsdx.gui;

import javax.swing.*;

import zsdx.Configuration;

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
	configurationPanel = new ConfigurationPanel();
	configurationPanel.setBorder(BorderFactory.createTitledBorder("Configuration"));
    }
    
    /**
     * Returns the component to show in the dialog box.
     * @return the component to show in the dialog box
     */
    public JComponent getComponent() {
	return configurationPanel;
    }
    
    /**
     * Takes into account the modifications made by
     * the user (i.e. updates the ZSDX root path).
     */
    public void applyModifications() {
	Configuration.getInstance().setZsdxRootPath(configurationPanel.getZsdxRootPathEntered());
    }
}
