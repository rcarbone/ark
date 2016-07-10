/*
** HASH: Simple hash table implementation.
** Copyright (C) 2000 Michael W. Shaffer <mwshaffer@yahoo.com>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.  
**
** You should have received a copy of the GNU General Public License
** along with this program (see the file COPYING). If not, write to:
**
** The Free Software Foundation, Inc.
** 59 Temple Place, Suite 330,
** Boston, MA  02111-1307  USA
*/

#ifndef __HASH_H__
#define __HASH_H__

#include "list.h"

typedef unsigned long (*hash_func)(char *value);

struct hdatum {
	void *key;
	unsigned long ksize;
	void *val;
	unsigned long vsize;
};

struct hash_table {
	hash_func func;
	unsigned int size;
	struct hlist *tbl;
};

/* Peter J. Wienberger's hash */
unsigned long hash_pjw (char *value);

void hash_table_init (struct hash_table *h);
void hash_table_free (struct hash_table *h);
struct hdatum *hash_table_insert (struct hash_table *h, struct hdatum *d); 
struct hdatum *hash_table_search (struct hash_table *h, struct hdatum *k);
void hash_table_delete (struct hash_table *h, struct hdatum *k);

#endif /* __HASH_H__ */

