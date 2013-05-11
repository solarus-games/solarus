/*
 * Copyright (C) 2006-2012 Christopho, Solarus - http://www.solarus-games.org
 *
 * Solarus Quest Editor is free software; you can redistribute it and/or modify
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

import java.awt.*;
import java.io.*;
import javax.swing.*;
import javax.swing.event.*;

/**
 * A simple editor using for editing any resources as a text file.
 */
public class TextEditorPanel extends AbstractEditorPanel implements DocumentListener {

    /**
     * The file open in the editor.
     */
    private File file;

    /**
     * The text area use for the edition.
     */
    private JTextArea textArea;

    /**
     * A boolean used for checking modifications in the content of the text area.
     */
    private boolean textChanged;

    /**
     * Creates a text editor.
     * @param mainWindow The main window of the quest editor.
     * @param file The file to edit.
     */
    public TextEditorPanel(EditorWindow mainWindow, File file) {
        super(mainWindow, getEditorId(file));
        setLayout(new BorderLayout());

        textArea = new JTextArea();
        textArea.setFont(new Font("Courier New", Font.PLAIN, 12));
        textArea.getDocument().addDocumentListener(this);
        JScrollPane scroller = new JScrollPane(textArea);
        add(scroller, BorderLayout.CENTER);

        setFile(file);
    }

    /**
     * Returns the id of any map editor that edits the specified text file.
     * @param file A text file.
     * @return Id of an editor that edits this text file.
     */
    public static String getEditorId(File file) {
        return file.getAbsolutePath();
    }

    /**
     * Sets the file to edit in this editor.
     * @param file The file to edit in this editor.
     */
    public void setFile(File file) {
        this.file = file;

        if (file == null) {
            return;
        }

        String text = "";
        try {
            BufferedReader br = new BufferedReader(
               new InputStreamReader(
                     new FileInputStream(file), "UTF-8"));
            String line = br.readLine();

            while (line != null) {
                text += line + System.lineSeparator();
                line = br.readLine();
            }
            br.close();
            textArea.setText(text);
        } catch (Exception err) {
            err.printStackTrace();
        }
        textChanged = false;
    }

    /**
     * Saves the current resource of this editor.
     */
    @Override
    public void save() {

        try {
            OutputStreamWriter ost = new OutputStreamWriter(new FileOutputStream(file) , "UTF-8");
            BufferedWriter out = new BufferedWriter(ost);
            out.write(textArea.getText());
            out.close();
            textChanged = false;
        }
        catch (IOException ex) {
            ex.printStackTrace();
        }
    }

    @Override
    public String getTitle() {
        return file == null ? "" : file.getName();
    }

    @Override
    public boolean checkCurrentFileSaved() {
        boolean result = true;

        if (textChanged) {
            int answer = JOptionPane.showConfirmDialog(this,
                    "The file has been modified. Do you want to save it?",
                    "Save the modifications",
                    JOptionPane.YES_NO_CANCEL_OPTION,
                    JOptionPane.WARNING_MESSAGE);
            if (answer == JOptionPane.YES_OPTION) {
                save();
            } else if (answer == JOptionPane.CANCEL_OPTION) {
                result = false;
            }
        }

        return result;
    }

    /**
     * Closes this editor without confirmation. 
     */
    @Override
    public void close() {
        setFile(null);
    }

    // Methods for checking modifications interface the text area.

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
