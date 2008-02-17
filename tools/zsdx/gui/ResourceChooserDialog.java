package zsdx.gui;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;

/**
 * A dialog box with a combo box to choose a resource element.
 */
public class ResourceChooserDialog extends JDialog {

    /**
     * Id of the element chosen (or an empty string if the user cancelled).
     */
    private String id = "";

    /**
     * Constants strings describing each resource type.
     */
    private static String[] resourceNames = {
	"map", "tileset", "music"
    };

    /**
     * Constructor.
     */
    public ResourceChooserDialog(int resourceType) {

	super((Frame) null, "Select a " + resourceNames[resourceType], true);

	Container contentPane = getContentPane();
	contentPane.setLayout(new BoxLayout(contentPane, BoxLayout.PAGE_AXIS));
	setMinimumSize(new Dimension(300, 0));
	setResizable(false);
	
	final ResourceChooser chooser = new ResourceChooser(resourceType, false);

	JPanel tilesetChooserPanel = new JPanel();
	tilesetChooserPanel.add(chooser);
	tilesetChooserPanel.setBorder(BorderFactory.createTitledBorder("Please select a " + resourceNames[resourceType]));

	JButton buttonOK = new JButton("OK");
	buttonOK.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent ev) {
		    id = chooser.getSelectedId();
		    dispose();
		}
	    });

	chooser.addKeyListener(new KeyAdapter() {
		public void keyPressed(KeyEvent ev) {
		    if (ev.getKeyCode() == KeyEvent.VK_ENTER) {
			id = chooser.getSelectedId();
			dispose();
		    }
		}
	    });

	chooser.addKeyListener(new KeyAdapter() {
		public void keyPressed(KeyEvent ev) {
		    if (ev.getKeyCode() == KeyEvent.VK_ESCAPE) {
			dispose();
		    }
		}
	    });

	JButton buttonCancel = new JButton("Cancel");
	buttonCancel.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent ev) {
		    dispose();
		}
	    });

	GridLayout gridLayout = new GridLayout(1, 2); 
	JPanel buttonPanel = new JPanel(gridLayout);
	gridLayout.setHgap(10);

	buttonPanel.add(buttonCancel);
	buttonPanel.add(buttonOK);

	JPanel bottomPanel = new JPanel();
	bottomPanel.setLayout(new BoxLayout(bottomPanel, BoxLayout.LINE_AXIS));

	bottomPanel.add(Box.createHorizontalGlue());
	bottomPanel.add(buttonPanel);

	add(tilesetChooserPanel);
	add(Box.createVerticalStrut(20));
	add(bottomPanel);
	add(Box.createVerticalStrut(10));
    }

    /**
     * Returns the id of the element that the user has just selected.
     * @return the id of the element, or -1 if he cancelled
     */
    public String getSelectedId() {

	String selectedId = this.id;
	this.id = "";
	return selectedId;
    }
}
