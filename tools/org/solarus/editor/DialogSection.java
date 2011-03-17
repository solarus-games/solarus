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

import java.util.Observable;

/**
 * This class describes a section of an INI file use for game and engine dialogs
 */
public class DialogSection extends Observable {

    public static final String SKIP_ACTION_NONE = "None";
    public static final String SKIP_ACTION_CURRENT = "Current";
    public static final String SKIP_ACTION_ALL = "All";
    /** Name of the current section */
    private String name;
    /** Comment about the current section */
    private String comment;
    /** Icon used for the current section */
    private int icon;
    /** Lines of the current section */
    private String line1, line2, line3;
    /** Indicates if the current section is a question */
    private boolean question;
    /** Action on user cancellation */
    private String skipAction;
    /** Name of the next section */
    private String nextSection;
    /** Name of the next section */
    private String nextSection2;

    public DialogSection(String name) {
        this.name = name;
        this.comment = "";
        this.icon = -1;
        this.line1 = "";
        this.line2 = "";
        this.line3 = "";
        this.question = false;
        this.skipAction = SKIP_ACTION_NONE;
        this.nextSection = "";
        this.nextSection2 = "";
    }

    /**
     * @return the name
     */
    public String getName() {
        return name;
    }

    /**
     * @param name the name to set
     */
    public void setName(String name) {
        this.name = name;
    }

    /**
     * @return the comment
     */
    public String getComment() {
        return comment;
    }

    /**
     * @param comment the comment to set
     */
    public void setComment(String comment) {
        this.comment = comment;
    }

    /**
     * @return the icon
     */
    public int getIcon() {
        return icon;
    }

    /**
     * @param icon the icon to set
     */
    public void setIcon(int icon) {
        this.icon = icon;
    }

    /**
     * @return the line1
     */
    public String getLine1() {
        return line1;
    }

    /**
     * @param line1 the line1 to set
     */
    public void setLine1(String line1) {
        this.line1 = line1;
    }

    /**
     * @return the lin2
     */
    public String getLine2() {
        return line2;
    }

    /**
     * @param lin2 the lin2 to set
     */
    public void setLine2(String line2) {
        this.line2 = line2;
    }

    /**
     * @return the line3
     */
    public String getLine3() {
        return line3;
    }

    /**
     * @param line3 the line3 to set
     */
    public void setLine3(String line3) {
        this.line3 = line3;
    }

    /**
     * @return the question
     */
    public boolean isQuestion() {
        return question;
    }

    /**
     * @param question the question to set
     */
    public void setQuestion(boolean question) {
        this.question = question;
    }

    /**
     * @return the skip
     */
    public String getSkipAction() {
        return skipAction;
    }

    /**
     * @param skip the skip to set
     */
    public void setSkipAction(String skipAction) {
        this.skipAction = skipAction;
    }

    /**
     * @return the nextSection
     */
    public String getNextSection() {
        return nextSection;
    }

    /**
     * @param nextSection the nextSection to set
     */
    public void setNextSection(String nextSection) {
        this.nextSection = nextSection;
    }

    /**
     * @return the nextSection
     */
    public String getNextSection2() {
        return nextSection2;
    }

    /**
     * @param nextSection the nextSection to set
     */
    public void setNextSection2(String nextSection2) {
        this.nextSection2 = nextSection2;
    }

    /**
     * Return the name - using for the GUI Editor
     */
    public String toString() {
        return name;
    }

    public String fileVersion() {
        StringBuffer result = new StringBuffer("["+name+"]");
        if (comment.length() > 0) {
            result.append("\n").append(comment);
        }
        if (icon >= 0) {
            result.append("\nicon = ").append(icon+"");
        }
        if (line1.length() > 0) {
            result.append("\nline1 = \"").append(line1).append("\"");
        }
        if (line2.length() > 0) {
            result.append("\nline2 = \"").append(line2).append("\"");
        }
        if (line3.length() > 0) {
            result.append("\nline3 = \"").append(line3).append("\"");
        }
        if (question) {
            result.append("\n").append("question = true");
        }
        if(skipAction.equals(SKIP_ACTION_CURRENT)) {
            result.append("\n").append("skip = current");
        } else if(skipAction.equals(SKIP_ACTION_ALL)) {
            result.append("\n").append("skip = all");
        }
        if (nextSection.length() > 0) {
            result.append("\nnext = ").append(nextSection);
        }
        if (nextSection2.length() > 0) {
            result.append("\nnext2 = ").append(nextSection2);
        }
        return result.toString();
    }

    public boolean checkSectionValidity() {

        return true;
    }

    public boolean equals(DialogSection other) {
        return name.equals(other.name);
    }
}
