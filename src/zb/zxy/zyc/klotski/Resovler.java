package zb.zxy.zyc.klotski;

public class Resovler {
	private byte[] currentBoard = null;
	
	public void init() {
		start();
	}
	
	public byte[] getNextBoard(byte[] board) {
		byte[] ret = null;
		// TODO
		return ret;
	}
	
	public void finalize() {
		end();
	}
	
	private native void start();
	private native byte[][] analyze(byte[] board);
	private native void end();
}
