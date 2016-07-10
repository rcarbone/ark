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

#include <stdlib.h>
#include <math.h>

#include "hash.h"

/* PRIMES: Simple prime number functions */
inline
unsigned int find_prime (unsigned long max)
{
  unsigned int i = 0;
  unsigned int f = 0;
  unsigned int prime = 1;

  for (i = max ; i > 1 ; i--) {
    if ((i % 2) == 0)
      goto NOTPRIME;
    for (f = 2 ; f <= ((unsigned int) floor (sqrt ((double) i))) ; f++) {
      if ((i % f) == 0)
	goto NOTPRIME;
    }
    prime = i;
    goto EXIT;
  NOTPRIME:
    continue;
  }

 EXIT:
  return prime;
}


unsigned long hash_pjw (char *value)
{
  unsigned long h = 0;
  unsigned long g = 0;

  while (*value) {
    h = (h << 4) + *(value++);
    if ((g = h & 0xF0000000))
      h ^= g >> 24;
    h &= ~g;
  }
  return h;
}

void hash_table_init (struct hash_table *h)
{
  int i = 0;

  if (!h)
    return;

  if (h->size < 1)
    h->size = 100;
  if (!h->func)
    h->func = hash_pjw;

  h->size = (unsigned int) find_prime (h->size);
  h->tbl = (struct hlist *) malloc (h->size * (sizeof (struct hlist)));
  if (!h->tbl)
    return;
  memset (h->tbl, 0, (h->size * sizeof (struct hlist)));

  for (i = 0 ; i < h->size ; i++) {
    list_init (&(h->tbl[i]));
  }

  return;
}

void hash_table_free (struct hash_table *h)
{
  int i = 0;
  struct hdatum *d = NULL;
  struct hlist_item *curr = NULL;

  if (!h)
    return;

  for (i = 0 ; i < h->size ; i++) {
    for (curr = h->tbl[i].head ; curr ; curr = curr->next) {
      if (curr->data) {
	d = (struct hdatum *) curr->data;
	if (d->key)
	  free (d->key);
	if (d->val)
	  free (d->val);
      }
    }
    list_free (&(h->tbl[i]));
  }

  if (h->tbl)
    free (h->tbl);

  h->func = NULL;
  h->size = 0;
  h->tbl = NULL;

  return;
}

struct hdatum *hash_table_insert (struct hash_table *h, struct hdatum *d)
{
  int slot = 0;  
  struct hdatum *hnew = NULL;
  struct hlist_item *item = NULL;

  if (!(d && h && (h->size > 0) && h->func && h->tbl))
    return NULL;

  hnew = (struct hdatum *) malloc (sizeof (struct hdatum));
  if (!hnew)
    goto ERROR;
  memset (hnew, 0, (sizeof (struct hdatum)));

  hnew->ksize = d->ksize;
  hnew->key = (void *) malloc (hnew->ksize + 1);
  if (!hnew->key)
    goto ERROR;
  memset (hnew->key, 0, (hnew->ksize + 1));
  memcpy (hnew->key, d->key, hnew->ksize);

  hnew->vsize = d->vsize;
  hnew->val = (void *) malloc (hnew->vsize + 1);
  if (!hnew->val)
    goto ERROR;
  memset (hnew->val, 0, (hnew->vsize + 1));
  memcpy (hnew->val, d->val, hnew->vsize);

  slot = (int) (h->func ((char *) d->key) % h->size);
  item = list_insert (&(h->tbl[slot]), (void *) hnew, sizeof (struct hdatum));
  goto EXIT;

 ERROR:
  if (hnew && (hnew->key))
    free (hnew->key);
  if (hnew && (hnew->val))
    free (hnew->val);
EXIT:
  if (hnew)
    free (hnew);
  return (struct hdatum *) item->data;
}

struct hdatum *hash_table_search (struct hash_table *h, struct hdatum *k)
{
  int slot = 0;
  struct hlist_item *curr = NULL;
  struct hdatum *d = NULL;

  if (!(k && h && (h->size > 0) && h->func && h->tbl))
    goto EXIT;

  slot = (int) (h->func ((char *) k->key) % h->size);
  for (curr = h->tbl[slot].head ; curr ; curr = curr->next) {
    d = (struct hdatum *) curr->data;
    if (d->ksize == k->ksize){
      if (!memcmp (d->key, k->key, d->ksize))
	goto EXIT;
    }
  }
  d = NULL;

 EXIT:
  return d;
}

static struct hlist_item *hash_table_search2 (struct hash_table *h, struct hdatum *k)
{
  int slot = 0;
  struct hlist_item *curr = NULL;
  struct hdatum *d = NULL;
  
  if (!(k && h && (h->size > 0) && h->func && h->tbl))
    goto EXIT;

  slot = (int) (h->func ((char *) k->key) % h->size);
  for (curr = h->tbl[slot].head ; curr ; curr = curr->next) {
    d = (struct hdatum *) curr->data;
    if (d->ksize == k->ksize){
      if (!memcmp (d->key, k->key, d->ksize))
	goto EXIT;
    }
  }
  curr = NULL;

 EXIT:
  return curr;
}

void hash_table_delete (struct hash_table *h, struct hdatum *k)
{
  struct hdatum *d = NULL;
  struct hlist_item *l = NULL;

  if (!(k && h && (h->size > 0) && h->func && h->tbl))
    return;

  if ((l = hash_table_search2 (h, k))) {
    d = (struct hdatum *) l->data;
    if (d->key)
      free (d->key);
    if (d->val)
      free (d->val);
    list_delete ((struct hlist_item *) l);
  }

  return;
}

