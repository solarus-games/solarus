package zsdx;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;

/**
 * A dialog box to set the configuration.
 */
public class ConfigurationDialog extends JDialog {

    /**
     * The configuration object.
     */
    private Configuration configuration;

    /**
     * Constructor.
     */
    public ConfigurationDialog() {
	super((Frame) null, "ZSDX configuration", true);

	this.configuration = Configuration.getInstance();
	setResizable(false);
	((BorderLayout) getLayout()).setHgap(10);
	
	final ConfigurationPanel configurationPanel = new ConfigurationPanel();
	configurationPanel.setBorder(BorderFactory.createTitledBorder("Configuration"));

	JButton buttonOK = new JButton("OK");
	buttonOK.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent e) {
		    configuration.setZsdxRootPath(configurationPanel.getZsdxRootPathEntered());
		    dispose();
		}
	    });

	JButton buttonCancel = new JButton("Cancel");
	buttonCancel.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent e) {
		    dispose();
		}
	    });

	JButton buttonApply = new JButton("Apply");
	buttonApply.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent e) {
		    configuration.setZsdxRootPath(configurationPanel.getZsdxRootPathEntered());
		}
	    });
	
	GridLayout gridLayout = new GridLayout(1, 3); 
	JPanel buttonPanel = new JPanel(gridLayout);
	gridLayout.setHgap(10);

	buttonPanel.add(buttonOK);
	buttonPanel.add(buttonCancel);
	buttonPanel.add(buttonApply);

	JPanel bottomPanel = new JPanel();
	bottomPanel.setLayout(new BoxLayout(bottomPanel, BoxLayout.LINE_AXIS));

	bottomPanel.add(Box.createHorizontalGlue());
	bottomPanel.add(buttonPanel);

	add(configurationPanel, BorderLayout.CENTER);
	add(bottomPanel, BorderLayout.SOUTH);
    }
    
}
