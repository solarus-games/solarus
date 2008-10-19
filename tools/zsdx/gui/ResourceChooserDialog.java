package zsdx.gui;

import zsdx.*;

/**
 * A dialog box with a combo box to choose a resource element.
 */
public class ResourceChooserDialog extends OkCancelDialog {

    /**
     * Id of the element chosen (or an empty string if the user cancelled).
     */
    private String id = "";
    
    /**
     * The component shown in the dialog box.
     */
    private ResourceChooser chooser;

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
	super("Select a " + resourceNames[resourceType], false);
    
	setBorderTitle("Please select a " + resourceNames[resourceType]);
    	chooser = new ResourceChooser(resourceType, false);
    	setComponent(chooser);
    }

    /**
     * Takes into account the modifications made by the user.
     */
    public void applyModifications() throws ZSDXException {
	id = chooser.getSelectedId();
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
