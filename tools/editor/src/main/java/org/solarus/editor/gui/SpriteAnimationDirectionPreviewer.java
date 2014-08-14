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
package org.solarus.editor.gui;

import java.awt.*;
import java.awt.event.*;
import java.util.*;

import javax.swing.*;

import org.solarus.editor.*;

/**
 * Component to preview a direction (animated).
 */
public class SpriteAnimationDirectionPreviewer extends JPanel implements Observer {

    /**
     * The sprite observed.
     */
    private Sprite sprite = null;

    /**
     * The animation observed.
     */
    private SpriteAnimation selectedAnimation = null;

    /**
     * The direction observed.
     */
    private SpriteAnimationDirection selectedDirection = null;

    // the image panel
    final private ImageView imageView;

    // current displayed frame view
    final private JLabel frameView;

    // buttons
    final private JButton startButton;
    final private JButton stopButton;
    final private JButton nextButton;
    final private JButton prevButton;
    final private JButton firstButton;
    final private JButton lastButton;

    // checkbox to draw origin point or not
    final private JCheckBox drawOrigin;

    // icons of start button
    final private ImageIcon startIcon;
    final private ImageIcon pauseIcon;

    /**
     * Constructor.
     */
    public SpriteAnimationDirectionPreviewer() {

        setLayout(new GridBagLayout());
        setBorder(BorderFactory.createEmptyBorder(5, 5, 5, 5));

        GridBagConstraints constraints = new GridBagConstraints();
        constraints.insets = new Insets(5, 5, 5, 5); // margins
        constraints.anchor = GridBagConstraints.PAGE_START; // alignment of the components

        // image view
        imageView = new ImageView();
        JPanel imageViewPanel = new JPanel(new BorderLayout());
        imageViewPanel.add(imageView);
        imageViewPanel.setBorder(BorderFactory.createEtchedBorder());

        constraints.gridx = 0;
        constraints.gridy = 0;
        constraints.weightx = 1.0;
        constraints.weighty = 1.0;
        add(imageViewPanel, constraints);

        // buttons row
        JPanel buttonsPanel = new JPanel(new GridBagLayout());

        GridBagConstraints constraints2 = new GridBagConstraints();
        constraints2.insets = new Insets(5, 5, 5, 5); // margins
        constraints2.anchor = GridBagConstraints.CENTER; // alignment of the components

        // previous button
        prevButton = new JButton(Project.getEditorImageIconOrEmpty("icon_previous.png"));
        prevButton.setToolTipText("Previous frame");
        prevButton.addActionListener(new ActionListener() {

            @Override
            public void actionPerformed(ActionEvent ae) {

                imageView.setCurrentFrame(imageView.getCurrentFrame() - 1);
                update(selectedDirection, null);
            }
        });
        constraints2.gridx = 0;
        constraints2.gridy = 0;
        constraints2.gridwidth = 1;
        constraints2.weightx = 0.0;
        constraints2.weighty = 0.0;
        buttonsPanel.add(prevButton, constraints2);

        // frame view
        frameView = new JLabel("0/0");
        constraints2.gridx = 1;
        constraints2.gridy = 0;
        constraints2.gridwidth = 2;
        constraints2.weightx = 0.0;
        constraints2.weighty = 0.0;
        buttonsPanel.add(frameView, constraints2);

        // next button
        nextButton = new JButton(Project.getEditorImageIconOrEmpty("icon_next.png"));
        nextButton.setToolTipText("Next frame");
        nextButton.addActionListener(new ActionListener() {

            @Override
            public void actionPerformed(ActionEvent ae) {

                imageView.setCurrentFrame(imageView.getCurrentFrame() + 1);
                update(selectedDirection, null);
            }
        });
        constraints2.gridx = 3;
        constraints2.gridy = 0;
        constraints2.gridwidth = 1;
        constraints2.weightx = 0.0;
        constraints2.weighty = 0.0;
        buttonsPanel.add(nextButton, constraints2);

        // second buttons row

        // first button
        firstButton = new JButton(Project.getEditorImageIconOrEmpty("icon_first.png"));
        firstButton.setToolTipText("First frame");
        firstButton.addActionListener(new ActionListener() {

            @Override
            public void actionPerformed(ActionEvent ae) {

                imageView.setCurrentFrame(0);
                update(selectedDirection, null);
            }
        });
        constraints2.gridx = 0;
        constraints2.gridy = 1;
        constraints2.gridwidth = 1;
        constraints2.weightx = 0.0;
        constraints2.weighty = 0.0;
        buttonsPanel.add(firstButton, constraints2);

        // start button
        startIcon = Project.getEditorImageIconOrEmpty("icon_start.png");
        pauseIcon = Project.getEditorImageIconOrEmpty("icon_pause.png");
        startButton = new JButton(startIcon);
        startButton.addActionListener(new ActionListener() {

            @Override
            public void actionPerformed(ActionEvent ae) {

                if (imageView.isStarted()) {
                    imageView.pause();
                } else {
                    imageView.start();
                }
                update(selectedDirection, null);
            }
        });
        constraints2.gridx = 1;
        constraints2.gridy = 1;
        constraints2.gridwidth = 1;
        constraints2.weightx = 0.0;
        constraints2.weighty = 0.0;
        buttonsPanel.add(startButton, constraints2);

        // stop button
        stopButton = new JButton(Project.getEditorImageIconOrEmpty("icon_stop.png"));
        stopButton.setToolTipText("Stop");
        stopButton.addActionListener(new ActionListener() {

            @Override
            public void actionPerformed(ActionEvent ae) {

                imageView.stop();
                update(selectedDirection, null);
            }
        });
        constraints2.gridx = 2;
        constraints2.gridy = 1;
        constraints2.gridwidth = 1;
        constraints2.weightx = 0.0;
        constraints2.weighty = 0.0;
        buttonsPanel.add(stopButton, constraints2);

        // last button
        lastButton = new JButton(Project.getEditorImageIconOrEmpty("icon_last.png"));
        lastButton.setToolTipText("Last frame");
        lastButton.addActionListener(new ActionListener() {

            @Override
            public void actionPerformed(ActionEvent ae) {

                if (selectedDirection != null) {
                    imageView.setCurrentFrame(selectedDirection.getNbFrames() - 1);
                } else {
                    imageView.setCurrentFrame(0);
                }
                update(selectedDirection, null);
            }
        });
        constraints2.gridx = 3;
        constraints2.gridy = 1;
        constraints2.gridwidth = 1;
        constraints2.weightx = 0.0;
        constraints2.weighty = 0.0;
        buttonsPanel.add(lastButton, constraints2);

        // origin point
        drawOrigin = new JCheckBox("Display origin point");
        drawOrigin.setAlignmentX(CENTER_ALIGNMENT);
        drawOrigin.setSelected(imageView.isDrawOrigin());
        drawOrigin.addActionListener(new ActionListener() {

            @Override
            public void actionPerformed(ActionEvent ae) {

                imageView.setDrawOrigin(drawOrigin.isSelected());
                imageView.repaint();
            }
        });

        constraints.gridy++;
        constraints.weightx = 1.0;
        constraints.weighty = 0.0;
        add(drawOrigin, constraints);

        constraints.gridy++;
        constraints.weightx = 1.0;
        add(buttonsPanel, constraints);
    }

    /**
     * Sets the sprite observed.
     * @param sprite the sprite
     */
    public void setSprite(Sprite sprite) {

        if (this.sprite != null) {
            this.sprite.deleteObserver(this);
        }

        this.sprite = sprite;

        if (sprite != null) {
            sprite.addObserver(this);
        }
        update(sprite, null);
    }

    /**
     * Sets the animation observed.
     * @param animation the animation
     */
    private void setAnimation(SpriteAnimation animation) {

        if (selectedAnimation != null) {
            selectedAnimation.deleteObserver(this);
        }

        selectedAnimation = animation;

        if (animation != null) {
            animation.addObserver(this);
        }
    }

    /**
     * Sets the direction observed.
     * @param direction the direction
     */
    private void setDirection(SpriteAnimationDirection direction) {

        if (selectedDirection != null) {
            selectedDirection.deleteObserver(this);
        }

        selectedDirection = direction;

        if (direction != null) {
            direction.addObserver(this);
        }
        update(direction, null);
    }

    /**
     * This function is called when the sprite, animation or direction is changed.
     */
    @Override
    public void update(Observable o, Object obj) {

        if (o instanceof Sprite) {
            setAnimation(sprite.getSelectedAnimation());
            setDirection(sprite.getSelectedDirection());
        }
        else {
            imageView.update(selectedDirection);

            if (selectedDirection != null) {

                int lastFrame = selectedDirection.getNbFrames() - 1;
                int currentFrame = imageView.getCurrentFrame();
                boolean started = imageView.isStarted();

                startButton.setEnabled(selectedDirection.getNbFrames() > 1);
                if (started) {
                    startButton.setToolTipText("Pause");
                    startButton.setIcon(pauseIcon);
                } else {
                    startButton.setToolTipText("Start");
                    startButton.setIcon(startIcon);
                }

                stopButton.setEnabled(started);
                nextButton.setEnabled(!started && currentFrame < lastFrame);
                prevButton.setEnabled(!started && currentFrame > 0);
                firstButton.setEnabled(!started && currentFrame > 0);
                lastButton.setEnabled(!started && currentFrame < lastFrame);
            } else {
                startButton.setEnabled(false);
                stopButton.setEnabled(false);
                nextButton.setEnabled(false);
                prevButton.setEnabled(false);
                firstButton.setEnabled(false);
                lastButton.setEnabled(false);
            }

            updateCurrentFrame();
        }
    }

    /**
     * Refresh the current frame number displayed.
     */
    protected void updateCurrentFrame() {

        if (selectedDirection != null) {
            int nbFrames = selectedDirection.getNbFrames();
            frameView.setText("" + imageView.getCurrentFrame() + "/" + (nbFrames - 1));
        } else {
            frameView.setText("0/0");
        }
    }

    /**
     * Component to show the direction frame (animated).
     */
    private class ImageView extends JPanel {

        // frames list
        private Image[] frames = null;
        // frame delay
        private int frameDelay = 0;
        // frame on loop
        private int loopOnFrame = 0;
        // the curretn frame
        private int currentFrame = 0;
        // draw origin point
        private boolean drawOrigin = true;

        /**
         * The timer used for animation.
         */
        private javax.swing.Timer timer;

        /**
         * Constructor.
         */
        public ImageView() {

            timer = new javax.swing.Timer(0, new ActionListener() {

                @Override
                public void actionPerformed(ActionEvent ae) {

                    if (selectedDirection == null || frameDelay <= 0 || frames == null || frames.length <= 1) {
                        stop();
                        SpriteAnimationDirectionPreviewer.this.update(selectedDirection, null);
                    } else {
                        int nbFrames = frames != null ? frames.length : 1;
                        if (currentFrame == nbFrames - 1) {
                            if (loopOnFrame >= 0) {
                                currentFrame = Math.min(loopOnFrame, nbFrames - 1);
                            } else {
                                pause();
                                SpriteAnimationDirectionPreviewer.this.update(selectedDirection, null);
                            }
                        } else {
                            currentFrame = (currentFrame + 1) % nbFrames;
                        }
                        updateCurrentFrame();
                    }
                    repaint();
                }
            });
            timer.stop();
        }

        /**
         * Check if the previewer is started.
         *
         * @return true if the previewer is stared, false otherwise
         */
        public boolean isStarted() {

            return timer.isRunning();
        }

        /**
         * Check if the previewer draw the origin point.
         * @return
         */
        public boolean isDrawOrigin() {

            return drawOrigin;
        }

        /**
         * Sets draw origin point.
         * @param drawOrigin
         */
        public void setDrawOrigin(boolean drawOrigin) {

            this.drawOrigin = drawOrigin;
        }

        /**
         * Starts the previwer.
         */
        public void start() {

            if (!isStarted() && frameDelay > 0) {
                currentFrame = 0;
                timer.setDelay(frameDelay);
                timer.setInitialDelay(frameDelay);
                timer.start();
            }
        }

        /**
         * Pauses the previewer.
         */
        public void pause() {

            if (isStarted()) {
                timer.stop();
            }
        }

        /**
         * Stops the previewer.
         */
        public void stop() {

            pause();
            currentFrame = 0;
        }

        /**
         * Changes the frame list to display.
         */
        public void update(Observable o) {

            if (selectedDirection != null) {
                frames = selectedDirection.getFrames();
                if (selectedAnimation != null) {
                    frameDelay = selectedAnimation.getFrameDelay();
                    loopOnFrame = selectedAnimation.getLoopOnFrame();

                    timer.setDelay(frameDelay);
                    timer.setInitialDelay(frameDelay);
                }
            } else {
                frames = null;
                frameDelay = 0;
                loopOnFrame = 0;
            }

            setMinimumSize(getFrameSize());
            setMaximumSize(getFrameSize());
            setPreferredSize(getFrameSize());
            revalidate();
            repaint();
        }

        /**
         * Returns the current displayed frame number.
         *
         * @return the current frame number
         */
        public int getCurrentFrame() {

            return currentFrame;
        }

        /**
         * Changes the current displayed frame.
         *
         * @param frame the frame number to display
         */
        public void setCurrentFrame(int frame) {

            int nbFrames = frames != null ? frames.length : 1;
            currentFrame = Math.max(0, frame % nbFrames);
            repaint();
        }

        @Override
        public void paint(Graphics g) {

            Dimension size = getPreferredSize();
            g.clearRect(0, 0, size.width, size.height);
            if (frames != null && frames.length > 0) {

                Image image = frames[currentFrame % frames.length];
                int width = image.getWidth(null) * 2;
                int height = image.getHeight(null) * 2;
                int x = (size.width - width) / 2;
                int y = (size.height - height) / 2;

                g.drawImage(image, x, y, width, height, null);

                if (drawOrigin && selectedDirection != null) {
                    Point origin = selectedDirection.getOrigin();
                    x += origin.x * 2;
                    y += origin.y * 2;

                    g.setColor(Color.MAGENTA);
                    g.drawLine(x, 0, x, size.height);
                    g.drawLine(x + 1, 0, x + 1, size.height);

                    g.drawLine(0, y, size.width, y);
                    g.drawLine(0, y + 1, size.width, y + 1);
                }
            }
        }

        public Dimension getFrameSize() {

            if (frames != null && frames.length > 0) {
                Image frame = frames[0];
                return new Dimension(frame.getWidth(null) * 2,
                        frame.getHeight(null) * 2);
            }

            return new Dimension(32, 32);
        }
    }
}
