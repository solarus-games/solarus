package zsdx.gui;

import javax.swing.*;
import zsdx.*;

/**
 * A combo box component to select an element from the resource database.
 */
public class ResourceChooser extends JComboBox implements ProjectObserver {

    /**
     * The kind of resource displayed in the combo box (maps, musics...).
     */
    private int resourceType;
    
    /**
     * Indicates whether the first element of the combo box should be
     * an empty option (showing that no element is selected).
     */
    private boolean showEmptyOption;

    /**
     * Constructor.
     * @param resourceType the kind of resource displayed in the combo box:
     * ResourceDatabase.RESOURCE_MAPS, ResourceDatabase.RESOURCE_MUSICS, etc.
     */
    public ResourceChooser(int resourceType, boolean showEmptyOption) {

	super();
	this.resourceType = resourceType;
	this.showEmptyOption = showEmptyOption;

	Project.addProjectObserver(this);
	
	buildList();
    }

    /**
     * Loads the list using the game resource database.
     */
    protected void buildList() {

	if (!Project.isLoaded()) {
	    return;
	}
	
	if (showEmptyOption) {
	    addItem(new KeyValue("", "          "));
	}

	try {
	    Resource resource = Project.getResource(resourceType);
	    String[] ids = resource.getIds();
	    String name;
	    
	    for (int i = 0; i < ids.length; i++) {
		name = resource.getElementName(ids[i]);
		addItem(new KeyValue(ids[i], name));
	    }
	}
	catch (ZSDXException ex) {
	    WindowTools.errorDialog("Unexpected error: " + ex.getMessage());
	    System.exit(1);
	}
    }

    /**
     * Reloads the list using the game resource database.
     * After the list is reloaded, the same item is selected again if it still exists.
     */
    protected void reloadList() {
	
	String selectedId = getSelectedId();

	removeAllItems();
	buildList();
	
	if (selectedId.length() > 0) {
	    setSelectedId(selectedId);
	}
    }
    
    /**
     * Returns the id of the currently selected element.
     * @return the id of the selected element, or an empty string if no element is selected
     */
    public String getSelectedId() {

	KeyValue item = (KeyValue) getSelectedItem();

	if (item == null) {
	    return "";
	}

	return item.getKey();
    }

    /**
     * Changes the currently selected element.
     * If the specified element doesn't exist, no exception is raised and
     * the selection is not changed.
     * @param id id of the element you want to make selected in the combo box,
     * or an empty string to select no element
     */
    public void setSelectedId(String id) {

	KeyValue item = new KeyValue(id, null);
	setSelectedItem(item);
    }

    /**
     * This method is called when a project has just been loaded.
     * The resource list is then loaded.
     */
    public void currentProjectChanged() {
	reloadList();
    }
}
