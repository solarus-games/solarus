package org.solarus.editor.gui.tree;

import java.awt.datatransfer.DataFlavor;
import java.awt.datatransfer.Transferable;
import java.awt.datatransfer.UnsupportedFlavorException;
import java.io.IOException;
import java.util.Arrays;

import javax.swing.tree.DefaultMutableTreeNode;
/**
 * Transferable object for the Resource in the QuestTree
 */
public class ResourceTransferable implements Transferable {
	protected static final DataFlavor resourceFlavor = new DataFlavor(DataFlavor.javaJVMLocalObjectMimeType,"ResourceNode");
	protected DefaultMutableTreeNode node;

	
	public ResourceTransferable(DefaultMutableTreeNode node) {
		this.node = node;
	}
	
	@Override
	public DataFlavor[] getTransferDataFlavors() {
		DataFlavor[] result = { resourceFlavor };
		return result;
	}

	@Override
	public boolean isDataFlavorSupported(DataFlavor flavor) {
		return Arrays.asList(getTransferDataFlavors()).contains(flavor);	}

	@Override
	public Object getTransferData(DataFlavor flavor)
			throws UnsupportedFlavorException, IOException {
		if (flavor.equals(resourceFlavor)) {
			return node;
		}
		else {
			throw new UnsupportedFlavorException(flavor);
		}
	}
	public static DataFlavor getResourceflavor() {
		return resourceFlavor;
	}

}
