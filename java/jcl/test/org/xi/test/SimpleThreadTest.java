package org.xi.test;

public class SimpleThreadTest extends Thread {
	public SimpleThreadTest(String str) {
		super(str);
	}

	public void run() {
		for (int i = 0; i < 10; i++) {
			System.out.println(i + " " + getName());
			try {
				sleep((int) (Math.random() * 100));
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
		}
		System.out.println("DONE! " + getName());
	}

	public static void main(String args[]) {
		new SimpleThreadTest("Jamaica").start();
		new SimpleThreadTest("Fiji").start();
	}
}
