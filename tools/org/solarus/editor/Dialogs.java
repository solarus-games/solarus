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

/**
 * This class describes game and editor dialogs.
 * Dialogs are observable.
 */
public class Dialogs extends Observable {

    /**
     * Id of the dialogs.
     */
    private String dialogsId;
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
     * @throws ZSDXException if the resource list could not be updated after the dialogs file creation
     */
    public Dialogs() throws ZSDXException {
        super();

        this.name = "New Dialogs";
        this.description = "";
        Resource dialogResource = Project.getResource(ResourceType.DIALOGS);

        sections = new ArrayList<DialogSection>();
        filteredSections = new ArrayList<DialogSection>();

        setChanged();
        notifyObservers();
    }

    /**
     * Loads an existing dialogs file.
     * @param dialogsId id of the dialogs file to load
     * @throws ZSDXException if the dialogs file could not be loaded
     */
    public Dialogs(String dialogsId) throws ZSDXException {
        this.dialogsId = dialogsId;
        this.sections = new ArrayList<DialogSection>();
        this.filteredSections = new ArrayList<DialogSection>();
        this.description = "";
        load();
    }

    /**
     * Loads an existing dialogs file.
     * @throws ZSDXException if the dialogs file could not be loaded
     */
    public void load() throws ZSDXException {
        String line =  "";
        try {
            // get the dialogs file name in the game resource database
            Resource mapResource = Project.getResource(ResourceType.DIALOGS);
            setName(mapResource.getElementName(dialogsId));

            File dialogsFile = Project.getDialogsFile(dialogsId);
            BufferedReader in = new BufferedReader(new FileReader(dialogsFile));


             line = in.readLine();

            if (line == null) {
                throw new ZSDXException("The dialogs file is empty");
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
            //System.out.println("We have " + getSections().size() + " sections interface this file");

        } catch (IOException ioe) {
            System.out.println(line);
            throw new ZSDXException(ioe.getMessage());
        }

    }

    public void save() throws ZSDXException {

        try {
            File dialogsFile = Project.getDialogsFile(dialogsId);
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
            throw new ZSDXException(er.getMessage());
        }

    }

    public boolean isSaved() {
        return saved;
    }

    public void setSaved(boolean saved) {
        this.saved = saved;
    }

    /**
     * Sets the name
     * @param name the new name
     */
    public void setName(String name) {
        this.name = name;
    }

    /**
     * Returns the name of the dialogs file
     * @return the name of the dialogs file
     */
    public String getName() {
        return name;
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
        System.out.println("On filtre sur " + filter);
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
