/*
** LIST: Simple doubly-linked list implementation.
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
#include <string.h>
#include "list.h"

void list_init (struct hlist *list)
{
	list->head = NULL;
	return;
}

struct hlist_item *list_insert (struct hlist *list, void *data, long size)
{
	struct hlist_item *hnew = NULL;

	if (!(list && data && (size > 0)))
		goto ERROR;

	hnew = (struct hlist_item *) malloc (sizeof (struct hlist_item));
	if (!hnew)
		goto ERROR;
	memset (hnew, 0, sizeof (struct hlist_item));

	hnew->size = size;
	hnew->data = (void *) malloc (size + 1);
	if (!hnew->data)
		goto ERROR;
	memset (hnew->data, 0, (size + 1));
	memcpy (hnew->data, data, size);

	if (list->head)
		list->head->prev = hnew;
	hnew->next = list->head;
	list->head = hnew;

	hnew->list = list;

	goto EXIT;

ERROR:
	if (hnew && hnew->data)
		free (hnew->data);
	if (hnew)
		free (hnew);
	hnew = NULL;
EXIT:
	return hnew;
}

struct hlist_item *list_search (struct hlist *list, void *data, long size)
{
	struct hlist_item *curr = NULL;

	if (!(list && data && (size > 0)))
		return NULL;

	for (curr = list->head ; curr ; curr = curr->next) {
		if (curr->size == size) {
			if (!memcmp (curr->data, data, size))
				return curr;
		}
	}
	return NULL;
}

void list_delete (struct hlist_item *item)
{
	if (!(item && item->list))
		return;

	if (item->next) {
		item->next->prev = item->prev;
	} else {
		if (item->list->head == item)
			item->list->head = NULL;
	}

	if (item->prev) {
		item->prev->next = item->next;
	} else {
		item->list->head = item->next;
	}

	free (item->data);
	free (item);

	return;
}

void list_free (struct hlist *list)
{
	struct hlist_item *next = NULL;
	struct hlist_item *curr = NULL;

	if (!list)
		return;

	if (!list->head) 
		goto EXIT;

	for (curr = list->head ; curr ; curr = next) {
		if (curr->data)
			free (curr->data);
		next = curr->next;
		free (curr);
	}

EXIT:
	list->head = NULL;
	return;
}

