/* Licensed to the Apache Software Foundation (ASF) under one or more
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

package org.apache.harmony.nio.internal;

import java.nio.channels.CancelledKeyException;
import java.nio.channels.SelectableChannel;
import java.nio.channels.SelectionKey;
import java.nio.channels.Selector;
import java.nio.channels.SocketChannel;
import java.nio.channels.spi.AbstractSelectableChannel;
import java.nio.channels.spi.AbstractSelectionKey;

/**
 * Default implementation of SelectionKey
 */
final class SelectionKeyImpl extends AbstractSelectionKey {

	private AbstractSelectableChannel channel;

	private int interestOps;

	private int readyOps;

	private SelectorImpl selector;

	// BEGIN android-removed
	// private int index;
	// END android-removed

	public SelectionKeyImpl(AbstractSelectableChannel channel, int operations,
			Object attachment, SelectorImpl selector) {
		this.channel = channel;
		interestOps = operations;
		this.selector = selector;
		attach(attachment);
	}

	@Override
	public SelectableChannel channel() {
		return channel;
	}

	@Override
	public int interestOps() {
		checkValid();
		synchronized (selector.keysLock) {
			return interestOps;
		}
	}

	int interestOpsNoCheck() {
		synchronized (selector.keysLock) {
			return interestOps;
		}
	}

	@Override
	public SelectionKey interestOps(int operations) {
		checkValid();
		if ((operations & ~(channel().validOps())) != 0) {
			throw new IllegalArgumentException();
		}
		synchronized (selector.keysLock) {
			interestOps = operations;
			// BEGIN android-removed
			// selector.modKey(this);
			// END android-removed
		}
		return this;
	}

	@Override
	public int readyOps() {
		checkValid();
		return readyOps;
	}

	@Override
	public Selector selector() {
		return selector;
	}

	/*
	 * package private method for setting the ready operation by selector
	 */
	void setReadyOps(int readyOps) {
		this.readyOps = readyOps;
	}

	// BEGIN android-removed
	// int getIndex() {
	// return index;
	// }

	// void setIndex(int index) {
	// this.index = index;
	// }
	// END android-removed

	private void checkValid() {
		if (!isValid()) {
			throw new CancelledKeyException();
		}
	}

	/**
	 * Returns true if the channel for this key is connected. If the channel
	 * does not need connecting, this always return true.
	 */
	boolean isConnected() {
		return !(channel instanceof SocketChannel)
				|| ((SocketChannel) channel).isConnected();
	}
}
