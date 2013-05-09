package org.xi.test;

public class IntTest {
	public static void main(String args[]) {
		System.out.println("Hello");
		System.out.println(65535);
		System.out.println(65536);
		System.out.println("append: " + 65537);
		System.out.println("append: " + 65539 + " x : ");

		for (long l = 0x000fffffff0L; l < 0x00100000000L; l++) {
			System.out.println(".");
		}

		int x = 0;
		long t = x - 1;
		System.out.println("int x = 0; long t = x - 1  ====> " + t);
		t = x - 10;
		System.out.println("int x = 0; long t = x - 10 ====> " + t);

		System.out.println("Integer.MAX_VALUE = " + Integer.MAX_VALUE);
		System.out.println("Long.MAX_VALUE = " + Long.MAX_VALUE);
		System.out.println("Integer.parseInt(7FFFFFFF) = "
				+ Integer.parseInt("7FFFFFFF", 16));
		System.out.println("Long.parseLong(10) = " + Long.parseLong("10"));
		System.out.println("Long.parseLong(100000001) = "
				+ Long.parseLong("100000001"));
		System.out.println("Long.parseLong(7FFFFFFF) = "
				+ Long.parseLong("7FFFFFFF", 16));
		System.out.println("Long.parseLong(80000000) = "
				+ Long.parseLong("80000000", 16));
		System.out.println("Long.parseLong(FFFFFFFF) = "
				+ Long.parseLong("FFFFFFFF", 16));
		System.out.println("Long.parseLong(100000000000) = "
				+ Long.parseLong("100000000000", 10));
		System.out.println("Long.parseLong(7FFFFFFFFFFFFFFF) = "
				+ Long.parseLong("7FFFFFFFFFFFFFFF", 16));

		String intStr;
		intStr = MyIntegralToString.intToString(123);
		System.out.println(intStr);
		intStr = MyIntegralToString.intToString(65535);
		System.out.println(intStr);
		intStr = MyIntegralToString.intToString(65536);
		System.out.println(intStr);
		intStr = MyIntegralToString.intToString(65550);
		System.out.println(intStr);
		intStr = MyIntegralToString.intToString(2147483647);
		System.out.println(intStr);
		intStr = MyIntegralToString.intToString(-1);
		System.out.println(intStr);
		intStr = MyIntegralToString.intToString(-2);
		System.out.println(intStr);
		intStr = MyIntegralToString.intToString(-2147483648);
		System.out.println(intStr);
		intStr = MyIntegralToString.intToString(2147483647 + 1);
		System.out.println(intStr);
		intStr = MyIntegralToString.longToString(2147483647L + 1);
		System.out.println(intStr);
	}
}

class MyIntegralToString {

	private static final String[] SMALL_NONNEGATIVE_VALUES = new String[100];
	private static final String[] SMALL_NEGATIVE_VALUES = new String[100];

	private static final char[] TENS = { '0', '0', '0', '0', '0', '0', '0',
			'0', '0', '0', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1',
			'2', '2', '2', '2', '2', '2', '2', '2', '2', '2', '3', '3', '3',
			'3', '3', '3', '3', '3', '3', '3', '4', '4', '4', '4', '4', '4',
			'4', '4', '4', '4', '5', '5', '5', '5', '5', '5', '5', '5', '5',
			'5', '6', '6', '6', '6', '6', '6', '6', '6', '6', '6', '7', '7',
			'7', '7', '7', '7', '7', '7', '7', '7', '8', '8', '8', '8', '8',
			'8', '8', '8', '8', '8', '9', '9', '9', '9', '9', '9', '9', '9',
			'9', '9' };

	/** Ones [i] contains the tens digit of the number i, 0 <= i <= 99. */
	private static final char[] ONES = { '0', '1', '2', '3', '4', '5', '6',
			'7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
			'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2',
			'3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5',
			'6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8',
			'9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1',
			'2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4',
			'5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7',
			'8', '9', };
	/**
	 * Table for MOD / DIV 10 computation described in Section 10-21 of Hank
	 * Warren's "Hacker's Delight" online addendum.
	 * http://www.hackersdelight.org/divcMore.pdf
	 */
	private static final char[] MOD_10_TABLE = { 0, 1, 2, 2, 3, 3, 4, 5, 5, 6,
			7, 7, 8, 8, 9, 0 };

	/**
	 * The digits for every supported radix.
	 */
	private static final char[] DIGITS = { '0', '1', '2', '3', '4', '5', '6',
			'7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
			'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w',
			'x', 'y', 'z' };

	private MyIntegralToString() {
	}

	public static String intToString(int i) {
		return convertInt(i);
	}

	private static String convertInt(int i) {
		boolean negative = false;
		String quickResult = null;
		if (i < 0) {
			negative = true;
			i = -i;
			if (i < 100) {
				if (i < 0) {
					// If -n is still negative, n is Integer.MIN_VALUE
					quickResult = "-2147483648";
				} else {
					quickResult = SMALL_NEGATIVE_VALUES[i];
					if (quickResult == null) {
						SMALL_NEGATIVE_VALUES[i] = quickResult = i < 10 ? stringOf(
								'-', ONES[i]) : stringOf('-', TENS[i], ONES[i]);
					}
				}
			}
		} else {
			if (i < 100) {
				quickResult = SMALL_NONNEGATIVE_VALUES[i];
				if (quickResult == null) {
					SMALL_NONNEGATIVE_VALUES[i] = quickResult = i < 10 ? stringOf(ONES[i])
							: stringOf(TENS[i], ONES[i]);
				}
			}
		}
		if (quickResult != null) {
			return quickResult;
		}

		int bufLen = 11; // Max number of chars in result
		char[] buf = new char[bufLen];
		int cursor = bufLen;

		// Calculate digits two-at-a-time till remaining digits fit in 16 bits
		while (i >= (1 << 16)) {
			// Compute q = n/100 and r = n % 100 as per "Hacker's Delight" 10-8
			int q = (int) ((0x51EB851FL * i) >>> 37);
			int r = i - 100 * q;
			buf[--cursor] = ONES[r];
			buf[--cursor] = TENS[r];
			i = q;
		}

		// Calculate remaining digits one-at-a-time for performance
		while (i != 0) {
			// Compute q = n/10 and r = n % 10 as per "Hacker's Delight" 10-8
			int q = (0xCCCD * i) >>> 19;
			int r = i - 10 * q;
			buf[--cursor] = DIGITS[r];
			i = q;
		}

		if (negative) {
			buf[--cursor] = '-';
		}

		return new String(buf, cursor, bufLen - cursor);
	}

	public static String longToString(long l) {
		return convertLong(l);
	}

	private static String convertLong(long n) {
		int i = (int) n;
		if (i == n) {
			return convertInt(i);
		}

		boolean negative = (n < 0);
		if (negative) {
			n = -n;
			if (n < 0) {
				// If -n is still negative, n is Long.MIN_VALUE
				String quickResult = "-9223372036854775808";
				return quickResult;
			}
		}

		int bufLen = 20; // Maximum number of chars in result
		char[] buf = new char[bufLen];

		int low = (int) (n % 1000000000); // Extract low-order 9 digits
		int cursor = intIntoCharArray(buf, bufLen, low);

		// Zero-pad Low order part to 9 digits
		while (cursor != (bufLen - 9)) {
			buf[--cursor] = '0';
		}

		/*
		 * The remaining digits are (n - low) / 1,000,000,000. This
		 * "exact division" is done as per the online addendum to Hank Warren's
		 * "Hacker's Delight" 10-20, http://www.hackersdelight.org/divcMore.pdf
		 */
		n = ((n - low) >>> 9) * 0x8E47CE423A2E9C6DL;

		/*
		 * If the remaining digits fit in an int, emit them using a single call
		 * to intIntoCharArray. Otherwise, strip off the low-order digit, put it
		 * in buf, and then call intIntoCharArray on the remaining digits (which
		 * now fit in an int).
		 */
		if ((n & (-1L << 32)) == 0) {
			cursor = intIntoCharArray(buf, cursor, (int) n);
		} else {
			/*
			 * Set midDigit to n % 10
			 */
			int lo32 = (int) n;
			int hi32 = (int) (n >>> 32);

			// midDigit = ((unsigned) low32) % 10, per "Hacker's Delight" 10-21
			int midDigit = MOD_10_TABLE[(0x19999999 * lo32 + (lo32 >>> 1) + (lo32 >>> 3)) >>> 28];

			// Adjust midDigit for hi32. (assert hi32 == 1 || hi32 == 2)
			midDigit -= hi32 << 2; // 1L << 32 == -4 MOD 10
			if (midDigit < 0) {
				midDigit += 10;
			}
			buf[--cursor] = DIGITS[midDigit];

			// Exact division as per Warren 10-20
			int rest = ((int) ((n - midDigit) >>> 1)) * 0xCCCCCCCD;
			cursor = intIntoCharArray(buf, cursor, rest);
		}

		if (negative) {
			buf[--cursor] = '-';
		}
		return new String(buf, cursor, bufLen - cursor);
	}

	private static int intIntoCharArray(char[] buf, int cursor, int n) {
		// Calculate digits two-at-a-time till remaining digits fit in 16 bits
		while ((n & 0xffff0000) != 0) {
			/*
			 * Compute q = n/100 and r = n % 100 as per "Hacker's Delight" 10-8.
			 * This computation is slightly different from the corresponding
			 * computation in intToString: the shifts before and after multiply
			 * can't be combined, as that would yield the wrong result if n's
			 * sign bit were set.
			 */
			int q = (int) ((0x51EB851FL * (n >>> 2)) >>> 35);
			int r = n - 100 * q;
			buf[--cursor] = ONES[r];
			buf[--cursor] = TENS[r];
			n = q;
		}

		// Calculate remaining digits one-at-a-time for performance
		while (n != 0) {
			// Compute q = n / 10 and r = n % 10 as per "Hacker's Delight" 10-8
			int q = (0xCCCD * n) >>> 19;
			int r = n - 10 * q;
			buf[--cursor] = DIGITS[r];
			n = q;
		}
		return cursor;
	}

	private static String stringOf(char... args) {
		return new String(args, 0, args.length);
	}
}
