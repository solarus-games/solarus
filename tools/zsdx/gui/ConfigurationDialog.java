package zsdx.gui;

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
	configurationPanel = new ConfigurationPanel();
	setBorderTitle("Configuration");
	setComponent(configurationPanel);
    }
    
    /**
     * Takes into account the modifications made by
     * the user (i.e. updates the ZSDX root path).
     */
    public void applyModifications() throws ZSDXException {
	Configuration.setZsdxRootPath(configurationPanel.getZsdxRootPathEntered());
    }
}
