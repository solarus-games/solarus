package org.solarus.editor.gui;

import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Insets;

import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JTextField;

import org.solarus.editor.ResourceType;

/**
 * Component to build a new resource
 * with its name and its user-friendly name
 */
public class ResourceBuilderComponent extends JPanel {
    private static final long serialVersionUID = 1L;

    // Subcomponents
    private JTextField idField;
    private JTextField friendlyNameField;

    /**
     * Constructor
     * @param resourceType the kind of the resource we want to build
     * @param basepath the default path of the resource
     */
    public ResourceBuilderComponent(ResourceType resourceType, String basepath) {
        super(new GridBagLayout());

        GridBagConstraints gridBagConstraints = new GridBagConstraints();
        gridBagConstraints.insets = new Insets(5, 5, 5, 5); // margins
        gridBagConstraints.anchor = GridBagConstraints.LINE_START;
        gridBagConstraints.gridy = 0;

        // Resource id
        JLabel idLabel = new JLabel(resourceType.getName() + " id (filename):");
        gridBagConstraints.gridx = 0;
        add(idLabel, gridBagConstraints);
        gridBagConstraints.gridx = 1;
        idField = new JTextField(15);
        idField.setText(basepath.isEmpty() ? "" : basepath + "/");
        add(idField, gridBagConstraints);
        gridBagConstraints.gridy++;

        //Resource friendly name
        JLabel friendlyNameLabel = new JLabel(resourceType.getName() + " friendly name:");
        gridBagConstraints.gridx = 0;
        add(friendlyNameLabel, gridBagConstraints);
        gridBagConstraints.gridx = 1;
        friendlyNameField = new JTextField(15);
        add(friendlyNameField, gridBagConstraints);
        gridBagConstraints.gridy++;

    }

    public String getId() {
        return idField.getText();
    }

    public String getFriendlyName() {
        return friendlyNameField.getText();
    }


}
