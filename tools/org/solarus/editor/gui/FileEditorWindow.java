/*
 * Copyright (C) 2009 Christopho, Zelda Solarus - http://www.zelda-solarus.com
 *
 * Zelda: Mystery of Solarus DX is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Zelda: Mystery of Solarus DX is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
package org.solarus.editor.gui;

import java.awt.BorderLayout;
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.util.Observable;
import javax.swing.JOptionPane;
import javax.swing.JScrollPane;
import javax.swing.JTextArea;
import javax.swing.event.DocumentEvent;
import javax.swing.event.DocumentListener;
import org.solarus.editor.Project;
import org.solarus.editor.ProjectObserver;

/**
 * A simple editor using for editing all quest resources as text files
 */
public class FileEditorWindow extends AbstractEditorWindow implements ProjectObserver, DocumentListener {

    private EditorWindow parentEditor;
    /**
     * The file opened in the editor
     */
    private File file;
    /**
     * The TextArea use for the edition
     */
    private JTextArea textArea;
    /**
     * A boolean used for checking modifications in thhe content of the TextArea
     */
    private boolean textChanged;

    /**
     * Creates a new window.
     */
    public FileEditorWindow(String quest, EditorWindow parentEditor) {
        setLayout(new BorderLayout());
        Project.addProjectObserver(this);
        this.parentEditor = parentEditor;
        // set a nice look and feel
        GuiTools.setLookAndFeel();

        textArea = new JTextArea();
        textArea.getDocument().addDocumentListener(this);
        JScrollPane jsp = new JScrollPane(textArea);
        add(jsp, BorderLayout.CENTER);
    }

    /**
     * Sets the file edited as text in the ediotr
     * @param f the file wich will be edited in the textarea
     */
    public void setFile(File f) {
        this.file = f;

        String text = "";
        try {
            BufferedReader br = new BufferedReader(new FileReader(file));
            String line = br.readLine();

            while (line != null) {
                text += line + "\n";
                line = br.readLine();
            }
            br.close();
            textArea.setText(text);
        } catch (Exception err) {
            err.printStackTrace();
        }
    }

    /**
     * Saves the contennt of the text area into the file
     */
    public void saveFile() {
        try {
            FileWriter lu = new FileWriter(file);
            BufferedWriter out = new BufferedWriter(lu);
            out.write(textArea.getText());
            out.close();
            textChanged = false;
        } catch (IOException er) {
            ;
        }
    }

    public String getResourceName() {
        return file.getName();
    }

    public boolean checkCurrentFileSaved() {
        boolean result = true;

        if (textChanged) {
            int answer = JOptionPane.showConfirmDialog(this,
                    "The file has been modified. Do you want to save it?",
                    "Save the modifications",
                    JOptionPane.YES_NO_CANCEL_OPTION,
                    JOptionPane.WARNING_MESSAGE);
            if (answer == JOptionPane.YES_OPTION) {
                saveFile();
            } else if (answer == JOptionPane.CANCEL_OPTION) {
                result = false;
            }
        }

        return result;
    }

    /**
     * This function is called when the tileset changes.
     * @param o the history
     * @param obj additional parameter
     */
    public void update(Observable o, Object obj) {
        this.parentEditor.update(o, obj);
    }

    /**
     * This method is called when a project has just been loaded.
     * The tileset menu is enabled.
     */
    public void currentProjectChanged() {
    }

    //
    //Methods for checking modifications interface the text area
    //
    /**
     * Gives notification that there was an insert into the document.
     * The range given by the DocumentEvent bounds the freshly inserted region.
     * @param e the document event
     */
    public void insertUpdate(DocumentEvent e) {
        textChanged = true;

    }

    /**
     * Gives notification that a portion of the document has been removed.
     * The range is given in terms of what the view last saw (that is, before updating sticky positions).
     * @param e the document event
     */
    public void removeUpdate(DocumentEvent e) {
        textChanged = true;

    }

    /**
     * Gives notification that an attribute or set of attributes changed.
     * @param e the document event
     */
    public void changedUpdate(DocumentEvent e) {
        textChanged = true;

    }
}
