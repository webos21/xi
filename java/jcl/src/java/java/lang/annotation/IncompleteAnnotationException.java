/* IncompleteAnnotationException.java - Thrown when annotation has changed
   Copyright (C) 2004 Free Software Foundation

This file is part of GNU Classpath.

GNU Classpath is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GNU Classpath is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU Classpath; see the file COPYING.  If not, write to the
Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
02110-1301 USA.

Linking this library statically or dynamically with other modules is
making a combined work based on this library.  Thus, the terms and
conditions of the GNU General Public License cover the whole
combination.

As a special exception, the copyright holders of this library give you
permission to link this library with independent modules to produce an
executable, regardless of the license terms of these independent
modules, and to copy and distribute the resulting executable under
terms of your choice, provided that you also meet, for each linked
independent module, the terms and conditions of the license of that
module.  An independent module is a module which is not derived from
or based on this library.  If you modify this library, you may extend
this exception to your version of the library, but you are not
obligated to do so.  If you do not wish to do so, delete this
exception statement from your version. */

package java.lang.annotation;

/**
 * Thrown when accessing an element within an annotation which was added since
 * compilation or serialization took place, and does not have a default value.
 * 
 * @author Tom Tromey (tromey@redhat.com)
 * @author Andrew John Hughes (gnu_andrew@member.fsf.org)
 * @since 1.5
 */
public class IncompleteAnnotationException extends RuntimeException {

	/**
	 * Constructs a new <code>IncompleteAnnotationException</code> which
	 * indicates that the element, <code>name</code>, was missing from the
	 * annotation, <code>type</code> at compile time and does not have a default
	 * value.
	 * 
	 * @param type
	 *            the type of annotation from which an element is missing.
	 * @param name
	 *            the name of the missing element.
	 */
	public IncompleteAnnotationException(Class<? extends Annotation> type,
			String name) {
		this.annotationType = type;
		this.elementName = name;
	}

	/**
	 * Returns the class representing the type of annotation from which an
	 * element was missing.
	 * 
	 * @return the type of annotation.
	 */
	public Class<? extends Annotation> annotationType() {
		return annotationType;
	}

	/**
	 * Returns the name of the missing annotation element.
	 * 
	 * @return the element name.
	 */
	public String elementName() {
		return elementName;
	}

	// Names are chosen from serialization spec.

	/**
	 * The class representing the type of annotation from which an element was
	 * found to be missing.
	 * 
	 * @serial the type of the annotation from which an element was missing.
	 */
	private Class<? extends Annotation> annotationType;

	/**
	 * The name of the missing element.
	 * 
	 * @serial the name of the missing element.
	 */
	private String elementName;

}
