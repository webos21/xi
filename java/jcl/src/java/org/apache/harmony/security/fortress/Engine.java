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

/**
 * @author Boris V. Kuznetsov
 * @version $Revision$
 */

package org.apache.harmony.security.fortress;

import java.security.NoSuchAlgorithmException;
import java.security.Provider;
import org.apache.harmony.security.Util;

/**
 * 
 * This class implements common functionality for all engine classes
 * 
 */
public class Engine {

	// Service name
	private final String serviceName;

	// for getInstance(String algorithm, Object param) optimization:
	// previous result
	private Provider.Service returnedService;

	// previous parameter
	private String lastAlgorithm;

	private int refreshNumber;

	/**
	 * Provider
	 */
	public Provider provider;

	/**
	 * SPI instance
	 */
	public Object spi;

	/**
	 * Access to package visible api in java.security
	 */
	public static SecurityAccess door;

	/**
	 * Creates a Engine object
	 * 
	 * @param service
	 */
	public Engine(String service) {
		this.serviceName = service;
	}

	/**
	 * 
	 * Finds the appropriate service implementation and creates instance of the
	 * class that implements corresponding Service Provider Interface.
	 * 
	 * @param algorithm
	 * @param service
	 * @throws NoSuchAlgorithmException
	 */
	public synchronized void getInstance(String algorithm, Object param)
			throws NoSuchAlgorithmException {
		Provider.Service serv;

		if (algorithm == null) {
			throw new NoSuchAlgorithmException("Null algorithm name");
		}
		Services.refresh();
		if (returnedService != null
				&& Util.equalsIgnoreCase(algorithm, lastAlgorithm)
				&& refreshNumber == Services.refreshNumber) {
			serv = returnedService;
		} else {
			if (Services.isEmpty()) {
				throw notFound(serviceName, algorithm);
			}
			serv = Services.getService(new StringBuilder(128)
					.append(serviceName).append(".")
					.append(Util.toUpperCase(algorithm)).toString());
			if (serv == null) {
				throw notFound(serviceName, algorithm);
			}
			returnedService = serv;
			lastAlgorithm = algorithm;
			refreshNumber = Services.refreshNumber;
		}
		spi = serv.newInstance(param);
		this.provider = serv.getProvider();
	}

	private NoSuchAlgorithmException notFound(String serviceName,
			String algorithm) throws NoSuchAlgorithmException {
		throw new NoSuchAlgorithmException(serviceName + " " + algorithm
				+ " implementation not found");
	}

	/**
	 * 
	 * Finds the appropriate service implementation and creates instance of the
	 * class that implements corresponding Service Provider Interface.
	 * 
	 * @param algorithm
	 * @param service
	 * @param provider
	 * @throws NoSuchAlgorithmException
	 */
	public synchronized void getInstance(String algorithm, Provider provider,
			Object param) throws NoSuchAlgorithmException {

		Provider.Service serv = null;
		if (algorithm == null) {
			throw new NoSuchAlgorithmException("algorithm == null");
		}
		serv = provider.getService(serviceName, algorithm);
		if (serv == null) {
			throw notFound(serviceName, algorithm);
		}
		spi = serv.newInstance(param);
		this.provider = provider;
	}

}
