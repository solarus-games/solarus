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
    final private JTextField frameView;

    // buttons
    final private JButton startButton;
    final private JButton stopButton;
    final private JButton nextButton;
    final private JButton prevButton;
    final private JButton firstButton;
    final private JButton lastButton;

    // icons of start button
    final private ImageIcon startIcon;
    final private ImageIcon pauseIcon;

    /**
     * Constructor.
     */
    public SpriteAnimationDirectionPreviewer() {

        setLayout(new BoxLayout(this, BoxLayout.PAGE_AXIS));
        setBorder(BorderFactory.createEmptyBorder(5, 5, 5, 5));

        // image view
        imageView = new ImageView();
        JScrollPane imageViewScroller = new JScrollPane(imageView);
        // first buttons row
        JPanel firstRowPanel = new JPanel();
        firstRowPanel.setLayout(new BoxLayout(firstRowPanel, BoxLayout.LINE_AXIS));

        // previous button
        prevButton = new JButton(Project.getEditorImageIconOrEmpty("icon_previous.png"));
        prevButton.setPreferredSize(new Dimension(24, 24));
        prevButton.setToolTipText("Previous frame");
        prevButton.addActionListener(new ActionListener() {

            @Override
            public void actionPerformed(ActionEvent ae) {

                imageView.setCurrentFrame(imageView.getCurrentFrame() - 1);
                update(selectedDirection, null);
            }
        });
        firstRowPanel.add(prevButton);

        // frame view
        frameView = new JTextField("0/0");
        frameView.setEditable(false);
        frameView.setHorizontalAlignment(JTextField.CENTER);
        frameView.setMinimumSize(new Dimension(48, 24));
        frameView.setMaximumSize(new Dimension(105, 24));
        firstRowPanel.add(Box.createRigidArea(new Dimension(5, 0)));
        firstRowPanel.add(frameView);

        // next button
        nextButton = new JButton(Project.getEditorImageIconOrEmpty("icon_next.png"));
        nextButton.setPreferredSize(new Dimension(24, 24));
        nextButton.setToolTipText("Next frame");
        nextButton.addActionListener(new ActionListener() {

            @Override
            public void actionPerformed(ActionEvent ae) {

                imageView.setCurrentFrame(imageView.getCurrentFrame() + 1);
                update(selectedDirection, null);
            }
        });
        firstRowPanel.add(Box.createRigidArea(new Dimension(5, 0)));
        firstRowPanel.add(nextButton);

        // second buttons row
        JPanel secondRowPanel = new JPanel();
        secondRowPanel.setLayout(new BoxLayout(secondRowPanel, BoxLayout.LINE_AXIS));

        // first button
        firstButton = new JButton(Project.getEditorImageIconOrEmpty("icon_first.png"));
        firstButton.setPreferredSize(new Dimension(24, 24));
        firstButton.setToolTipText("First frame");
        firstButton.addActionListener(new ActionListener() {

            @Override
            public void actionPerformed(ActionEvent ae) {

                imageView.setCurrentFrame(0);
                update(selectedDirection, null);
            }
        });
        secondRowPanel.add(firstButton);

        // start button
        startIcon = Project.getEditorImageIconOrEmpty("icon_start.png");
        pauseIcon = Project.getEditorImageIconOrEmpty("icon_pause.png");
        startButton = new JButton(startIcon);
        startButton.setPreferredSize(new Dimension(24, 24));
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
        secondRowPanel.add(Box.createRigidArea(new Dimension(5, 0)));
        secondRowPanel.add(startButton);

        // stop button
        stopButton = new JButton(Project.getEditorImageIconOrEmpty("icon_stop.png"));
        stopButton.setPreferredSize(new Dimension(24, 24));
        stopButton.setToolTipText("Stop");
        stopButton.addActionListener(new ActionListener() {

            @Override
            public void actionPerformed(ActionEvent ae) {

                imageView.stop();
                update(selectedDirection, null);
            }
        });
        secondRowPanel.add(Box.createRigidArea(new Dimension(5, 0)));
        secondRowPanel.add(stopButton);

        // last button
        lastButton = new JButton(Project.getEditorImageIconOrEmpty("icon_last.png"));
        lastButton.setPreferredSize(new Dimension(24, 24));
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
        secondRowPanel.add(Box.createRigidArea(new Dimension(5, 0)));
        secondRowPanel.add(lastButton);

        add(imageViewScroller);
        add(Box.createRigidArea(new Dimension(0, 5)));
        add(firstRowPanel);
        add(Box.createRigidArea(new Dimension(0, 5)));
        add(secondRowPanel);
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
    private class ImageView extends JPanel implements Scrollable {

        // frames list
        private Image[] frames = null;
        // frame delay
        private int frameDelay = 0;
        // frame on loop
        private int loopOnFrame = 0;
        // the curretn frame
        private int currentFrame = 0;

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
         * Check if the previwer is started.
         *
         * @return true if the previewer is stared, false otherwise
         */
        public boolean isStarted() {

            return timer.isRunning();
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
                g.drawImage(image, 0, 0, size.width, size.height, null);
            }
        }

        @Override
        public Dimension getPreferredSize() {

            if (frames != null && frames.length > 0) {
                Image frame = frames[0];
                return new Dimension(frame.getWidth(null) * 2, frame.getHeight(null) * 2);
            }

            return new Dimension(0, 0);
        }

        @Override
        public Dimension getPreferredScrollableViewportSize() {

            return new Dimension(240, 128);
        }

        @Override
        public int getScrollableUnitIncrement(Rectangle rctngl, int i, int i1) {
            return 16;
        }

        @Override
        public int getScrollableBlockIncrement(Rectangle rctngl, int i, int i1) {
            return 160;
        }

        @Override
        public boolean getScrollableTracksViewportWidth() {
            return false;
        }

        @Override
        public boolean getScrollableTracksViewportHeight() {
            return false;
        }

    }
}
