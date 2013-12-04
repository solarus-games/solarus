package org.solarus.editor.gui.tree;

import java.awt.datatransfer.Transferable;

import javax.lang.model.element.ElementKind;
import javax.swing.JComponent;
import javax.swing.JTree;
import javax.swing.TransferHandler;
import javax.swing.tree.DefaultMutableTreeNode;
import javax.swing.tree.DefaultTreeModel;
import javax.swing.tree.TreePath;

import org.solarus.editor.Project;
import org.solarus.editor.ResourceElement;
/**
 * Transfer handler to allow drag&drop on the QuestTree
 */
public class QuestTreeTransferHandler extends TransferHandler {
	private static final long serialVersionUID = 1L;

	public int getSourceActions(JComponent c)
	{
		return TransferHandler.MOVE;
	}
	protected Transferable createTransferable(JComponent c)
	{
		JTree tree = (JTree)c;
		DefaultMutableTreeNode selected = (DefaultMutableTreeNode)tree.getLastSelectedPathComponent();
		
		ResourceTransferable transferable = new ResourceTransferable(selected);
		
		return transferable;
		
	}
	public boolean canImport(TransferHandler.TransferSupport support)
	{
		return support.isDrop() && support.getTransferable().isDataFlavorSupported(ResourceTransferable.getResourceflavor());
	}
	public boolean importData(TransferSupport support) 
	{
		if (canImport(support)) {
			try {
				//Retrieve the transferable / the selected node
				Transferable t = support.getTransferable();
				DefaultMutableTreeNode dmt = (DefaultMutableTreeNode)t.getTransferData(ResourceTransferable.resourceFlavor);
								
				JTree.DropLocation dl = (JTree.DropLocation) support.getDropLocation();
				TreePath tp = dl.getPath();

				if (tp == null) {
					return false;
				}
				// Retrieve where we have dropped the element
				DefaultMutableTreeNode droppedIn = (DefaultMutableTreeNode)tp.getLastPathComponent();
				//Check that we aren't dropping the node in itself
				if (droppedIn == dmt) {
					return false;
				}
				//Check that we aren't trying to move a top-level directory
				if (!(dmt.getUserObject() instanceof ResourceElement)) {
					return false;
				}
				//Check we aren't dropping something on another category
				if (!droppedIn.getUserObject().getClass().equals(dmt.getUserObject().getClass())) {
					return false;
				}
				//Check we aren't dropping something on another category
				ResourceElement dmtResource = (ResourceElement)dmt.getUserObject();
				ResourceElement droppedInResource = (ResourceElement)droppedIn.getUserObject();
				if (dmtResource.type != droppedInResource.type) {
					return false;
				}
				//Check for parent
				if (droppedIn.isNodeAncestor(dmt)) {
					return false;
				}
				// Insert the node at the index where it has been dropped
				DefaultMutableTreeNode parent = (DefaultMutableTreeNode)droppedIn.getParent();
				parent.insert(dmt,parent.getIndex(droppedIn));				
				// Reloading of the tree model
				JTree tree = (JTree)support.getComponent();
				DefaultTreeModel model = (DefaultTreeModel)tree.getModel();
				model.reload(parent);
				//Change the resource index in the project data files and save it
				ResourceElement element = (ResourceElement)dmt.getUserObject();				
				Project.getResource(element.type).setElementIndex(element.id, parent.getIndex(dmt));
				Project.getResourceDatabase().save();
				
				return true;
			}
			catch(Exception ex) {
				ex.printStackTrace();
			}

		}
		return false;
	}
}	
