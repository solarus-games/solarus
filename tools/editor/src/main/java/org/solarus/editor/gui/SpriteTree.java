package org.solarus.editor.gui;

import java.util.Observable;
import java.util.Observer;

import javax.swing.JTree;

import org.solarus.editor.Sprite;

public class SpriteTree extends JTree implements Observer {

    /**
     * The sprite observed.
     */
    private Sprite sprite;

    /**
     * Sets the sprite observed.
     * @param sprite the sprite
     */
    public void setSprite(Sprite sprite) {

        if (this.sprite != sprite) {
            if (this.sprite != null) {
                this.sprite.deleteObserver(this);
            }

            this.sprite = sprite;

            if (sprite != null) {
                sprite.addObserver(this);
            }
            update(sprite, null);
        }
    }

    @Override
    public void update(Observable arg0, Object arg1) {
        // TODO Auto-generated method stub
        
    }

}
