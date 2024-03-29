/*
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package java.util.jar;

import java.util.zip.ZipException;

/**
 * This runtime exception is thrown when a problem occurs while reading a JAR
 * file.
 */
public class JarException extends ZipException {

	private static final long serialVersionUID = 7159778400963954473L;

	/**
	 * Constructs a new {@code JarException} instance.
	 */
	public JarException() {
		super();
	}

	/**
	 * Constructs a new {@code JarException} instance with the specified
	 * message.
	 * 
	 * @param detailMessage
	 *            the detail message for the exception.
	 */
	public JarException(String detailMessage) {
		super(detailMessage);
	}
}
