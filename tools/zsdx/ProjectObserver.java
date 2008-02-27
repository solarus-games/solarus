package zsdx;

/**
 * The classes implementing this interface can be notified when a project is loaded.
 */
public interface ProjectObserver {

    /**
     * This method is called when a project has just been loaded.
     */
    public void currentProjectChanged();
    
}
