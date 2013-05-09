package org.xi.test;

import java.util.Calendar;
import java.util.Locale;
import java.util.TimeZone;

public class LocaleTestLauncher {

	public static void main(String[] args) {
		long systime = System.currentTimeMillis();

		Calendar cal = Calendar.getInstance();
		long caltime = cal.getTimeInMillis();

		Locale locale = Locale.getDefault();
		TimeZone timezone = TimeZone.getDefault();

		System.out.println("currentTimeMillis() = " + systime);
		System.out.println("Calendar.getTimeInMillis() = " + caltime);
		System.out.println("Locale.getDefault().getCountry() = "
				+ locale.getCountry());
		System.out.println("TimeZone.getDefault().getDisplayName() = "
				+ timezone.getDisplayName());
		System.out.println("TimeZone.getDefault().getID() = "
				+ timezone.getID());
		System.out.println("TimeZone.getTimeZone(\"Asia/Seoul\") = "
				+ TimeZone.getTimeZone("Asia/Seoul"));
	}
}
