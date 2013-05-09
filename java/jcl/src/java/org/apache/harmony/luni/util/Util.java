/*
 *  Licensed to the Apache Software Foundation (ASF) under one or more
 *  contributor license agreements.  See the NOTICE file distributed with
 *  this work for additional information regarding copyright ownership.
 *  The ASF licenses this file to You under the Apache License, Version 2.0
 *  (the "License"); you may not use this file except in compliance with
 *  the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

package org.apache.harmony.luni.util;

import java.io.ByteArrayOutputStream;
import java.io.UnsupportedEncodingException;

public final class Util {
	/**
	 * '%' and two following hex digit characters are converted to the
	 * equivalent byte value. All other characters are passed through
	 * unmodified. e.g. "ABC %24%25" -> "ABC $%"
	 * 
	 * @param s
	 *            java.lang.String The encoded string.
	 * @return java.lang.String The decoded version.
	 */
	public static String decode(String s, boolean convertPlus) {
		return decode(s, convertPlus, null);
	}

	/**
	 * '%' and two following hex digit characters are converted to the
	 * equivalent byte value. All other characters are passed through
	 * unmodified. e.g. "ABC %24%25" -> "ABC $%"
	 * 
	 * @param s
	 *            java.lang.String The encoded string.
	 * @param encoding
	 *            the specified encoding
	 * @return java.lang.String The decoded version.
	 */
	public static String decode(String s, boolean convertPlus, String encoding) {
		if (!convertPlus && s.indexOf('%') == -1)
			return s;
		StringBuilder result = new StringBuilder(s.length());
		ByteArrayOutputStream out = new ByteArrayOutputStream();
		for (int i = 0; i < s.length();) {
			char c = s.charAt(i);
			if (convertPlus && c == '+')
				result.append(' ');
			else if (c == '%') {
				out.reset();
				do {
					if (i + 2 >= s.length()) {
						throw new IllegalArgumentException(
								"Incomplete % sequence at: " + i);
					}
					int d1 = Character.digit(s.charAt(i + 1), 16);
					int d2 = Character.digit(s.charAt(i + 2), 16);
					if (d1 == -1 || d2 == -1) {
						throw new IllegalArgumentException(
								"Invalid % sequence " + s.substring(i, i + 3)
										+ " at " + i);
					}
					out.write((byte) ((d1 << 4) + d2));
					i += 3;
				} while (i < s.length() && s.charAt(i) == '%');
				if (encoding == null) {
					result.append(out.toString());
				} else {
					try {
						result.append(out.toString(encoding));
					} catch (UnsupportedEncodingException e) {
						throw new IllegalArgumentException(e);
					}
				}
				continue;
			} else
				result.append(c);
			i++;
		}
		return result.toString();
	}

	/**
	 * Encode the given url string as RFC-1738 required.
	 * 
	 * @param urlString
	 * @return encoded URL string
	 */
	// by jshwang - added
	public static String encodeURL(String urlStr) {
		String digits = "0123456789ABCDEF"; //$NON-NLS-1$

		StringBuilder buf = new StringBuilder(urlStr.length() + 16);
		for (int i = 0; i < urlStr.length(); i++) {
			char ch = urlStr.charAt(i);
			if ('%' == ch) {
				if (i + 1 < urlStr.length()
						&& i + 2 < urlStr.length()
						&& digits.indexOf(Character.toUpperCase(urlStr
								.charAt(i + 1))) != -1
						&& digits.indexOf(Character.toUpperCase(urlStr
								.charAt(i + 2))) != -1) {
					buf.append(ch);
					buf.append(urlStr.charAt(i + 1));
					buf.append(urlStr.charAt(i + 2));
					i += 2;
				} else {
					buf.append("%25"); //$NON-NLS-1$
				}
			} else if ("\"<>%\\^[]`+$,{}`~| ".indexOf(ch) == -1) { //$NON-NLS-1$
				buf.append(ch);
			} else {
				byte[] bytes = null;
				try {
					bytes = new String(new char[] { ch }).getBytes("UTF-8"); //$NON-NLS-1$
				} catch (UnsupportedEncodingException e) {
					throw new AssertionError(e);
				}
				for (int j = 0; j < bytes.length; j++) {
					buf.append('%');
					buf.append(digits.charAt((bytes[j] & 0xf0) >> 4));
					buf.append(digits.charAt(bytes[j] & 0xf));
				}
			}
		}
		return buf.toString();
	}

	public static String toASCIILowerCase(String s) {
		int len = s.length();
		StringBuilder buffer = new StringBuilder(len);
		for (int i = 0; i < len; i++) {
			char c = s.charAt(i);
			if ('A' <= c && c <= 'Z') {
				buffer.append((char) (c + ('a' - 'A')));
			} else {
				buffer.append(c);
			}
		}
		return buffer.toString();
	}

	public static String toASCIIUpperCase(String s) {
		if (s == null) { // by jshwang
			System.err.println("[toASCIIUpperCase] Warning: null string");
			return s;
		}
		int len = s.length();
		StringBuilder buffer = new StringBuilder(len);
		for (int i = 0; i < len; i++) {
			char c = s.charAt(i);
			if ('a' <= c && c <= 'z') {
				buffer.append((char) (c - ('a' - 'A')));
			} else {
				buffer.append(c);
			}
		}
		return buffer.toString();
	}
}
