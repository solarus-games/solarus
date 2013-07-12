/*
 * Copyright (C) 2006-2013 Christopho, Solarus - http://www.solarus-games.org
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
package org.solarus.editor.gui;

import org.solarus.editor.*;
import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import java.io.*;
import org.luaj.vm2.*;

/**
 * A dialog that runs an external Lua script and shows its output.
 */
public class ExternalLuaScriptDialog extends JDialog {

    /**
     * The script to run (without extension).
     */
    private String fileName;

    /**
     * Argument to pass to the script or null.
     */
    private String arg;

    /**
     * Label indicating the status of running the script.
     */
    private JLabel statusLabel;

    /**
     * Text area containing the output of the script.
     */
    private JTextArea textArea;

    /**
     * Button to close the dialog when the script is finished.
     */
    private JButton okButton;

    /**
     * Indicates that the script has finished running.
     */
    private boolean finished;

    /**
     * Indicates whether the script suceeded.
     */
    private boolean success = false;

    /**
     * Constructor.
     * @param title Title of the dialog box.
     * @param fileName The script to run, without extension.
     * @param arg An argument to pass to the script, or null.
     */
    public ExternalLuaScriptDialog(String title, String fileName, String arg) {

        super((Frame) null, title, true);

        this.fileName = fileName;
        this.arg = arg;
        this.finished = false;

        Container contentPane = getContentPane();
        contentPane.setLayout(new BoxLayout(contentPane, BoxLayout.PAGE_AXIS));
        setResizable(false);
        setDefaultCloseOperation(DO_NOTHING_ON_CLOSE);

        // Description.
        JLabel descriptionLabel = new JLabel(title + "...");
        statusLabel = new JLabel("In progress");
        statusLabel.setForeground(new Color(255, 128, 0));

        JPanel topPanel = new JPanel();
        topPanel.setLayout(new BoxLayout(topPanel, BoxLayout.LINE_AXIS));

        topPanel.add(Box.createHorizontalStrut(20));
        topPanel.add(descriptionLabel);
        topPanel.add(Box.createHorizontalStrut(10));
        topPanel.add(statusLabel);
        topPanel.add(Box.createHorizontalGlue());

        // Script output.
        textArea = new JTextArea(30, 80);
        JScrollPane scrollPane = new JScrollPane(textArea);
        JPanel outputPanel = new JPanel();
        outputPanel.add(scrollPane);
        outputPanel.setBorder(BorderFactory.createTitledBorder("Status"));

        // OK button.
        okButton = new JButton("        OK        ");
        okButton.setEnabled(false);
        okButton.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent ev) {
                if (finished) {
                    dispose();
                }
            }
        });

        JPanel bottomPanel = new JPanel();
        bottomPanel.setLayout(new BoxLayout(bottomPanel, BoxLayout.LINE_AXIS));

        bottomPanel.add(Box.createHorizontalGlue());
        bottomPanel.add(okButton);
        bottomPanel.add(Box.createHorizontalStrut(20));

        getContentPane().add(Box.createVerticalStrut(10));
        getContentPane().add(topPanel);
        getContentPane().add(Box.createVerticalStrut(20));
        getContentPane().add(outputPanel);
        getContentPane().add(Box.createVerticalStrut(20));
        getContentPane().add(bottomPanel);
        getContentPane().add(Box.createVerticalStrut(10));
    }

    /**
     * Displays the dialog box.
     * @return true in the script was successful, false otherwise.
     */
    public boolean display() {
        new ScriptRunner().start();
        new LogFileReader().start();

        setLocationRelativeTo(null);
        pack();
        setVisible(true);
        return success;
    }

    /**
     * Allows the user to close the dialog.
     */
    private void setFinished() {
        okButton.setEnabled(true);
        setDefaultCloseOperation(DISPOSE_ON_CLOSE);
        finished = true;
        if (success) {
            statusLabel.setText("Success!");
            statusLabel.setForeground(new Color(0, 128, 0));
        }
        else {
            statusLabel.setText("Failure");
            statusLabel.setForeground(Color.red);
        }
    }

    private class ScriptRunner extends Thread {

        public void run() {
            try {
                LuaTools.runScript(fileName, arg, fileName + ".log");
                success = true;
            }
            catch (LuaError ex) {
            }
            setFinished();
        }
    }

    private class LogFileReader extends Thread {

        public void run() {
            try {
                int totalLinesRead = 0;
                while (!finished) {
                    Thread.sleep(200);
                    BufferedReader buffer = new BufferedReader(new InputStreamReader(new FileInputStream(fileName + ".log")));
                    String line;

                    int linesRead = 0;
                    while ((line = buffer.readLine()) != null) {
                        if (linesRead >= totalLinesRead) {
                            textArea.append(line);
                            textArea.append("\n");
                            totalLinesRead++;
                        }
                        linesRead++;
                    }
                }
            }
            catch (Exception ex) {
                textArea.append("Error: " + ex.getMessage());
            }
        }
    }
}

