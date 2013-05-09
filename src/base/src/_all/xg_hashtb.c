/*
 * Copyright 2013 Cheolmin Jo (webos21@gmail.com)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * File : xg_hashtb.c
 */

#include "xi/xi_hashtb.h"

#include "xi/xi_mem.h"
#include "xi/xi_clock.h"
#include "xi/xi_log.h"

// ----------------------------------------------
// Inner Structures
// ----------------------------------------------

// XXX : tunable == 2^n - 1
#define XG_HASHTB_INIT_MAX 15

typedef struct _xg_hashtb_entry {
	struct _xg_hashtb_entry *next;
	xuint32 hashid;
	const xvoid *key;
	xint32 klen;
	const xvoid *val;
} xg_hashtb_entry_t;

struct _xi_hashtb_idx {
	xi_hashtb_t *ht;
	xg_hashtb_entry_t *curr;
	xg_hashtb_entry_t *next;
	xuint32 index;
};

struct _xi_hashtb {
	xg_hashtb_entry_t **array;
	xi_hashtb_idx_t iterator; // For xi_hash_first(NULL, ...)
	xuint32 count;
	xuint32 max;
	xuint32 seed;
	xi_hashtb_keygen hash_func;
	xg_hashtb_entry_t *free; // List of recycled entries
};

// ----------------------------------------------
// Global Variables
// ----------------------------------------------

// ----------------------------------------------
// Part Internal Functions
// ----------------------------------------------

static xg_hashtb_entry_t **xg_hashtb_alloc_array(xi_hashtb_t *htb, xuint32 max) {
	return xi_mem_calloc(sizeof(*htb->array), (max + 1));
}

static void xg_hashtb_expand_array(xi_hashtb_t *htb) {
	xi_hashtb_idx_t *hi;
	xg_hashtb_entry_t **new_array;
	xuint32 new_max;

	new_max = htb->max * 2 + 1;
	new_array = xg_hashtb_alloc_array(htb, new_max);
	for (hi = xi_hashtb_first(htb); hi; hi = xi_hashtb_next(hi)) {
		xuint32 i = hi->curr->hashid & new_max;
		hi->curr->next = new_array[i];
		new_array[i] = hi->curr;
	}
	xi_mem_free(htb->array);
	htb->array = new_array;
	htb->max = new_max;
}

static xuint32 xg_hashtb_keygen_default(const xvoid *char_key, xint32 *klen,
		xuint32 hash) {
	const xuint8 *key = (const xuint8 *) char_key;
	const xuint8 *p;
	xint32 i;

	/*
	 * This is the popular `times 33' hash algorithm which is used by
	 * perl and also appears in Berkeley DB. This is one of the best
	 * known hash functions for strings because it is both computed
	 * very fast and distributes very well.
	 *                  -- Ralf S. Engelschall <rse@engelschall.com>
	 */

	if (XI_HASHTB_KEY_STRING == (*klen)) {
		for (p = key; *p; p++) {
			hash = hash * 33 + (xint32)(*p);
		}
		*klen = (xint32)(p - key);
	} else {
		for (p = key, i = *klen; i; i--, p++) {
			hash = hash * 33 + (xuint32)(*p);
		}
	}

	return hash;
}

static xg_hashtb_entry_t **xg_hashtb_find_entry(xi_hashtb_t *htb,
		const xvoid *key, xint32 klen, const xvoid *val) {
	xg_hashtb_entry_t **hep, *he;
	xuint32 hashid;

	if (htb->hash_func) {
		hashid = htb->hash_func(key, &klen);
	} else {
		hashid = xg_hashtb_keygen_default(key, &klen, htb->seed);
	}

	// scan linked list
	for (hep = &htb->array[hashid & htb->max], he = *hep; he; hep = &he->next, he
			= *hep) {
		if (he->hashid == hashid && he->klen == klen && xi_mem_cmp(he->key,
				key, (xsize)klen) == 0) {
			break;
		}
	}
	if (he || !val) {
		return hep;
	}

	// add a new entry for non-NULL values
	if ((he = htb->free) != NULL) {
		htb->free = he->next;
	} else {
		he = xi_mem_alloc(sizeof(*he));
	}
	he->next = NULL;
	he->hashid = hashid;
	he->key = key;
	he->klen = klen;
	he->val = val;
	*hep = he;
	htb->count++;
	return hep;
}

// ----------------------------------------------
// XI Functions
// ----------------------------------------------

xi_hashtb_t *xi_hashtb_create() {
	xi_hashtb_t *htb;
	xuint64 now = (xuint64) xi_clock_msec();

	htb = xi_mem_alloc(sizeof(xi_hashtb_t));
	htb->free = NULL;
	htb->count = 0;
	htb->max = XG_HASHTB_INIT_MAX;
	htb->seed
			= (xuint32) (((xuint32)(now >> 32)) ^ now ^ ((xuintptr) htb) ^ ((xuintptr) &now))
					- 1;
	htb->array = xg_hashtb_alloc_array(htb, htb->max);
	htb->hash_func = NULL;

	return htb;
}

xi_hashtb_t *xi_hashtb_create_custom(xi_hashtb_keygen func) {
	xi_hashtb_t *htb = xi_hashtb_create();
	htb->hash_func = func;
	return htb;
}

xvoid xi_hashtb_set(xi_hashtb_t *htb, const xvoid *key, xint32 klen,
		const xvoid *val) {
	xg_hashtb_entry_t **hep;
	hep = xg_hashtb_find_entry(htb, key, klen, val);
	if (*hep) {
		if (val == NULL) {
			// delete entry
			xg_hashtb_entry_t *old = *hep;
			*hep = (*hep)->next;
			old->next = htb->free;
			htb->free = old;
			htb->count--;
		} else {
			// replace entry
			(*hep)->val = val;
			// check that the collision rate isn't too high
			if (htb->count > htb->max) {
				xg_hashtb_expand_array(htb);
			}
		}
	}
	/* else key not present and val==NULL */
}

xvoid *xi_hashtb_get(xi_hashtb_t *htb, const xvoid *key, xint32 klen) {
	xg_hashtb_entry_t *he;
	he = *xg_hashtb_find_entry(htb, key, klen, NULL);
	if (he) {
		return (xvoid *) he->val;
	} else {
		return NULL;
	}
}

xuint32 xi_hashtb_count(xi_hashtb_t *htb) {
	return htb->count;
}

xuint32 xi_hashtb_max(xi_hashtb_t *htb) {
	return htb->max;
}

xi_hashtb_idx_t *xi_hashtb_first(xi_hashtb_t *htb) {
	xi_hashtb_idx_t *hi;

	hi = &htb->iterator;
	hi->ht = htb;
	hi->index = 0;
	hi->curr = NULL;
	hi->next = NULL;
	return xi_hashtb_next(hi);
}

xi_hashtb_idx_t *xi_hashtb_next(xi_hashtb_idx_t *hidx) {
	hidx->curr = hidx->next;
	while (!hidx->curr) {
		if (hidx->index > hidx->ht->max) {
			return NULL;
		}
		hidx->curr = hidx->ht->array[hidx->index++];
	}
	hidx->next = hidx->curr->next;
	return hidx;
}

xvoid xi_hashtb_this(xi_hashtb_idx_t *hidx, const xvoid **key, xint32 *klen,
		xvoid **val) {
	if (key) {
		*key = hidx->curr->key;
	}
	if (klen) {
		*klen = hidx->curr->klen;
	}
	if (val) {
		*val = (xvoid *) hidx->curr->val;
	}
}

xvoid xi_hashtb_clear(xi_hashtb_t *htb) {
	xi_hashtb_idx_t *hidx;
	for (hidx = xi_hashtb_first(htb); hidx; hidx = xi_hashtb_next(hidx)) {
		xi_hashtb_set(htb, hidx->curr->key, hidx->curr->klen, NULL);
	}
}

xi_hashtb_t *xi_hashtb_clone(xi_hashtb_t *htb) {
	xi_hashtb_t *newtb;
	xg_hashtb_entry_t *new_vals;
	xuint32 i, j;

	newtb = xi_mem_alloc(
			sizeof(xi_hashtb_t) + sizeof(*newtb->array) * (htb->max + 1)
					+ sizeof(xg_hashtb_entry_t) * htb->count);
	newtb->free = NULL;
	newtb->count = htb->count;
	newtb->max = htb->max;
	newtb->seed = htb->seed;
	newtb->hash_func = htb->hash_func;
	newtb->array
			= (xg_hashtb_entry_t **) ((char *) newtb + sizeof(xi_hashtb_t));

	new_vals = (xg_hashtb_entry_t *) ((char *) (newtb) + sizeof(xi_hashtb_t)
			+ sizeof(*newtb->array) * (htb->max + 1));
	for (i = 0, j = 0; i <= newtb->max; i++) {
		xg_hashtb_entry_t **new_entry = &(newtb->array[i]);
		xg_hashtb_entry_t *orig_entry = htb->array[i];
		while (orig_entry) {
			*new_entry = &new_vals[j++];
			(*new_entry)->hashid = orig_entry->hashid;
			(*new_entry)->key = orig_entry->key;
			(*new_entry)->klen = orig_entry->klen;
			(*new_entry)->val = orig_entry->val;
			new_entry = &((*new_entry)->next);
			orig_entry = orig_entry->next;
		}
		*new_entry = NULL;
	}
	return newtb;
}

xvoid xi_hashtb_destroy(xi_hashtb_t *ht) {
	if (ht) {
		if (ht->array) {
			xi_mem_free(ht->array);
		}
		if (ht->free) {
			xi_mem_free(ht->free);
		}
	}
}
