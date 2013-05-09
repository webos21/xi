package org.xi.test;

import java.io.File;
import java.io.FileInputStream;

// GC Test
///*
public class GcTestLauncher implements Runnable {
	public static void main(String[] args) {

		GcTestLauncher abnl = new GcTestLauncher();

		for (int i = 0; i < 10; ++i) {
			new Thread(abnl, "GcTest-" + i).start();
		}

		System.out.println("Entering loop...");

		byte[] infobuf = new byte[103];
		Thread[] tlist = new Thread[50];

		while (true) {

			try {
				int cnt = Thread.currentThread().getThreadGroup()
						.enumerate(tlist);
				System.out.println("========= Thread List =========");
				for (int i = 0; i < cnt; i++) {
					System.out.println("Thread[" + i + "] " + tlist[i]);
				}
				System.out.println("===============================");

				long mmax = Runtime.getRuntime().maxMemory() / 1024;
				long mtotal = Runtime.getRuntime().totalMemory() / 1024;
				long mfree = Runtime.getRuntime().freeMemory() / 1024;

				System.out.println("========= Java Memory =========");
				System.out.println("Max     :          " + mmax + " kB");
				System.out.println("Total   :          " + mtotal + " kB");
				System.out.println("Free    :          " + mfree + " kB");
				System.out.println("===============================");

				File memif = new File("/proc/meminfo");
				if (memif.exists()) {
					System.out.println("======== Native Memory ========");
					FileInputStream meminfo = new FileInputStream(memif);
					meminfo.read(infobuf);
					System.out.println(new String(infobuf));
					System.out.println("===============================\n\n");
					meminfo.close();
					meminfo = null;
				}
				memif = null;

				Thread.sleep(1000);
			} catch (Exception e) {
			}

		}
	}

	public void run() {
		int cnt = 0;
		byte[] barry;

		while (true) {

			try {
				barry = new byte[(cnt % 10) * 1024 * 1024];

				System.out.println("cnt: " + cnt + ", ThreadID: "
						+ Thread.currentThread().getId() + ", byte[] len: "
						+ barry.length + ", addr=" + barry);

				++cnt;
				Thread.sleep(Thread.currentThread().getId() * 500);
			} catch (Exception e) {
			}

		}
	}

}
// */

// CPU % Test
/*
 * public class AbnLauncher implements Runnable { public static void
 * main(String[] args) {
 * 
 * AbnLauncher abnl = new AbnLauncher();
 * 
 * for (int i = 0; i < 1; ++i) { new Thread(abnl).start(); }
 * 
 * abnl.processBoot();
 * 
 * System.out.println("Entering loop..."); while (true) {
 * System.out.println("Do nothing but print this message");
 * 
 * try { Thread.sleep(30000); } catch (Exception e) { }
 * 
 * } }
 * 
 * public void run() { Object syncObj = new Object();
 * 
 * while (true) { System.out.println("ThreadID: " +
 * Thread.currentThread().getId());
 * 
 * try { //Thread.sleep(30000); synchronized(syncObj) { syncObj.wait(30000); } }
 * catch (Exception e) { }
 * 
 * } }
 * 
 * private void processBoot() { NDeviceManager.initialize();
 * 
 * }
 * 
 * }
 */
