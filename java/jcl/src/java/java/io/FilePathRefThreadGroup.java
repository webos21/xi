package java.io;

public class FilePathRefThreadGroup extends ThreadGroup {

	private String ctxRootPath = null;

	public FilePathRefThreadGroup(String name) {
		super(name);
	}

	public FilePathRefThreadGroup(ThreadGroup parent, String name) {
		super(parent, name);
	}

	public String getContextRootPath() {
		return ctxRootPath;
	}

	public void setContextRootPath(String cpath) {
		this.ctxRootPath = cpath;
	}
}
