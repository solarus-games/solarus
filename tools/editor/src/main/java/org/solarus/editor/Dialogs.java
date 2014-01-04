/*
 * Copyright (C) 2006-2014 Christopho, Solarus - http://www.solarus-games.org
 *
 * Solarus Quest Editor is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Solarus Quest Editor is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
package org.solarus.editor;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.util.ArrayList;
import java.util.Observable;

/*
 * FIXME Dialogs are not working.
 * - All values of "skip" disappear from the dialog file when saving from the editor.
 *   (This makes all dialog of the quest become uninterruptible).
 * - Existing comments do not always stay at their original place in the file.
 * - There is no way to see if a line reaches the limit of characters.
 * - The format of dialogs has changed.
 */

/**
 * This class describes game and editor dialogs.
 * Dialogs are observable.
 */
public class Dialogs extends Observable {

    /**
     * Language of these dialogs.
     */
    private String languageId;

    /**
     * Name of the dialogs.
     */
    private String name;

    /**
     * Description of the dialogs
     */
    private String description;

    /**
     * Sections of the dialogs
     */
    private ArrayList<DialogSection> sections;

    /**
     * Sections of the dialogs
     */
    private ArrayList<DialogSection> filteredSections;

    /**
     *
     */
    private boolean saved;

    /**
     * Creates a new dialogs file.
     * @throws QuestEditorException if the resource list could not be updated
     * after the dialogs file creation
     */
    public Dialogs() throws QuestEditorException {
        super();

        /*
        this.name = "New Dialogs";
        this.description = "";
        Resource dialogResource = Project.getResource(ResourceType.LANGUAGE);

        sections = new ArrayList<DialogSection>();
        filteredSections = new ArrayList<DialogSection>();

        setChanged();
        notifyObservers();
        */

        // disable the support of dialogs for now since it's not working
        throw new QuestEditorException("Creating dialogs with the GUI is not working yet, please use a text editor");
    }

    /**
     * Loads an existing dialogs file.
     * @param languageId Languages of the dialogs to load.
     * @throws QuestEditorException if the dialogs file could not be loaded
     */
    public Dialogs(String languageId) throws QuestEditorException {
        this.languageId = languageId;
        this.sections = new ArrayList<DialogSection>();
        this.filteredSections = new ArrayList<DialogSection>();
        this.description = "";
        load();

        // disable the support of dialogs for now since it's not working
        throw new QuestEditorException("Editing dialogs with the GUI is not working yet, please use a text editor");
    }

    /**
     * Loads an existing dialogs file.
     * @throws QuestEditorException if the dialogs file could not be loaded
     */
    public void load() throws QuestEditorException {
        String line =  "";
        try {
            File dialogsFile = Project.getDialogsFile(languageId);
            BufferedReader in = new BufferedReader(new FileReader(dialogsFile));

            line = in.readLine();

            if (line == null) {
                in.close();
                throw new QuestEditorException("The dialogs file is empty");
            }
            // First step : read the file description.
            // It will ends at the start of the first section
            while (line != null && !line.startsWith("[")) {
                setDescription(getDescription() + line + "\n");
                line = in.readLine();
            }
            // Now current line is the first section name
            DialogSection currentSection = null;
            while (line != null) {
                if (line.startsWith("[")) {
                    //name
                    if (currentSection != null) {
                        //removing a \n on the coment
                        if(currentSection.getComment().length() > 2)
                        currentSection.setComment(currentSection.getComment().substring(0, currentSection.getComment().length() - 1));
                        sections.add(currentSection);
                    }
                    currentSection = new DialogSection(line.substring(1, line.length() - 1));
                } else if (line.startsWith("#")) {
                    currentSection.setComment(currentSection.getComment() + line + "\n");
                } else if (line.startsWith("icon")) {
                    currentSection.setIcon(Integer.parseInt(line.substring(7)));
                } else if (line.startsWith("line1")) {
                    currentSection.setLine1(line.substring(9, line.length() - 1));
                } else if (line.startsWith("line2")) {
                    currentSection.setLine2(line.substring(9, line.length() - 1));
                } else if (line.startsWith("line3")) {
                    currentSection.setLine3(line.substring(9, line.length() - 1));
                } else if (line.startsWith("question")) {
                    currentSection.setQuestion(true);
                } else if (line.startsWith("skip")) {
                    String skipAction = line.substring(8, line.length() - 1);
                    if (skipAction.equals("current")) {
                        currentSection.setSkipAction(DialogSection.SKIP_ACTION_CURRENT);
                    } else if (skipAction.equals("all")) {
                        currentSection.setSkipAction(DialogSection.SKIP_ACTION_ALL);
                    }
                } else if (line.startsWith("next2")) {
                    currentSection.setNextSection2(line.substring(8, line.length()));
                } else if (line.startsWith("next")) {
                    currentSection.setNextSection(line.substring(7, line.length()));
                }

                line = in.readLine();
            }

            in.close();
            //for the last section
            if(currentSection.getComment().length() > 2)
            currentSection.setComment(currentSection.getComment().substring(0, currentSection.getComment().length() - 1));
            sections.add(currentSection);
            setSaved(true);
            filterList("");

        } catch (IOException ioe) {
            throw new QuestEditorException(ioe.getMessage());
        }

    }

    public void save() throws QuestEditorException {

        try {
            File dialogsFile = Project.getDialogsFile(languageId);
            OutputStreamWriter ost = new OutputStreamWriter(new FileOutputStream(dialogsFile), "UTF-8");
            BufferedWriter out = new BufferedWriter(ost);
            out.write(description);
            for (DialogSection section : sections) {
                out.write(section.fileVersion());
                out.write("\n\n");
            }
            out.close();
            saved = true;
        } catch (IOException er) {
            throw new QuestEditorException(er.getMessage());
        }

    }

    public boolean isSaved() {
        return saved;
    }

    public void setSaved(boolean saved) {
        this.saved = saved;
    }

    /**
     * Returns the human-readable name of the language.
     * @return The name of the language.
     */
    public String getLanguageName() {

        String languageName = "";
        try {
            Resource languages = Project.getResource(ResourceType.LANGUAGE);
            languageName = languages.getElementName(languageId);
        }
        catch (QuestEditorException ex) {
            // Should not happen.
            ex.printStackTrace();
        }
        
        return languageName;
    }

    /**
     * @return the sections
     */
    public ArrayList<DialogSection> getSections() {
        return sections;
    }

    /**
     * @param sections the sections to set
     */
    public void setSections(ArrayList<DialogSection> sections) {
        this.sections = sections;
    }

    /**
     *
     */
    public String toString() {
        return name;
    }

    public String getLanguageId() {
        return languageId;
    }

    /**
     * @return the description
     */
    public String getDescription() {
        return description;
    }

    /**
     * @param description the description to set
     */
    public void setDescription(String description) {
        this.description = description;
    }

    public void filterList(String filter) {
        filteredSections.clear();
        if (filter.length() == 0) {
            filteredSections.addAll(sections);
        } else {
            for (DialogSection section : sections) {
                if (section.getName().contains(filter)) {
                    filteredSections.add(section);
                }
            }
        }
         setChanged();
         notifyObservers();
    }
}
