package org.xi.test;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.FileOutputStream;
import java.io.OutputStream;
import java.net.HttpURLConnection;
import java.net.URL;

public class URLDownloadTest {
	public static void main(String[] args) {
		try {
			HttpURLConnection c;

			// save file
			URL url = new URL("http://192.168.23.10/AveMaria.mp3");
			c = (HttpURLConnection) url.openConnection();

			// connect
			c.connect();

			// input stream
			BufferedInputStream in = new BufferedInputStream(c.getInputStream());

			// save the file
			OutputStream out = new BufferedOutputStream(new FileOutputStream(
					"AveMaria.mp3"));
			byte[] buf = new byte[20480];
			int n = 0;
			int t = 0;
			while ((n = in.read(buf)) >= 0) {
				t += n;
				System.out.println("total = " + t + " / read " + n);
				out.write(buf, 0, n);
			}
			out.flush();
			out.close();
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
}
