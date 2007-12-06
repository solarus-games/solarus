package editors;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;

/**
 * A dialog box with a combo box to choose a tileset.
 */
public class TilesetChooserDialog extends JDialog {

    /**
     * Name of the tileset selected (or null if the user cancelled).
     */
    private String tilesetName;

    /**
     * Constructor.
     */
    public TilesetChooserDialog() {

	super((Frame) null, "Select a tileset", true);

	Container contentPane = getContentPane();
	contentPane.setLayout(new BoxLayout(contentPane, BoxLayout.PAGE_AXIS));
	setMinimumSize(new Dimension(300, 0));
	setResizable(false);
	
	final TilesetChooser tilesetChooser = new TilesetChooser(false);

	JPanel tilesetChooserPanel = new JPanel();
	tilesetChooserPanel.add(tilesetChooser);
	tilesetChooserPanel.setBorder(BorderFactory.createTitledBorder("Please select a tileset"));

	JButton buttonOK = new JButton("OK");
	buttonOK.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent ev) {
		    tilesetName = (String) tilesetChooser.getSelectedItem();
		    dispose();
		}
	    });

	tilesetChooser.addKeyListener(new KeyAdapter() {
		public void keyPressed(KeyEvent ev) {
		    if (ev.getKeyCode() == KeyEvent.VK_ENTER) {
			tilesetName = (String) tilesetChooser.getSelectedItem();
			dispose();
		    }
		}
	    });

	tilesetChooser.addKeyListener(new KeyAdapter() {
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
     * Returns the name of tileset that the user has just selected.
     * @return the name of the tileset, or null if he cancelled
     */
    public String getTilesetName() {
	String name = tilesetName;
	tilesetName = null;
	return name;
    }
}
