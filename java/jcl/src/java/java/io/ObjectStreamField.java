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

package java.io;

// BEGIN android-note
// Harmony uses ObjectAccessors to access fields through JNI. Android has not
// yet migrated that API.
// END android-note

import java.lang.ref.WeakReference;
import java.util.Arrays;
import java.util.Comparator;

// BEGIN android-removed
// import org.apache.harmony.misc.accessors.ObjectAccessor;
// END android-removed

/**
 * Describes a field for the purpose of serialization. Classes can define the
 * collection of fields that are serialized, which may be different from the set
 * of all declared fields.
 * 
 * @see ObjectOutputStream#writeFields()
 * @see ObjectInputStream#readFields()
 */
public class ObjectStreamField implements Comparable<Object> {

	static final int FIELD_IS_NOT_RESOLVED = -1;
	static final int FIELD_IS_ABSENT = -2;

	// Declared name of the field
	private String name;

	// Declared type of the field
	private Object type;

	// offset of this field in the object
	int offset;

	// Cached version of intern'ed type String
	private String typeString;

	private boolean unshared;

	private boolean isDeserialized;

	// BEGIN android-removed
	// private long assocFieldID = FIELD_IS_NOT_RESOLVED;
	//
	// long getFieldID(ObjectAccessor accessor, Class<?> declaringClass) {
	// if (assocFieldID != FIELD_IS_NOT_RESOLVED) {
	// return assocFieldID;
	// } else {
	// try {
	// assocFieldID = accessor.getFieldID(declaringClass, name);
	// } catch(NoSuchFieldError e) {
	// assocFieldID = FIELD_IS_ABSENT;
	// }
	// return assocFieldID;
	// }
	// }
	// END android-removed

	/**
	 * Constructs an ObjectStreamField with the specified name and type.
	 * 
	 * @param name
	 *            the name of the field.
	 * @param cl
	 *            the type of the field.
	 * @throws NullPointerException
	 *             if {@code name} or {@code cl} is {@code null}.
	 */
	public ObjectStreamField(String name, Class<?> cl) {
		if (name == null || cl == null) {
			throw new NullPointerException();
		}
		this.name = name;
		this.type = new WeakReference<Class<?>>(cl);
	}

	/**
	 * Constructs an ObjectStreamField with the specified name, type and the
	 * indication if it is unshared.
	 * 
	 * @param name
	 *            the name of the field.
	 * @param cl
	 *            the type of the field.
	 * @param unshared
	 *            {@code true} if the field is written and read unshared;
	 *            {@code false} otherwise.
	 * @throws NullPointerException
	 *             if {@code name} or {@code cl} is {@code null}.
	 * @see ObjectOutputStream#writeUnshared(Object)
	 */
	public ObjectStreamField(String name, Class<?> cl, boolean unshared) {
		if (name == null || cl == null) {
			throw new NullPointerException();
		}
		this.name = name;
		this.type = (cl.getClassLoader() == null) ? cl
				: new WeakReference<Class<?>>(cl);
		this.unshared = unshared;
	}

	/**
	 * Constructs an ObjectStreamField with the given name and the given type.
	 * The type may be null.
	 * 
	 * @param signature
	 *            A String representing the type of the field
	 * @param name
	 *            a String, the name of the field, or null
	 */
	ObjectStreamField(String signature, String name) {
		if (name == null) {
			throw new NullPointerException();
		}
		this.name = name;
		this.typeString = signature.replace('.', '/').intern();
		defaultResolve();
		this.isDeserialized = true;
	}

	/**
	 * Compares this field descriptor to the specified one. Checks first if one
	 * of the compared fields has a primitive type and the other one not. If so,
	 * the field with the primitive type is considered to be "smaller". If both
	 * fields are equal, their names are compared.
	 * 
	 * @param o
	 *            the object to compare with.
	 * @return -1 if this field is "smaller" than field {@code o}, 0 if both
	 *         fields are equal; 1 if this field is "greater" than field
	 *         {@code o}.
	 */
	public int compareTo(Object o) {
		ObjectStreamField f = (ObjectStreamField) o;
		boolean thisPrimitive = this.isPrimitive();
		boolean fPrimitive = f.isPrimitive();

		// If one is primitive and the other isn't, we have enough info to
		// compare
		if (thisPrimitive != fPrimitive) {
			return thisPrimitive ? -1 : 1;
		}

		// Either both primitives or both not primitives. Compare based on name.
		return this.getName().compareTo(f.getName());
	}

	// BEGIN android-removed
	// There shouldn't be an implementation of these methods.
	// @Override
	// public boolean equals(Object arg0) {
	// return (arg0 instanceof ObjectStreamField) && (compareTo(arg0) == 0);
	// }
	//
	// @Override
	// public int hashCode() {
	// return getName().hashCode();
	// }
	// END android-removed

	/**
	 * Gets the name of this field.
	 * 
	 * @return the field's name.
	 */
	public String getName() {
		return name;
	}

	/**
	 * Gets the offset of this field in the object.
	 * 
	 * @return this field's offset.
	 */
	public int getOffset() {
		return offset;
	}

	/**
	 * Return the type of the field the receiver represents, this is an internal
	 * method
	 * 
	 * @return A Class object representing the type of the field
	 */
	// BEGIN android-note
	// Changed from private to default visibility for usage in ObjectStreamClass
	// END android-note
	/* package */Class<?> getTypeInternal() {
		if (type instanceof WeakReference) {
			return (Class<?>) ((WeakReference<?>) type).get();
		}
		return (Class<?>) type;
	}

	/**
	 * Gets the type of this field.
	 * 
	 * @return a {@code Class} object representing the type of the field.
	 */
	public Class<?> getType() {
		Class<?> cl = getTypeInternal();
		if (isDeserialized && !cl.isPrimitive()) {
			return Object.class;
		}
		return cl;
	}

	/**
	 * Gets a character code for the type of this field. The following codes are
	 * used:
	 * 
	 * <pre>
	 * B     byte
	 * C     char
	 * D     double
	 * F     float
	 * I     int
	 * J     long
	 * L     class or interface
	 * S     short
	 * Z     boolean
	 * [     array
	 * </pre>
	 * 
	 * @return the field's type code.
	 */
	public char getTypeCode() {
		Class<?> t = getTypeInternal();
		if (t == Integer.TYPE) {
			return 'I';
		}
		if (t == Byte.TYPE) {
			return 'B';
		}
		if (t == Character.TYPE) {
			return 'C';
		}
		if (t == Short.TYPE) {
			return 'S';
		}
		if (t == Boolean.TYPE) {
			return 'Z';
		}
		if (t == Long.TYPE) {
			return 'J';
		}
		if (t == Float.TYPE) {
			return 'F';
		}
		if (t == Double.TYPE) {
			return 'D';
		}
		if (t.isArray()) {
			return '[';
		}
		return 'L';
	}

	/**
	 * Gets the type signature used by the VM to represent the type of this
	 * field.
	 * 
	 * @return the signature of this field's class or {@code null} if this
	 *         field's type is primitive.
	 */
	public String getTypeString() {
		if (isPrimitive()) {
			return null;
		}
		if (typeString == null) {
			Class<?> t = getTypeInternal();
			String typeName = t.getName().replace('.', '/');
			String str = (t.isArray()) ? typeName : ("L" + typeName + ';');
			typeString = str.intern();
		}
		return typeString;
	}

	/**
	 * Indicates whether this field's type is a primitive type.
	 * 
	 * @return {@code true} if this field's type is primitive; {@code false} if
	 *         the type of this field is a regular class.
	 */
	public boolean isPrimitive() {
		Class<?> t = getTypeInternal();
		return t != null && t.isPrimitive();
	}

	/**
	 * Sets this field's offset in the object.
	 * 
	 * @param newValue
	 *            the field's new offset.
	 */
	protected void setOffset(int newValue) {
		this.offset = newValue;
	}

	/**
	 * Returns a string containing a concise, human-readable description of this
	 * field descriptor.
	 * 
	 * @return a printable representation of this descriptor.
	 */
	@Override
	public String toString() {
		return this.getClass().getName() + '(' + getName() + ':'
				+ getTypeInternal() + ')';
	}

	/**
	 * Sorts the fields for dumping. Primitive types come first, then regular
	 * types.
	 * 
	 * @param fields
	 *            ObjectStreamField[] fields to be sorted
	 */
	static void sortFields(ObjectStreamField[] fields) {
		// Sort if necessary
		if (fields.length > 1) {
			Comparator<ObjectStreamField> fieldDescComparator = new Comparator<ObjectStreamField>() {
				public int compare(ObjectStreamField f1, ObjectStreamField f2) {
					return f1.compareTo(f2);
				}
			};
			Arrays.sort(fields, fieldDescComparator);
		}
	}

	void resolve(ClassLoader loader) {
		if (typeString == null && isPrimitive()) {
			// primitive type declared in a serializable class
			typeString = String.valueOf(getTypeCode());
		}

		if (typeString.length() == 1) {
			if (defaultResolve()) {
				return;
			}
		}

		String className = typeString.replace('/', '.');
		if (className.charAt(0) == 'L') {
			// remove L and ;
			className = className.substring(1, className.length() - 1);
		}
		try {
			Class<?> cl = Class.forName(className, false, loader);
			type = (cl.getClassLoader() == null) ? cl
					: new WeakReference<Class<?>>(cl);
		} catch (ClassNotFoundException e) {
			// Ignored
		}
	}

	/**
	 * Indicates whether this field is unshared.
	 * 
	 * @return {@code true} if this field is unshared, {@code false} otherwise.
	 */
	public boolean isUnshared() {
		return unshared;
	}

	void setUnshared(boolean unshared) {
		this.unshared = unshared;
	}

	/**
	 * Resolves typeString into type. Returns true if the type is primitive and
	 * false otherwise.
	 */
	private boolean defaultResolve() {
		switch (typeString.charAt(0)) {
		case 'I':
			type = Integer.TYPE;
			return true;
		case 'B':
			type = Byte.TYPE;
			return true;
		case 'C':
			type = Character.TYPE;
			return true;
		case 'S':
			type = Short.TYPE;
			return true;
		case 'Z':
			type = Boolean.TYPE;
			return true;
		case 'J':
			type = Long.TYPE;
			return true;
		case 'F':
			type = Float.TYPE;
			return true;
		case 'D':
			type = Double.TYPE;
			return true;
		default:
			type = Object.class;
			return false;
		}
	}
}
