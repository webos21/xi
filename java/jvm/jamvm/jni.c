/*
 * Copyright (C) 2003, 2004, 2005, 2006, 2007, 2008, 2009
 * Robert Lougher <rob@jamvm.org.uk>.
 *
 * This file is part of JamVM.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2,
 * or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "xi/xi_string.h"
#include "xi/xi_mem.h"

#include "jni.h"
#include "jam.h"
#include "thread.h"
#include "lock.h"
#include "symbol.h"
#include "excep.h"
#include "reflect.h"
#include "jni-internal.h"
#include "alloc.h"

#define JNI_VERSION JNI_VERSION_1_6

/* The extra used in expanding the local refs table.
 * This must be >= size of JNIFrame to be thread safe
 * wrt GC thread suspension */
#define LREF_LIST_INCR 8

/* The extra used in expanding the global refs table.
 * Also the initial list size */
#define GREF_LIST_INCR 32

/* The amount of local reference space "ensured" before
 entering a JNI method.  The method is garaunteed to
 be able to create this amount without failure */
#define JNI_DEFAULT_LREFS 16

/* Forward declarations */
const jchar *Jam_GetStringChars(JNIEnv *env, jstring string, jboolean *isCopy);
void Jam_ReleaseStringChars(JNIEnv *env, jstring string, const jchar *chars);
jobject Jam_NewGlobalRef(JNIEnv *env, jobject obj);
void Jam_DeleteGlobalRef(JNIEnv *env, jobject obj);
JNIFrame *ensureJNILrefCapacity(int cap);
static void initJNIGrefs();

static char nio_init_OK = FALSE;

/* Cached values initialised on startup for JNI 1.4 NIO support */
// by jshwang - temporarily block till implement NewDirectByteBuffer.
static Class *directBufferClass, *readWriteBufferClass;
static MethodBlock *directBuff_getEffectiveAddr_mb;
static int platformAddr_toLong_voffset;
static MethodBlock *platformAddrFac_on_mb;
static MethodBlock *readWriteBuff_init_mb;
static int platformAddr_osaddr_offset;
static int buffer_capacity_offset;
static int buffer_effectiveDirectAddr_offset;

void initialiseJNI() {
	Class *bufferClass;
	Class *platformAddressClass, *platformAddressFactoryClass;
	MethodBlock *platformAddr_toLong_mb;
	FieldBlock *platformAddr_osaddr_fb;
	FieldBlock *buffer_capacity_fb;
	FieldBlock *buffer_effectiveDirectAddr_fb;

	/* Initialise the global reference tables */
	initJNIGrefs();

	/*
	 * Look up and cache pointers to some direct buffer classes, fields,
	 * and methods.
	 */
	platformAddressClass = findSystemClass0(
			SYMBOL(org_apache_harmony_luni_platform_PlatformAddress));
	platformAddressFactoryClass = findSystemClass0(
			SYMBOL(org_apache_harmony_luni_platform_PlatformAddressFactory));
	directBufferClass = findSystemClass0(
			SYMBOL(org_apache_harmony_nio_internal_DirectBuffer));
	readWriteBufferClass = findSystemClass0(
			SYMBOL(java_nio_ReadWriteDirectByteBuffer));
	bufferClass = findSystemClass0(SYMBOL(java_nio_Buffer));

	if (platformAddressClass == NULL || platformAddressFactoryClass == NULL
			|| directBufferClass == NULL || readWriteBufferClass == NULL
			|| bufferClass == NULL) {
		log_error(XDLOG, "=======> Unable to find internal direct buffer classes\n");
		return;
	}

	/*
	 * We need a Method* here rather than a vtable offset, because
	 * DirectBuffer is an interface class.
	 */
	// ToDo: find virtual method
	directBuff_getEffectiveAddr_mb = findMethod(directBufferClass,
			SYMBOL(getEffectiveAddress),
			SYMBOL(___org_apache_harmony_luni_platform_PlatformAddress));
	if (directBuff_getEffectiveAddr_mb == NULL) {
		log_error(XDLOG, "=======> Unable to find PlatformAddress.getEffectiveAddress\n");
		return;
	}

	// ToDo: find virtual method
	platformAddr_toLong_mb = findMethod(platformAddressClass, SYMBOL(toLong),
			SYMBOL(___J));
	if (platformAddr_toLong_mb == NULL) {
		log_error(XDLOG, "=========> Unable to find PlatformAddress.toLong\n");
		return;
	}
	platformAddr_toLong_voffset = platformAddr_toLong_mb->method_table_index;

	platformAddrFac_on_mb = findMethod(platformAddressFactoryClass, SYMBOL(on),
			SYMBOL(_I___org_apache_harmony_luni_platform_PlatformAddress));

	if (platformAddrFac_on_mb == NULL) {
		log_error(XDLOG, "=========> Unable to find PlatformAddressFactory.on\n");
		return;
	}

	readWriteBuff_init_mb = findMethod(readWriteBufferClass,
			SYMBOL(object_init),
			SYMBOL(_org_apache_harmony_luni_platform_PlatformAddress_I_I__V));
	if (readWriteBuff_init_mb == NULL) {
		log_error(XDLOG, "=========> Unable to find ReadWriteDirectByteBuffer.<init>\n");
		return;
	}

	platformAddr_osaddr_fb = findField(platformAddressClass, SYMBOL(osaddr),
			SYMBOL(I));
	if (platformAddr_osaddr_fb == NULL) {
		log_error(XDLOG, "=========> Unable to find PlatformAddress.osaddr\n");
		return;
	}
	platformAddr_osaddr_offset = platformAddr_osaddr_fb->u.offset;

	buffer_capacity_fb = findField(bufferClass, SYMBOL(capacity), SYMBOL(I));
	if (buffer_capacity_fb == NULL) {
		log_error(XDLOG, "=========> Unable to find Buffer.capacity\n");
		return;
	}
	buffer_capacity_offset = buffer_capacity_fb->u.offset;

	buffer_effectiveDirectAddr_fb = findField(bufferClass,
			SYMBOL(effectiveDirectAddress), SYMBOL(I));
	if (buffer_effectiveDirectAddr_fb == NULL) {
		log_error(XDLOG, "==========> Unable to find Buffer.effectiveDirectAddress\n");
		return;
	}
	buffer_effectiveDirectAddr_offset = buffer_effectiveDirectAddr_fb->u.offset;

	registerStaticClassRef(&directBufferClass);
	registerStaticClassRef(&readWriteBufferClass);

	nio_init_OK = TRUE;
	log_debug(XDLOG, "=====> initJNI OK\n");
}

/* ---------- Local reference support functions ---------- */

int initJNILrefs() {
	JNIFrame *frame = ensureJNILrefCapacity(JNI_DEFAULT_LREFS);

	if (frame != NULL) {
		frame->next_ref = frame->lrefs + frame->mb->args_count;
		return TRUE;
	}

	return FALSE;
}

JNIFrame *expandJNILrefs(ExecEnv *ee, JNIFrame *frame, int incr) {
	JNIFrame *new_frame = (JNIFrame*) ((Object**) frame + incr);

	if ((char*) (new_frame + 1) > ee->stack_end)
		return NULL;

	xi_mem_copy(new_frame, frame, sizeof(JNIFrame));
	new_frame->ostack = (xuintptr*) (new_frame + 1);
	ee->last_frame = (Frame*) new_frame;
	xi_mem_set(frame, 0, incr * sizeof(Object*));
	return new_frame;
}

JNIFrame *ensureJNILrefCapacity(int cap) {
	ExecEnv *ee = getExecEnv();
	JNIFrame *frame = (JNIFrame*) ee->last_frame;
	int size = (Object**) frame - frame->lrefs - frame->mb->args_count;

	if (size < cap) {
		int incr = cap - size;
		if (incr < sizeof(JNIFrame) / sizeof(Object*))
			incr = sizeof(JNIFrame) / sizeof(Object*);

		if ((frame = expandJNILrefs(ee, frame, incr)) == NULL)
			signalException(java_lang_OutOfMemoryError, "JNI local references");
	}

	return frame;
}

jobject addJNILref(Object *ref) {
	ExecEnv *ee = getExecEnv();
	JNIFrame *frame = (JNIFrame*) ee->last_frame;

	if (ref == NULL) {
		return NULL;
	}

	if (frame->next_ref == (Object**) frame) {
		if ((frame = expandJNILrefs(ee, frame, LREF_LIST_INCR)) == NULL) {
			//JNIFrame *forceSeg = NULL;
			//log_info(XDLOG, "p=%p\n", forceSeg->next_ref);
			// XXX : debug code for addJNILref
			//			char buffer[256];
			//			String2Buff(INST_DATA(ee->thread, Object*, 52), buffer,
			//					sizeof(buffer));
			//			log_info(XDLOG, "[addJNILref] Object=%s\n", CLASS_CB(ref->class)->name);
			//			log_info(XDLOG, "[addJNILref] thread=%s / stack=%p / last_frame=%p / stack_size=%d / stack_end=%p\n",
			//					buffer, ee->stack, ee->last_frame, ee->stack_size, ee->stack_end);
			log_error(XDLOG,
					"JNI - FatalError: cannot expand local references.\n");
			exitVM(1);
		}
	}

	return *frame->next_ref++ = ref;
}

void delJNILref(Object *ref) {
	ExecEnv *ee = getExecEnv();
	JNIFrame *frame = (JNIFrame*) ee->last_frame;
	Object **opntr = frame->lrefs;

	for (; opntr < frame->next_ref; opntr++)
		if (*opntr == ref) {
			*opntr = NULL;
			return;
		}
}

JNIFrame *pushJNILrefFrame(int cap) {
	ExecEnv *ee = getExecEnv();
	JNIFrame *frame = (JNIFrame*) ee->last_frame;
	JNIFrame *new_frame = (JNIFrame*) ((Object**) (frame + 1) + cap);

	if ((char*) (new_frame + 1) > ee->stack_end) {
		signalException(java_lang_OutOfMemoryError, "JNI local references");
		return NULL;
	}

	new_frame->lrefs = new_frame->next_ref = (Object**) (frame + 1);
	new_frame->ostack = (xuintptr*) (new_frame + 1);
	new_frame->prev = frame->prev;
	new_frame->mb = frame->mb;

	xi_mem_set(frame + 1, 0, cap * sizeof(Object*));
	ee->last_frame = (Frame*) new_frame;

	return new_frame;
}

void popJNILrefFrame() {
	ExecEnv *ee = getExecEnv();
	JNIFrame *frame = (JNIFrame*) ee->last_frame;
	JNIFrame *prev = (JNIFrame*) frame->lrefs - 1;

	ee->last_frame = (Frame*) prev;
}

/* ----------------- Global reference support ----------------- */

/* There are 3 global reference tables, one for normal (strong)
 global references, one for weak global references and one for
 weak references that have been cleared */
#define NUM_GLOBAL_TABLES 3

typedef struct global_ref_table {
	Object **table;
	int size;
	int next;
	int has_deleted;
	VMLock lock;
} GlobalRefTable;

static GlobalRefTable global_refs[NUM_GLOBAL_TABLES];

static void initJNIGrefs() {
	int i;

	for (i = 0; i < NUM_GLOBAL_TABLES; i++)
		initVMLock(global_refs[i].lock);
}

void lockJNIGrefs(Thread *self, int type) {
	/* Disabling and enabling suspension is slow,
	 so only do it if we have to block */
	if (!tryLockVMLock(global_refs[type].lock, self)) {
		disableSuspend(self);
		lockVMLock(global_refs[type].lock, self);
		enableSuspend(self);
	}
	fastDisableSuspend(self);
}

void unlockJNIGrefs(Thread *self, int type) {
	fastEnableSuspend(self);
	unlockVMLock(global_refs[type].lock, self);
}

void addJNIGrefUnlocked(Object *ref, int type) {
	if (global_refs[type].next == global_refs[type].size) {
		/* To save the cost of shuffling entries every time a ref is deleted,
		 refs are simply nulled-out, and a flag is set.  We then compact
		 the table when it becomes full */
		if (global_refs[type].has_deleted) {
			int i, j;

			for (i = j = 0; i < global_refs[type].size; i++)
				if (global_refs[type].table[i])
					global_refs[type].table[j++] = global_refs[type].table[i];

			global_refs[type].has_deleted = FALSE;
			global_refs[type].next = j;
		}

		if (global_refs[type].next + GREF_LIST_INCR > global_refs[type].size) {
			global_refs[type].size = global_refs[type].next + GREF_LIST_INCR;
			global_refs[type].table = sysRealloc(global_refs[type].table,
					global_refs[type].size * sizeof(Object*));
		}
	}

	global_refs[type].table[global_refs[type].next++] = ref;
}

static jobject addJNIGref(Object *ref, int type) {
	if (ref == NULL)
		return NULL;
	else {
		Thread *self = threadSelf();

		lockJNIGrefs(self, type);
		addJNIGrefUnlocked(ref, type);
		unlockJNIGrefs(self, type);

		return OBJ_TO_REF(ref, type);
	}
}

static int delJNIGref(Object *ref, int type) {
	Thread *self = threadSelf();
	int i;

	lockJNIGrefs(self, type);

	/* As NewRef and DeleteRef are paired, we search
	 backwards to optimise the case where the ref
	 has recently been added */
	for (i = global_refs[type].next - 1; i >= 0; i--)
		if (global_refs[type].table[i] == ref) {

			/* If it's the last ref we can just update
			 the next free index */
			if (i == global_refs[type].next - 1)
				global_refs[type].next = i;
			else {
				global_refs[type].table[i] = NULL;
				global_refs[type].has_deleted = TRUE;
			}
			break;
		}

	unlockJNIGrefs(self, type);
	return i >= 0;
}

/* Called during mark phase of GC.  No need to
 grab lock as no thread can be suspended
 while the list is being modified */

#define MARK_JNI_GLOBAL_REFS(type, ref_type)                    \
void markJNI##type##Refs() {                                    \
    int i;                                                      \
                                                                \
    for(i = 0; i < global_refs[ref_type].next; i++)             \
        if(global_refs[ref_type].table[i])                      \
            markJNI##type##Ref(global_refs[ref_type].table[i]); \
}

MARK_JNI_GLOBAL_REFS(Global, GLOBAL_REF)
MARK_JNI_GLOBAL_REFS(ClearedWeak, CLEARED_WEAK_REF)

void scanJNIWeakGlobalRefs() {
	int i;

	for (i = 0; i < global_refs[WEAK_GLOBAL_REF].next; i++) {
		Object *ref = global_refs[WEAK_GLOBAL_REF].table[i];

		if (ref != NULL && !isMarkedJNIWeakGlobalRef(ref)) {
			addJNIGrefUnlocked(ref, CLEARED_WEAK_REF);
			global_refs[WEAK_GLOBAL_REF].table[i] = NULL;
			global_refs[WEAK_GLOBAL_REF].has_deleted = TRUE;
		}
	}
}

/* --------------- JNI interface functions --------------- */

/* Extensions added to JNI in JDK 1.6 */

jobjectRefType Jam_GetObjectRefType(JNIEnv *env, jobject obj) {
	if (obj != NULL) {
		switch (REF_TYPE(obj)) {
		case GLOBAL_REF:
			return JNIGlobalRefType;

		case WEAK_GLOBAL_REF:
			return JNIWeakGlobalRefType;

		case LOCAL_REF:
			return JNILocalRefType;

		default:
			break;
		}
	}

	return JNIInvalidRefType;
}

/* Extensions added to JNI in JDK 1.4 */

/*
 * Allocate and return a new java.nio.ByteBuffer for this block of memory.
 *
 * "address" may not be NULL, and "capacity" must be > 0.  (These are only
 * verified when CheckJNI is enabled.)
 */
static jobject Jam_NewDirectByteBuffer(JNIEnv *env, void *addr, jlong capacity) {
	Object *platformAddress = NULL;
	Class *tmpClazz;
	Object *newobj;
	jobject result = NULL;

	if (!nio_init_OK)
		return NULL;

	log_info(XDLOG, "\n++++++++++++++++++++++++++++++\n");
	log_info(XDLOG, "+++++ NewDirectByteBuffer +++++\n");
	log_info(XDLOG, "+++++++++++++++++++++++++++++++\n");

	////////////////////////////////////////////////////
	tmpClazz = platformAddrFac_on_mb->class;
	if (initClass(tmpClazz) == NULL) {
		return NULL;
	}

	/* get an instance of PlatformAddress that wraps the provided address */
	platformAddress = executeStaticMethod(platformAddrFac_on_mb->class,
			platformAddrFac_on_mb, addr);
	if (exceptionOccurred() != NULL || platformAddress == NULL) {
		return NULL;
	}

	/* create an instance of java.nio.ReadWriteDirectByteBuffer */
	tmpClazz = readWriteBufferClass;
	if (initClass(tmpClazz) == NULL) {
		return NULL;
	}

	newobj = allocObject(tmpClazz);
	if (newobj != NULL) {
		/* call the (PlatformAddress, int, int) constructor */
		result = addJNILref(newobj);
		executeMethod(newobj, readWriteBuff_init_mb, platformAddress,
				(jint) capacity, (jint) 0);
		if (exceptionOccurred() != NULL) {
			delJNILref(result);
			result = NULL;
		}
	}

	return result;
}

static void *Jam_GetDirectBufferAddress(JNIEnv *env, jobject buffer) {
	Object *buff = REF_TO_OBJ(buffer);
	const MethodBlock *meth;
	Object *platformAddr;
	void *result;

	if (!nio_init_OK)
		return NULL;

	log_info(XDLOG, "\n++++++++++++++++++++++++++++++\n");
	log_info(XDLOG, "++++ GetDirectBufferAddress +++\n");
	log_info(XDLOG, "+++++++++++++++++++++++++++++++\n");

	if (buff == NULL)
		return NULL;

	/*
	 * All Buffer objects have an effectiveDirectAddress field.  If it's
	 * nonzero, we can just return that value.  If not, we have to call
	 * through DirectBuffer.getEffectiveAddress(), which as a side-effect
	 * will set the effectiveDirectAddress field for direct buffers (and
	 * things that wrap direct buffers).
	 */
	result = INST_DATA(buff, void*, buffer_effectiveDirectAddr_offset);
	if (result != NULL) {
		return result;
	}

	/*
	 * Start by determining if the object supports the DirectBuffer
	 * interfaces.  Note this does not guarantee that it's a direct buffer.
	 */
	if (buff->class == directBufferClass->class) {
		return result;
	}

	/*
	 * Get a PlatformAddress object with the effective address.
	 *
	 * If this isn't a direct buffer, the result will be NULL and/or an
	 * exception will have been thrown.
	 */
	meth = lookupVirtualMethod(buff, directBuff_getEffectiveAddr_mb);
	platformAddr = executeMethod(buff, meth);
	if (exceptionOccurred() != NULL) {
		clearException();
		platformAddr = NULL;
	}

	if (platformAddr == NULL) {
		log_info(XDLOG, "Got request for address of non-direct buffer\n");
		return result;
	}

	/*
	 * Extract the address from the PlatformAddress object.  Instead of
	 * calling the toLong() method, just grab the field directly.  This
	 * is faster but more fragile.
	 */
	return INST_DATA(platformAddr, void*, platformAddr_osaddr_offset);
}

/*
 * Get the capacity of the buffer for the specified java.nio.Buffer.
 *
 * Returns -1 if the object is not a direct buffer.  (We actually skip
 * this check, since it's expensive to determine, and just return the
 * capacity regardless.)
 */
static jlong Jam_GetDirectBufferCapacity(JNIEnv *env, jobject buffer) {
	Object *buff = REF_TO_OBJ(buffer);

	if (!nio_init_OK)
		return -1;

	log_info(XDLOG, "\n++++++++++++++++++++++++++++++\n");
	log_info(XDLOG, "+++ GetDirectBufferCapacity +++\n");
	log_info(XDLOG, "+++++++++++++++++++++++++++++++\n");

	/*
	 * The capacity is always in the Buffer.capacity field.
	 *
	 * (The "check" version should verify that this is actually a Buffer,
	 * but we're not required to do so here.)
	 */
	if (buff != NULL) {
		return INST_DATA(buff, jlong, buffer_capacity_offset);
	}

	return -1;
}

/* Extensions added to JNI in JDK 1.2 */

jmethodID Jam_FromReflectedMethod(JNIEnv *env, jobject method) {
	return mbFromReflectObject(REF_TO_OBJ(method));
}

jfieldID Jam_FromReflectedField(JNIEnv *env, jobject field) {
	return fbFromReflectObject(REF_TO_OBJ(field));
}

jobject Jam_ToReflectedMethod(JNIEnv *env, jclass cls, jmethodID methodID,
		jboolean isStatic) {

	MethodBlock *mb = methodID;
	Object *method;

	if (mb->name == SYMBOL(object_init)) {
		method = createReflectConstructorObject(mb);
	} else {
		method = createReflectMethodObject(mb);
	}

	return addJNILref(method);
}

jobject Jam_ToReflectedField(JNIEnv *env, jclass cls, jfieldID fieldID,
		jboolean isStatic) {
	Object *field = createReflectFieldObject(fieldID);
	return addJNILref(field);
}

jint Jam_PushLocalFrame(JNIEnv *env, jint capacity) {
	return pushJNILrefFrame(capacity) == NULL ? JNI_ERR : JNI_OK;
}

jobject Jam_PopLocalFrame(JNIEnv *env, jobject result) {
	popJNILrefFrame();
	return addJNILref(REF_TO_OBJ(result));
}

jobject Jam_NewLocalRef(JNIEnv *env, jobject obj) {
	return addJNILref(REF_TO_OBJ_WEAK_NULL_CHECK(obj));
}

jint Jam_EnsureLocalCapacity(JNIEnv *env, jint capacity) {
	return ensureJNILrefCapacity(capacity) == NULL ? JNI_ERR : JNI_OK;
}

void Jam_GetStringRegion(JNIEnv *env, jstring string_ref, jsize start,
		jsize len, jchar *buf) {

	Object *string = REF_TO_OBJ(string_ref);

	if ((start + len) > getStringLen(string))
		signalException(java_lang_StringIndexOutOfBoundsException, NULL);
	else
		xi_mem_copy(buf, getStringChars(string) + start, len * sizeof(short));
}

void Jam_GetStringUTFRegion(JNIEnv *env, jstring string_ref, jsize start,
		jsize len, char *buf) {

	Object *string = REF_TO_OBJ(string_ref);

	if ((start + len) > getStringLen(string))
		signalException(java_lang_StringIndexOutOfBoundsException, NULL);
	else
		StringRegion2Utf8(string, start, len, buf);
}

void *Jam_GetPrimitiveArrayCritical(JNIEnv *env, jarray array_ref,
		jboolean *isCopy) {

	Object *array = REF_TO_OBJ(array_ref);

	if (isCopy != NULL)
		*isCopy = JNI_FALSE;

	/* Pin the array */
	addJNIGref(array, GLOBAL_REF);

	return ARRAY_DATA(array, void);
}

void Jam_ReleasePrimitiveArrayCritical(JNIEnv *env, jarray array, void *carray,
		jint mode) {

	delJNIGref(REF_TO_OBJ(array), GLOBAL_REF);
}

const jchar *Jam_GetStringCritical(JNIEnv *env, jstring string,
		jboolean *isCopy) {

	return Jam_GetStringChars(env, string, isCopy);
}

void Jam_ReleaseStringCritical(JNIEnv *env, jstring string, const jchar *chars) {

	Jam_ReleaseStringChars(env, string, chars);
}

jweak Jam_NewWeakGlobalRef(JNIEnv *env, jobject obj) {
	return addJNIGref(REF_TO_OBJ_WEAK_NULL_CHECK(obj), WEAK_GLOBAL_REF);
}

void Jam_DeleteWeakGlobalRef(JNIEnv *env, jweak obj) {
	if (REF_TYPE(obj) == WEAK_GLOBAL_REF) {
		Object *ob = REF_TO_OBJ(obj);

		if (!delJNIGref(ob, WEAK_GLOBAL_REF))
			delJNIGref(ob, CLEARED_WEAK_REF);
	}
}

jboolean Jam_ExceptionCheck(JNIEnv *env) {
	return exceptionOccurred() ? JNI_TRUE : JNI_FALSE;
}

/* JNI 1.1 interface */

jint Jam_GetVersion(JNIEnv *env) {
	return JNI_VERSION;
}

jclass Jam_DefineClass(JNIEnv *env, const char *name, jobject loader,
		const jbyte *buf, jsize bufLen) {

	Class *class = defineClass((char*) name, (char *) buf, 0, (int) bufLen,
			REF_TO_OBJ(loader));

	return addJNILref(class);
}

jclass Jam_FindClass(JNIEnv *env, const char *name) {
	/* We use the class loader associated with the calling native method.
	 However, if this has been called from an attached thread there may
	 be no native Java frame.  In this case use the system class loader */
	Frame *last = getExecEnv()->last_frame;
	Object *loader;
	Class *class;

	if (last->prev) {
		ClassBlock *cb = CLASS_CB(last->mb->class);
		loader = cb->class_loader;

		/* Ensure correct context if called from JNI_OnLoad */
		if (loader == NULL) {
			if (cb->name == SYMBOL(java_lang_VMRuntime)) {
				loader = (Object*) last->lvars[1];
				log_debug(XDLOG, "           - - - - > loader: %p\n", loader);
			}
		}
	} else
		loader = getSystemClassLoader();

	class = findClassFromClassLoader((char*) name, loader);
	if (class == NULL) {
		log_warn(XDLOG, "           Cannot find the class: %s, loader: %p\n", name, loader);
	}
	return addJNILref(class);
}

jclass Jam_GetSuperClass(JNIEnv *env, jclass clazz) {
	ClassBlock *cb = CLASS_CB(REF_TO_OBJ(clazz));
	return IS_INTERFACE(cb) ? NULL : addJNILref(cb->super);
}

jboolean Jam_IsAssignableFrom(JNIEnv *env, jclass clazz1, jclass clazz2) {
	return isInstanceOf(REF_TO_OBJ(clazz2), REF_TO_OBJ(clazz1));
}

jint Jam_Throw(JNIEnv *env, jthrowable obj) {
	setException(REF_TO_OBJ(obj));
	return JNI_OK;
}

jint Jam_ThrowNew(JNIEnv *env, jclass clazz, const char *message) {
	signalExceptionClass(REF_TO_OBJ(clazz), (char*)message);
	return JNI_OK;
}

jthrowable Jam_ExceptionOccurred(JNIEnv *env) {
	return addJNILref(exceptionOccurred());
}

void Jam_ExceptionDescribe(JNIEnv *env) {
	printException();
}

void Jam_ExceptionClear(JNIEnv *env) {
	clearException();
}

void Jam_FatalError(JNIEnv *env, const char *message) {
	log_error(XDLOG, "JNI - FatalError: %s\n", message);
	exitVM(1);
}

jobject Jam_NewGlobalRef(JNIEnv *env, jobject obj) {
	return addJNIGref(REF_TO_OBJ_WEAK_NULL_CHECK(obj), GLOBAL_REF);
}

void Jam_DeleteGlobalRef(JNIEnv *env, jobject obj) {
	if (REF_TYPE(obj) == GLOBAL_REF)
		delJNIGref(REF_TO_OBJ(obj), GLOBAL_REF);
}

void Jam_DeleteLocalRef(JNIEnv *env, jobject obj) {
	delJNILref(obj);
}

jboolean Jam_IsSameObject(JNIEnv *env, jobject obj1, jobject obj2) {
	return REF_TO_OBJ_WEAK_NULL_CHECK(obj1) == REF_TO_OBJ_WEAK_NULL_CHECK(obj2);
}

/* JNI helper function.  The class may be invalid
 or it may not have been initialised yet */
Object *allocObjectClassCheck(Class *class) {
	ClassBlock *cb = CLASS_CB(class);

	/* Check the class can be instantiated */
	if (cb->access_flags & (ACC_INTERFACE | ACC_ABSTRACT)) {
		signalException(java_lang_InstantiationException, cb->name);
		return NULL;
	}

	/* Creating an instance of a class is an active use;
	 make sure it is initialised */
	if (initClass(class) == NULL)
		return NULL;

	return allocObject(class);
}

jobject Jam_AllocObject(JNIEnv *env, jclass clazz) {
	Object *obj = allocObjectClassCheck(REF_TO_OBJ(clazz));
	return addJNILref(obj);
}

jclass Jam_GetObjectClass(JNIEnv *env, jobject obj) {
	return addJNILref(REF_TO_OBJ(obj)->class);
}

jboolean Jam_IsInstanceOf(JNIEnv *env, jobject obj, jclass clazz) {
	return (obj == NULL) || isInstanceOf(REF_TO_OBJ(clazz),
			REF_TO_OBJ(obj)->class);
}

jmethodID getMethodID(JNIEnv *env, jclass clazz, const char *name,
		const char *sig, char is_static) {

	Class *class = initClass(REF_TO_OBJ(clazz));
	MethodBlock *mb = NULL;

	if (class != NULL) {
		if (!IS_PRIMITIVE(CLASS_CB(class))) {
			char *method_name = findUtf8((char*)name);
			char *method_sig = findUtf8((char*)sig);

			if (method_name != NULL && method_sig != NULL) {
				if (method_name == SYMBOL(object_init) || method_name
						== SYMBOL(class_init))
					mb = findMethod(class, method_name, method_sig);
				else
					mb = lookupMethod(class, method_name, method_sig);
			}
		}

		if (mb == NULL || ((mb->access_flags & ACC_STATIC) != 0) != is_static)
			signalException(java_lang_NoSuchMethodError, (char*)name);
	}

	return mb;
}

jmethodID Jam_GetMethodID(JNIEnv *env, jclass clazz, const char *name,
		const char *sig) {

	return getMethodID(env, clazz, name, sig, FALSE);
}

jfieldID Jam_GetFieldID(JNIEnv *env, jclass clazz, const char *name,
		const char *sig) {

	char *field_name = findUtf8((char*)name);
	char *field_sig = findUtf8((char*)sig);

	Class *class = initClass(REF_TO_OBJ(clazz));
	FieldBlock *fb = NULL;

	if (class != NULL) {
		if (field_name != NULL && field_sig != NULL)
			fb = lookupField(class, field_name, field_sig);

		if (fb == NULL)
			signalException(java_lang_NoSuchFieldError, field_name);
	}

	return fb;
}

jmethodID Jam_GetStaticMethodID(JNIEnv *env, jclass clazz, const char *name,
		const char *sig) {

	return getMethodID(env, clazz, name, sig, TRUE);
}

jfieldID Jam_GetStaticFieldID(JNIEnv *env, jclass clazz, const char *name,
		const char *sig) {

	char *field_name = findUtf8((char*)name);
	char *field_sig = findUtf8((char*)sig);

	Class *class = initClass(REF_TO_OBJ(clazz));
	FieldBlock *fb = NULL;

	if (class != NULL) {
		if (field_name != NULL && field_sig != NULL)
			fb = findField(class, field_name, field_sig);

		if (fb == NULL)
			signalException(java_lang_NoSuchFieldError, field_name);
	}

	return fb;
}

jstring Jam_NewString(JNIEnv *env, const jchar *unicodeChars, jsize len) {
	Object *str = createStringFromUnicode((unsigned short*) unicodeChars, len);
	return addJNILref(str);
}

jsize Jam_GetStringLength(JNIEnv *env, jstring string) {
	return getStringLen(REF_TO_OBJ(string));
}

const jchar *Jam_GetStringChars(JNIEnv *env, jstring string_ref,
		jboolean *isCopy) {

	Object *string = REF_TO_OBJ(string_ref);

	if (isCopy != NULL)
		*isCopy = JNI_FALSE;

	/* Pin the reference */
	addJNIGref(getStringCharsArray(string), GLOBAL_REF);

	return (const jchar *) getStringChars(string);
}

void Jam_ReleaseStringChars(JNIEnv *env, jstring string, const jchar *chars) {
	/* Unpin the reference */
	delJNIGref(getStringCharsArray(REF_TO_OBJ(string)), GLOBAL_REF);
}

jstring Jam_NewStringUTF(JNIEnv *env, const char *bytes) {
	return addJNILref(createString((char*) bytes));
}

jsize Jam_GetStringUTFLength(JNIEnv *env, jstring string) {
	if (string == NULL)
		return 0;
	return getStringUtf8Len(REF_TO_OBJ(string));
}

const char *Jam_GetStringUTFChars(JNIEnv *env, jstring string, jboolean *isCopy) {
	if (isCopy != NULL)
		*isCopy = JNI_TRUE;

	if (string == NULL)
		return NULL;
	return (const char*) String2Utf8(REF_TO_OBJ(string));
}

void Jam_ReleaseStringUTFChars(JNIEnv *env, jstring string, const char *chars) {
	sysFree((void*) chars);
}

jsize Jam_GetArrayLength(JNIEnv *env, jarray arr) {
	// XXX : http://git.berlios.de/cgi-bin/cgit.cgi/jamvm/commit/?id=5c1ee07b78ecf18b24493b1759e20a7e22570c19
#if 0
	log_info(XDLOG, "Jam_GetArrayLength(arr=%p)\n", arr);
	if (arr == NULL) {
		signalException(java_lang_NullPointerException, NULL);
		return 0;
	} else {
		Object *array = REF_TO_OBJ(arr);
		ClassBlock *cb = CLASS_CB(array->class);

		if (!IS_ARRAY(cb)) {
			signalException(java_lang_IllegalArgumentException, NULL);
			return 0;
		}

		return ARRAY_LEN(array);
	}
#else
	return ARRAY_LEN(REF_TO_OBJ(arr));
#endif
}

jobject Jam_NewObject(JNIEnv *env, jclass clazz, jmethodID methodID, ...) {
	Object *ob = allocObjectClassCheck(REF_TO_OBJ(clazz));

	if (ob != NULL) {
		va_list jargs;
		va_start(jargs, methodID);
		executeMethodVaList(ob, ob->class, methodID, jargs);
		va_end(jargs);
	}

	return addJNILref(ob);
}

jobject Jam_NewObjectA(JNIEnv *env, jclass clazz, jmethodID methodID,
		jvalue *args) {

	Object *ob = allocObjectClassCheck(REF_TO_OBJ(clazz));

	if (ob != NULL)
		executeMethodList(ob, ob->class, methodID, (u8*) args);

	return addJNILref(ob);
}

jobject Jam_NewObjectV(JNIEnv *env, jclass clazz, jmethodID methodID,
		va_list args) {

	Object *ob = allocObjectClassCheck(REF_TO_OBJ(clazz));

	if (ob != NULL)
		executeMethodVaList(ob, ob->class, methodID, args);

	return addJNILref(ob);
}

jarray Jam_NewObjectArray(JNIEnv *env, jsize length, jclass elementClass_ref,
		jobject initialElement_ref) {

	Object *initialElement = REF_TO_OBJ(initialElement_ref);
	Class *elementClass = REF_TO_OBJ(elementClass_ref);
	char *element_name = CLASS_CB(elementClass)->name;
	char ac_name[xi_strlen(element_name) + 4];
	Class *array_class;

	if (length < 0) {
		signalException(java_lang_NegativeArraySizeException, NULL);
		return NULL;
	}

	if (element_name[0] == '[')
		xi_strcat(xi_strcpy(ac_name, "["), element_name);
	else
		xi_strcat(xi_strcat(xi_strcpy(ac_name, "[L"), element_name), ";");

	array_class = findArrayClassFromClass(ac_name, elementClass);
	if (array_class != NULL) {
		Object *array = allocArray(array_class, length, sizeof(Object*));
		if (array != NULL) {
			if (initialElement != NULL) {
				Object **data = ARRAY_DATA(array, Object*);

				while (length--)
					*data++ = initialElement;
			}
			return addJNILref(array);
		}
	}
	return NULL;
}

jarray Jam_GetObjectArrayElement(JNIEnv *env, jobjectArray arr, jsize index) {
	// XXX : http://git.berlios.de/cgi-bin/cgit.cgi/jamvm/commit/?id=5c1ee07b78ecf18b24493b1759e20a7e22570c19
#if 0
	log_info(XDLOG, "Jam_GetObjectArrayElement(arr=%p)\n", arr);
	if (arr == NULL) {
		signalException(java_lang_NullPointerException, NULL);
		return NULL;
	} else {
		Object *array = REF_TO_OBJ(arr);
		ClassBlock *cb = CLASS_CB(array->class);

		if (!IS_ARRAY(cb)) {
			signalException(java_lang_IllegalArgumentException, NULL);
			return NULL;
		}

		if (index > ARRAY_LEN(array)) {
			signalException(java_lang_ArrayIndexOutOfBoundsException, NULL);
			return NULL;
		} else {
			u4 widened;
			int size = sigElement2Size(cb->name[1]);
			void *addr = &ARRAY_DATA(array, char)[index * size];
			Class *type = cb->dim > 1 ? cb->super : cb->element_class;

			if (size < sizeof(u4)) {
				widened = size == 1 ? *(signed char*) addr
				: cb->name[1] == 'S' ? *(signed short*) addr
				: *(unsigned short*) addr;
				addr = &widened;
			}

			return getReflectReturnObject(type, addr, REF_SRC_FIELD);
		}
	}
#else
	return addJNILref(ARRAY_DATA(REF_TO_OBJ(arr), Object*)[index]);
#endif
}

void Jam_SetObjectArrayElement(JNIEnv *env, jobjectArray array, jsize index,
		jobject value) {

	ARRAY_DATA(REF_TO_OBJ(array), Object*)[index] = value;
}

jint Jam_RegisterNatives(JNIEnv *env, jclass clazz,
		const JNINativeMethod *methods, jint nMethods) {
	return JNI_OK;
}

jint Jam_UnregisterNatives(JNIEnv *env, jclass clazz) {
	return JNI_OK;
}

jint Jam_MonitorEnter(JNIEnv *env, jobject obj) {
	objectLock(REF_TO_OBJ(obj));
	return JNI_OK;
}

jint Jam_MonitorExit(JNIEnv *env, jobject obj) {
	objectUnlock(REF_TO_OBJ(obj));
	return JNI_OK;
}

struct _JNIInvokeInterface Jam_JNIInvokeInterface;
JavaVM invokeIntf = &Jam_JNIInvokeInterface;

jint Jam_GetJavaVM(JNIEnv *env, JavaVM **vm) {
	*vm = &invokeIntf;
	return JNI_OK;
}

#define GET_FIELD(type, native_type)                                         \
native_type Jam_Get##type##Field(JNIEnv *env, jobject obj,                   \
                                 jfieldID fieldID) {                         \
    FieldBlock *fb = fieldID;                                                \
    Object *ob = REF_TO_OBJ(obj);                                            \
    return INST_DATA(ob, native_type, fb->u.offset);                         \
}

#define INT_GET_FIELD(type, native_type)                                     \
native_type Jam_Get##type##Field(JNIEnv *env, jobject obj,                   \
                                 jfieldID fieldID) {                         \
    FieldBlock *fb = fieldID;                                                \
    Object *ob = REF_TO_OBJ(obj);                                            \
    return (native_type)INST_DATA(ob, int, fb->u.offset);                    \
}

#define SET_FIELD(type, native_type)                                         \
void Jam_Set##type##Field(JNIEnv *env, jobject obj, jfieldID fieldID,        \
                          native_type value) {                               \
    FieldBlock *fb = fieldID;                                                \
    Object *ob = REF_TO_OBJ(obj);                                            \
    INST_DATA(ob, native_type, fb->u.offset) = value;                        \
}

#define INT_SET_FIELD(type, native_type)                                     \
void Jam_Set##type##Field(JNIEnv *env, jobject obj, jfieldID fieldID,        \
                          native_type value) {                               \
    FieldBlock *fb = fieldID;                                                \
    Object *ob = REF_TO_OBJ(obj);                                            \
    INST_DATA(ob, int, fb->u.offset) = (int)value;                           \
}

#define GET_STATIC_FIELD(type, native_type)                                  \
native_type Jam_GetStatic##type##Field(JNIEnv *env, jclass clazz,            \
                                       jfieldID fieldID) {                   \
    FieldBlock *fb = fieldID;                                                \
    return *(native_type *)fb->u.static_value.data;                          \
}

#define INT_GET_STATIC_FIELD(type, native_type)                              \
native_type Jam_GetStatic##type##Field(JNIEnv *env, jclass clazz,            \
                                       jfieldID fieldID) {                   \
    FieldBlock *fb = fieldID;                                                \
    return (native_type)fb->u.static_value.i;                                \
}

#define SET_STATIC_FIELD(type, native_type)                                  \
void Jam_SetStatic##type##Field(JNIEnv *env, jclass clazz, jfieldID fieldID, \
                                native_type value) {                         \
    FieldBlock *fb = fieldID;                                                \
    *(native_type *)fb->u.static_value.data = value;                         \
}

#define INT_SET_STATIC_FIELD(type, native_type)                              \
void Jam_SetStatic##type##Field(JNIEnv *env, jclass clazz, jfieldID fieldID, \
                native_type value) {                                         \
    FieldBlock *fb = fieldID;                                                \
    fb->u.static_value.i = (int)value;                                       \
}

#define FIELD_ACCESS(type, native_type)          \
        GET_FIELD(type, native_type);            \
        SET_FIELD(type, native_type);            \
        GET_STATIC_FIELD(type, native_type);     \
        SET_STATIC_FIELD(type, native_type);

#define INT_FIELD_ACCESS(type, native_type)      \
        INT_GET_FIELD(type, native_type);        \
        INT_SET_FIELD(type, native_type);        \
        INT_GET_STATIC_FIELD(type, native_type); \
        INT_SET_STATIC_FIELD(type, native_type);

INT_FIELD_ACCESS(Boolean, jboolean)
;
INT_FIELD_ACCESS(Byte, jbyte)
;
INT_FIELD_ACCESS(Char, jchar)
;
INT_FIELD_ACCESS(Short, jshort)
;
INT_FIELD_ACCESS(Int, jint)
;
FIELD_ACCESS(Long, jlong)
;
FIELD_ACCESS(Float, jfloat)
;
FIELD_ACCESS(Double, jdouble)
;

jobject Jam_GetObjectField(JNIEnv *env, jobject obj, jfieldID fieldID) {
	Object *ob = REF_TO_OBJ(obj);
	FieldBlock *fb = fieldID;

	return addJNILref(INST_DATA(ob, Object*, fb->u.offset));
}

void Jam_SetObjectField(JNIEnv *env, jobject obj, jfieldID fieldID,
		jobject value) {
	Object *ob = REF_TO_OBJ(obj);
	FieldBlock *fb = fieldID;

	INST_DATA(ob, jobject, fb->u.offset) = value;
}

jobject Jam_GetStaticObjectField(JNIEnv *env, jclass clazz, jfieldID fieldID) {
	FieldBlock *fb = fieldID;
	return addJNILref(fb->u.static_value.p);
}

void Jam_SetStaticObjectField(JNIEnv *env, jclass clazz, jfieldID fieldID,
		jobject value) {

	FieldBlock *fb = fieldID;
	fb->u.static_value.p = value;
}

#define VIRTUAL_METHOD(type, native_type)                                    \
native_type Jam_Call##type##Method(JNIEnv *env, jobject obj,                 \
                                   jmethodID mID, ...) {                     \
    Object *ob = REF_TO_OBJ(obj);                                            \
    native_type *ret;                                                        \
    va_list jargs;                                                           \
                                                                             \
    MethodBlock *mb = lookupVirtualMethod(ob, mID);                          \
    if(mb == NULL)                                                           \
        return (native_type) 0;                                              \
                                                                             \
    va_start(jargs, mID);                                                    \
    ret = (native_type*) executeMethodVaList(ob, ob->class, mb, jargs);      \
    va_end(jargs);                                                           \
                                                                             \
    return *ret;                                                             \
}                                                                            \
                                                                             \
native_type Jam_Call##type##MethodV(JNIEnv *env, jobject obj, jmethodID mID, \
                                    va_list jargs) {                         \
    Object *ob = REF_TO_OBJ(obj);                                            \
    MethodBlock *mb = lookupVirtualMethod(ob, mID);                          \
    if(mb == NULL)                                                           \
        return (native_type) 0;                                              \
    return *(native_type*)executeMethodVaList(ob, ob->class, mb, jargs);     \
}                                                                            \
                                                                             \
native_type Jam_Call##type##MethodA(JNIEnv *env, jobject obj, jmethodID mID, \
                                    jvalue *jargs) {                         \
    Object *ob = REF_TO_OBJ(obj);                                            \
    MethodBlock *mb = lookupVirtualMethod(ob, mID);                          \
    if(mb == NULL)                                                           \
        return (native_type) 0;                                              \
    return *(native_type*)executeMethodList(ob, ob->class, mb, (u8*)jargs);  \
}

#define NONVIRTUAL_METHOD(type, native_type)                                 \
native_type Jam_CallNonvirtual##type##Method(JNIEnv *env, jobject obj,       \
                                             jclass clazz, jmethodID mID,    \
                                             ...) {                          \
    native_type *ret;                                                        \
    va_list jargs;                                                           \
                                                                             \
    va_start(jargs, mID);                                                    \
    ret = (native_type*) executeMethodVaList(REF_TO_OBJ(obj),                \
                                             REF_TO_OBJ(clazz), mID, jargs); \
    va_end(jargs);                                                           \
                                                                             \
    return *ret;                                                             \
}                                                                            \
                                                                             \
native_type Jam_CallNonvirtual##type##MethodV(JNIEnv *env, jobject obj,      \
                                              jclass clazz, jmethodID mID,   \
                                              va_list jargs) {               \
    return *(native_type*)                                                   \
                  executeMethodVaList(REF_TO_OBJ(obj), REF_TO_OBJ(clazz),    \
                                      mID, jargs);                           \
}                                                                            \
                                                                             \
native_type Jam_CallNonvirtual##type##MethodA(JNIEnv *env, jobject obj,      \
                                              jclass clazz, jmethodID mID,   \
                                              jvalue *jargs) {               \
    return *(native_type*)                                                   \
                  executeMethodList(REF_TO_OBJ(obj), REF_TO_OBJ(clazz),      \
                                    mID, (u8*)jargs);                        \
}

#define STATIC_METHOD(type, native_type)                                     \
native_type Jam_CallStatic##type##Method(JNIEnv *env, jclass clazz,          \
                                         jmethodID methodID, ...) {          \
    native_type *ret;                                                        \
    va_list jargs;                                                           \
                                                                             \
    va_start(jargs, methodID);                                               \
    ret = (native_type*)                                                     \
             executeMethodVaList(NULL, REF_TO_OBJ(clazz), methodID, jargs);  \
    va_end(jargs);                                                           \
                                                                             \
    return *ret;                                                             \
}                                                                            \
                                                                             \
native_type Jam_CallStatic##type##MethodV(JNIEnv *env, jclass clazz,         \
                                          jmethodID mID, va_list jargs) {    \
    return *(native_type*)                                                   \
               executeMethodVaList(NULL, REF_TO_OBJ(clazz), mID, jargs);     \
}                                                                            \
                                                                             \
native_type Jam_CallStatic##type##MethodA(JNIEnv *env, jclass clazz,         \
                                          jmethodID mID, jvalue *jargs) {    \
    return *(native_type*)                                                   \
               executeMethodList(NULL, REF_TO_OBJ(clazz), mID, (u8*)jargs);  \
}

#define CALL_METHOD(access)         \
access##_METHOD(Boolean, jboolean); \
access##_METHOD(Byte, jbyte);       \
access##_METHOD(Char, jchar);       \
access##_METHOD(Short, jshort);     \
access##_METHOD(Int, jint);         \
access##_METHOD(Long, jlong);       \
access##_METHOD(Float, jfloat);     \
access##_METHOD(Double, jdouble);

CALL_METHOD(VIRTUAL)
;
CALL_METHOD(NONVIRTUAL)
;
CALL_METHOD(STATIC)
;

jobject Jam_CallObjectMethod(JNIEnv *env, jobject obj, jmethodID methodID, ...) {
	va_list jargs;
	Object **ret, *ob = REF_TO_OBJ(obj);
	MethodBlock *mb = lookupVirtualMethod(ob, methodID);

	if (mb == NULL)
		return NULL;

	va_start(jargs, methodID);
	ret = executeMethodVaList(ob, ob->class, mb, jargs);
	va_end(jargs);

	return addJNILref(*ret);
}

jobject Jam_CallObjectMethodV(JNIEnv *env, jobject obj, jmethodID methodID,
		va_list jargs) {

	Object **ret, *ob = REF_TO_OBJ(obj);
	MethodBlock *mb = lookupVirtualMethod(ob, methodID);

	if (mb == NULL)
		return NULL;

	ret = executeMethodVaList(ob, ob->class, mb, jargs);
	return addJNILref(*ret);
}

jobject Jam_CallObjectMethodA(JNIEnv *env, jobject obj, jmethodID methodID,
		jvalue *jargs) {

	Object **ret, *ob = REF_TO_OBJ(obj);
	MethodBlock *mb = lookupVirtualMethod(ob, methodID);

	if (mb == NULL)
		return NULL;

	ret = executeMethodList(ob, ob->class, mb, (u8*) jargs);
	return addJNILref(*ret);
}

jobject Jam_CallNonvirtualObjectMethod(JNIEnv *env, jobject obj, jclass clazz,
		jmethodID methodID, ...) {
	Object **ret;
	va_list jargs;

	va_start(jargs, methodID);
	ret = executeMethodVaList(REF_TO_OBJ(obj), REF_TO_OBJ(clazz), methodID,
			jargs);
	va_end(jargs);

	return addJNILref(*ret);
}

jobject Jam_CallNonvirtualObjectMethodV(JNIEnv *env, jobject obj, jclass clazz,
		jmethodID methodID, va_list jargs) {

	Object **ret = executeMethodVaList(REF_TO_OBJ(obj), REF_TO_OBJ(clazz),
			methodID, jargs);
	return addJNILref(*ret);
}

jobject Jam_CallNonvirtualObjectMethodA(JNIEnv *env, jobject obj, jclass clazz,
		jmethodID methodID, jvalue *jargs) {

	Object **ret = executeMethodList(REF_TO_OBJ(obj), REF_TO_OBJ(clazz),
			methodID, (u8*) jargs);
	return addJNILref(*ret);
}

jobject Jam_CallStaticObjectMethod(JNIEnv *env, jclass clazz,
		jmethodID methodID, ...) {
	Object **ret;
	va_list jargs;

	va_start(jargs, methodID);
	ret = executeMethodVaList(NULL, REF_TO_OBJ(clazz), methodID, jargs);
	va_end(jargs);

	return addJNILref(*ret);
}

jobject Jam_CallStaticObjectMethodV(JNIEnv *env, jclass clazz,
		jmethodID methodID, va_list jargs) {

	Object **ret =
			executeMethodVaList(NULL, REF_TO_OBJ(clazz), methodID, jargs);
	return addJNILref(*ret);
}

jobject Jam_CallStaticObjectMethodA(JNIEnv *env, jclass clazz,
		jmethodID methodID, jvalue *jargs) {

	Object **ret = executeMethodList(NULL, REF_TO_OBJ(clazz), methodID,
			(u8*) jargs);
	return addJNILref(*ret);
}

void Jam_CallVoidMethod(JNIEnv *env, jobject obj, jmethodID methodID, ...) {
	va_list jargs;
	MethodBlock *mb;
	Object *ob = REF_TO_OBJ(obj);

	va_start(jargs, methodID);
	if ((mb = lookupVirtualMethod(ob, methodID)) != NULL)
		executeMethodVaList(ob, ob->class, mb, jargs);
	va_end(jargs);
}

void Jam_CallVoidMethodV(JNIEnv *env, jobject obj, jmethodID methodID,
		va_list jargs) {
	MethodBlock *mb;
	Object *ob = REF_TO_OBJ(obj);

	if ((mb = lookupVirtualMethod(ob, methodID)) != NULL)
		executeMethodVaList(ob, ob->class, mb, jargs);
}

void Jam_CallVoidMethodA(JNIEnv *env, jobject obj, jmethodID methodID,
		jvalue *jargs) {
	MethodBlock *mb;
	Object *ob = REF_TO_OBJ(obj);

	if ((mb = lookupVirtualMethod(ob, methodID)) != NULL)
		executeMethodList(ob, ob->class, mb, (u8*) jargs);
}

void Jam_CallNonvirtualVoidMethod(JNIEnv *env, jobject obj, jclass clazz,
		jmethodID methodID, ...) {
	va_list jargs;

	va_start(jargs, methodID);
	executeMethodVaList(REF_TO_OBJ(obj), REF_TO_OBJ(clazz), methodID, jargs);
	va_end(jargs);
}

void Jam_CallNonvirtualVoidMethodV(JNIEnv *env, jobject obj, jclass clazz,
		jmethodID methodID, va_list jargs) {

	executeMethodVaList(REF_TO_OBJ(obj), REF_TO_OBJ(clazz), methodID, jargs);
}

void Jam_CallNonvirtualVoidMethodA(JNIEnv *env, jobject obj, jclass clazz,
		jmethodID methodID, jvalue *jargs) {

	executeMethodList(REF_TO_OBJ(obj), REF_TO_OBJ(clazz), methodID, (u8*) jargs);
}

void Jam_CallStaticVoidMethod(JNIEnv *env, jclass clazz, jmethodID methodID,
		...) {
	va_list jargs;

	va_start(jargs, methodID);
	executeMethodVaList(NULL, REF_TO_OBJ(clazz), methodID, jargs);
	va_end(jargs);
}

void Jam_CallStaticVoidMethodV(JNIEnv *env, jclass clazz, jmethodID methodID,
		va_list jargs) {

	executeMethodVaList(NULL, REF_TO_OBJ(clazz), methodID, jargs);
}

void Jam_CallStaticVoidMethodA(JNIEnv *env, jclass clazz, jmethodID methodID,
		jvalue *jargs) {

	executeMethodList(NULL, REF_TO_OBJ(clazz), methodID, (u8*) jargs);
}

#define NEW_PRIM_ARRAY(type, native_type, array_type)                        \
native_type##Array Jam_New##type##Array(JNIEnv *env, jsize length) {         \
    return (native_type##Array)                                              \
               addJNILref(allocTypeArray(array_type, length));               \
}

#define GET_PRIM_ARRAY_ELEMENTS(type, native_type)                           \
native_type *Jam_Get##type##ArrayElements(JNIEnv *env,                       \
                                          native_type##Array array_ref,      \
                                          jboolean *isCopy) {                \
    Object *array = REF_TO_OBJ(array_ref);                                   \
    if(isCopy != NULL)                                                       \
        *isCopy = JNI_FALSE;                                                 \
    addJNIGref(array, GLOBAL_REF);                                           \
    return ARRAY_DATA(array, native_type);                                   \
}

#define RELEASE_PRIM_ARRAY_ELEMENTS(type, native_type)                       \
void Jam_Release##type##ArrayElements(JNIEnv *env, native_type##Array array, \
                                      native_type *elems, jint mode) {       \
    delJNIGref(REF_TO_OBJ(array), GLOBAL_REF);                               \
}

#define GET_PRIM_ARRAY_REGION(type, native_type)                             \
void Jam_Get##type##ArrayRegion(JNIEnv *env, native_type##Array array,       \
                                jsize start, jsize len, native_type *buf) {  \
    xi_mem_copy(buf, ARRAY_DATA(REF_TO_OBJ(array), native_type) + start,          \
           len * sizeof(native_type));                                       \
}

#define SET_PRIM_ARRAY_REGION(type, native_type)                             \
void Jam_Set##type##ArrayRegion(JNIEnv *env, native_type##Array array,       \
                                jsize start, jsize len, native_type *buf) {  \
    xi_mem_copy(ARRAY_DATA(REF_TO_OBJ(array), native_type) + start, buf,          \
           len * sizeof(native_type));                                       \
}

#define PRIM_ARRAY_OP(type, native_type, array_type) \
    NEW_PRIM_ARRAY(type, native_type, array_type);   \
    GET_PRIM_ARRAY_ELEMENTS(type, native_type);      \
    RELEASE_PRIM_ARRAY_ELEMENTS(type, native_type);  \
    GET_PRIM_ARRAY_REGION(type, native_type);        \
    SET_PRIM_ARRAY_REGION(type, native_type);

PRIM_ARRAY_OP(Boolean, jboolean, T_BOOLEAN)
;
PRIM_ARRAY_OP(Byte, jbyte, T_BYTE)
;
PRIM_ARRAY_OP(Char, jchar, T_CHAR)
;
PRIM_ARRAY_OP(Short, jshort, T_SHORT)
;
PRIM_ARRAY_OP(Int, jint, T_INT)
;
PRIM_ARRAY_OP(Long, jlong, T_LONG)
;
PRIM_ARRAY_OP(Float, jfloat, T_FLOAT)
;
PRIM_ARRAY_OP(Double, jdouble, T_DOUBLE)
;

#define METHOD(type, ret_type)                \
    Jam_Call##type##ret_type##Method,         \
    Jam_Call##type##ret_type##MethodV,        \
    Jam_Call##type##ret_type##MethodA

#define METHODS(type)                         \
    METHOD(type, Object),                     \
    METHOD(type, Boolean),                    \
    METHOD(type, Byte),                       \
    METHOD(type, Char),                       \
    METHOD(type, Short),                      \
    METHOD(type, Int),                        \
    METHOD(type, Long),                       \
    METHOD(type, Float),                      \
    METHOD(type, Double),                     \
    METHOD(type, Void)

#define FIELD(direction, type, field_type)    \
    Jam_##direction##type##field_type##Field

#define FIELDS2(direction, type)              \
        FIELD(direction, type,  Object),      \
        FIELD(direction, type, Boolean),      \
        FIELD(direction, type,  Byte),        \
        FIELD(direction, type, Char),         \
        FIELD(direction, type, Short),        \
        FIELD(direction, type, Int),          \
        FIELD(direction, type, Long),         \
        FIELD(direction, type, Float),        \
        FIELD(direction, type, Double)

#define FIELDS(type)                          \
        FIELDS2(Get, type),                   \
        FIELDS2(Set, type)

#define ARRAY(op, el_type, type)              \
        Jam_##op##el_type##Array##type

#define ARRAY_OPS(op, type)                   \
        ARRAY(op, Boolean, type),             \
        ARRAY(op, Byte, type),                \
        ARRAY(op, Char, type),                \
        ARRAY(op, Short, type),               \
        ARRAY(op, Int, type),                 \
        ARRAY(op, Long, type),                \
        ARRAY(op, Float, type),               \
        ARRAY(op, Double, type)

struct _JNINativeInterface Jam_JNINativeInterface = { NULL, NULL, NULL, NULL,
		Jam_GetVersion, Jam_DefineClass, Jam_FindClass,
		Jam_FromReflectedMethod, Jam_FromReflectedField, Jam_ToReflectedMethod,
		Jam_GetSuperClass, Jam_IsAssignableFrom, Jam_ToReflectedField,
		Jam_Throw, Jam_ThrowNew, Jam_ExceptionOccurred, Jam_ExceptionDescribe,
		Jam_ExceptionClear, Jam_FatalError, Jam_PushLocalFrame,
		Jam_PopLocalFrame, Jam_NewGlobalRef, Jam_DeleteGlobalRef,
		Jam_DeleteLocalRef, Jam_IsSameObject, Jam_NewLocalRef,
		Jam_EnsureLocalCapacity, Jam_AllocObject, Jam_NewObject,
		Jam_NewObjectV, Jam_NewObjectA, Jam_GetObjectClass, Jam_IsInstanceOf,
		Jam_GetMethodID, METHODS(/*virtual*/),
    METHODS(Nonvirtual),
    Jam_GetFieldID,
    FIELDS(/*instance*/),
    Jam_GetStaticMethodID,
    METHODS(Static),
    Jam_GetStaticFieldID,
    FIELDS(Static),
    Jam_NewString,
    Jam_GetStringLength,
    Jam_GetStringChars,
    Jam_ReleaseStringChars,
    Jam_NewStringUTF,
    Jam_GetStringUTFLength,
    Jam_GetStringUTFChars,
    Jam_ReleaseStringUTFChars,
    Jam_GetArrayLength,


























    ARRAY(New, Object,)																																																				,
    ARRAY(Get, Object, Element),
    ARRAY(Set, Object, Element),
    ARRAY_OPS(New,),
    ARRAY_OPS(Get, Elements),
    ARRAY_OPS(Release, Elements),
    ARRAY_OPS(Get, Region),
    ARRAY_OPS(Set, Region),
    Jam_RegisterNatives,
    Jam_UnregisterNatives,
    Jam_MonitorEnter,
    Jam_MonitorExit,
    Jam_GetJavaVM,
    Jam_GetStringRegion,
    Jam_GetStringUTFRegion,
    Jam_GetPrimitiveArrayCritical,
    Jam_ReleasePrimitiveArrayCritical,
    Jam_GetStringCritical,
    Jam_ReleaseStringCritical,
    Jam_NewWeakGlobalRef,
    Jam_DeleteWeakGlobalRef,
    Jam_ExceptionCheck,
    Jam_NewDirectByteBuffer,
    Jam_GetDirectBufferAddress,
    Jam_GetDirectBufferCapacity,  Jam_GetObjectRefType
};

jint Jam_DestroyJavaVM(JavaVM *vm) {
	mainThreadWaitToExitVM();
	exitVM(0);

	return JNI_OK;
}

static void *env = &Jam_JNINativeInterface;

static jint attachCurrentThread(void **penv, void *args, int is_daemon) {
	if (threadSelf() == NULL) {
		char *name = NULL;
		Object *group = NULL;

		if (args != NULL) {
			JavaVMAttachArgs *attach_args = (JavaVMAttachArgs*) args;
			if (attach_args->version != JNI_VERSION_1_6 && attach_args->version
					!= JNI_VERSION_1_4 && attach_args->version
					!= JNI_VERSION_1_2)
				return JNI_EVERSION;

			name = attach_args->name;
			group = attach_args->group;
		}

		if (attachJNIThread(name, is_daemon, group) == NULL)
			return JNI_ERR;

		initJNILrefs();
	}

	*penv = &env;
	return JNI_OK;
}

jint Jam_AttachCurrentThread(JavaVM *vm, void **penv, void *args) {
	return attachCurrentThread(penv, args, FALSE);
}

jint Jam_AttachCurrentThreadAsDaemon(JavaVM *vm, void **penv, void *args) {
	return attachCurrentThread(penv, args, TRUE);
}

jint Jam_DetachCurrentThread(JavaVM *vm) {
	Thread *thread = threadSelf();

	if (thread == NULL) {
		return JNI_EDETACHED;
	}

	detachJNIThread(thread);
	return JNI_OK;
}

jint Jam_GetEnv(JavaVM *vm, void **penv, jint version) {
	if ((version != JNI_VERSION_1_6) && (version != JNI_VERSION_1_4)
			&& (version != JNI_VERSION_1_2) && (version != JNI_VERSION_1_1)) {
		*penv = NULL;
		return JNI_EVERSION;
	}

	if (threadSelf() == NULL) {
		*penv = NULL;
		return JNI_EDETACHED;
	}

	*penv = &env;
	return JNI_OK;
}

struct _JNIInvokeInterface Jam_JNIInvokeInterface = { NULL, NULL, NULL,
		Jam_DestroyJavaVM, Jam_AttachCurrentThread, Jam_DetachCurrentThread,
		Jam_GetEnv, Jam_AttachCurrentThreadAsDaemon, };

jint JNI_GetDefaultJavaVMInitArgs(void *args) {
	JavaVMInitArgs *vm_args = (JavaVMInitArgs*) args;

	if (vm_args->version != JNI_VERSION_1_6 && vm_args->version
			!= JNI_VERSION_1_4 && vm_args->version != JNI_VERSION_1_2)
		return JNI_EVERSION;

	return JNI_OK;
}

jint parseInitOptions(JavaVMInitArgs *vm_args, InitArgs *args) {
	Property props[vm_args->nOptions];
	int props_count = 0;
	int i;

	for (i = 0; i < vm_args->nOptions; i++) {
		char *string = vm_args->options[i].optionString;

		if (xi_strcmp(string, "vfprintf") == 0)
			args->vfprintf = vm_args->options[i].extraInfo;

		else if (xi_strcmp(string, "exit") == 0)
			args->exit = vm_args->options[i].extraInfo;

		else if (xi_strcmp(string, "abort") == 0)
			args->abort = vm_args->options[i].extraInfo;

		else if (xi_strncmp(string, "-verbose:", 9) == 0) {
			char *type = &string[8];

			do {
				type++;

				if (xi_strncmp(type, "class", 5) == 0) {
					args->verboseclass = TRUE;
					type += 5;
				} else if (xi_strncmp(type, "gc", 2) == 0) {
					args->verbosegc = TRUE;
					type += 2;
				} else if (xi_strncmp(type, "jni", 3) == 0) {
					args->verbosedll = TRUE;
					type += 3;
				}
			} while (*type == ',');

		} else if (xi_strcmp(string, "-Xasyncgc") == 0)
			args->asyncgc = TRUE;

		else if (xi_strncmp(string, "-Xms", 4) == 0) {
			args->min_heap = parseMemValue(string + 4);
			if (args->min_heap < MIN_HEAP)
				goto error;

		} else if (xi_strncmp(string, "-Xmx", 4) == 0) {
			args->max_heap = parseMemValue(string + 4);
			if (args->max_heap < MIN_HEAP)
				goto error;

		} else if (xi_strncmp(string, "-Xss", 4) == 0) {
			args->java_stack = parseMemValue(string + 4);
			if (args->java_stack < MIN_STACK)
				goto error;

		} else if (xi_strncmp(string, "-D", 2) == 0) {
			char *pntr;
			char *key = xi_strcpy(sysMalloc(xi_strlen(string + 2) + 1),
					string + 2);

			for (pntr = key; *pntr && (*pntr != '='); pntr++) {
				// Do nothing
			}
			if (pntr == key) {
				goto error;
			}

			*pntr++ = '\0';
			props[props_count].key = key;
			props[props_count++].value = pntr;

		} else if (xi_strncmp(string, "-Xbootclasspath:", 16) == 0) {

			args->bootpathopt = '\0';
			args->bootpath = string + 16;

		} else if (xi_strncmp(string, "-Xbootclasspath/a:", 18) == 0
				|| xi_strncmp(string, "-Xbootclasspath/p:", 18) == 0
				|| xi_strncmp(string, "-Xbootclasspath/c:", 18) == 0
				|| xi_strncmp(string, "-Xbootclasspath/v:", 18) == 0) {

			args->bootpathopt = string[16];
			args->bootpath = string + 18;

		} else if (xi_strcmp(string, "-Xnocompact") == 0) {
			args->compact_specified = TRUE;
			args->do_compact = FALSE;

		} else if (xi_strcmp(string, "-Xcompactalways") == 0) {
			args->compact_specified = args->do_compact = TRUE;
		} else if (!vm_args->ignoreUnrecognized)
			goto error;
	}

	if (args->min_heap > args->max_heap)
		goto error;

	if ((args->props_count = props_count)) {
		args->commandline_props = sysMalloc(props_count * sizeof(Property));
		xi_mem_copy(args->commandline_props, &props[0],
				props_count * sizeof(Property));
	}

	return JNI_OK;

	error: // ERROR GOTO LABEL

	return JNI_ERR;
}

jint JNI_CreateJavaVM(JavaVM **pvm, void **penv, void *args) {
	JavaVMInitArgs *vm_args = (JavaVMInitArgs*) args;
	InitArgs init_args;

	if (vm_args->version != JNI_VERSION_1_6 && vm_args->version
			!= JNI_VERSION_1_4 && vm_args->version != JNI_VERSION_1_2)
		return JNI_EVERSION;

	log_debug(XDLOG, "***********************************************************\n");
	log_debug(XDLOG, "******************     CreateJavaVM        ****************\n");
	log_debug(XDLOG, "***********************************************************\n");

	setDefaultInitArgs(&init_args);

	if (parseInitOptions(vm_args, &init_args) == JNI_ERR)
		return JNI_ERR;

	init_args.main_stack_base = nativeStackBase();
	initVM(&init_args);
	initJNILrefs();

	*penv = &env;
	*pvm = &invokeIntf;

	return JNI_OK;
}

jint JNI_GetCreatedJavaVMs(JavaVM **buff, jsize buff_len, jsize *num) {
	if (buff_len > 0) {
		*buff = &invokeIntf;
		*num = 1;
		return JNI_OK;
	}
	return JNI_ERR;
}

void *getJNIInterface() {
	return &Jam_JNINativeInterface;
}
