package org.xi.test;

class NewThread extends Thread {

	boolean suspendFlag;

	NewThread(String tname, ThreadGroup tgroup) {
		super(tgroup, tname);
		System.out.println("New thread: " + this);

		suspendFlag = false;
		start();
	}

	public void run() {
		try {
			for (int i = 5; i > 0; i--) {
				System.out.println(getName() + ": " + i);
				Thread.sleep(1000);

				synchronized (this) {
					while (suspendFlag) {
						wait();
					}
				}
			}
		} catch (Exception e) {
			System.out.println("Exception in " + getName());
		}
		System.out.println(getName() + " exiting");
	}

	void mySuspend() {
		suspendFlag = true;
	}

	synchronized void myResume() {
		suspendFlag = false;
		notify();
	}
}

public class ThreadGroupDemo {
	public static void main(String[] args) {
		ThreadGroup groupA = new ThreadGroup("Group A");
		ThreadGroup groupB = new ThreadGroup("Group B");

		NewThread tobj1 = new NewThread("One", groupA);
		NewThread tobj2 = new NewThread("Two", groupA);
		NewThread tobj3 = new NewThread("Three", groupB);
		NewThread tobj4 = new NewThread("Four", groupB);

		System.out.println("\n\nHere is output from list():");
		groupA.list();
		groupB.list();

		System.out.println("\nSuspending Group A");
		Thread tgA[] = new Thread[groupA.activeCount()];
		groupA.enumerate(tgA);
		for (int i = 0; i < tgA.length; i++) {
			((NewThread) tgA[i]).mySuspend();
		}

		try {
			Thread.sleep(4000);
		} catch (InterruptedException e) {
			System.out.println("Main thread interrupted");
		}

		System.out.println("\nResuming Group A");
		for (int i = 0; i < tgA.length; i++) {
			((NewThread) tgA[i]).myResume();
		}

		try {
			System.out.println("Waiting for threads to finish");
			tobj1.join();
			tobj2.join();
			tobj3.join();
			tobj4.join();
		} catch (Exception e) {
			System.out.println("Exception in Main thread");
		}

		System.out.println("\nMain thread exiting");
	}
}
