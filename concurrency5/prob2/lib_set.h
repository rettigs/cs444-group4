/*
 * set.h
 *
 *  Created on: Dec 1, 2009
 *      Author: petergoodman
 *     Version: $Id$
 */

#ifndef LIB_SET_H_
#define LIB_SET_H_

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "lib_sem.h"

typedef struct set *set_t;

set_t set_alloc(const int num_slots);
void set_exit_free(set_t set);
void set_free(set_t set);
void set_insert(set_t set, const int item);
int set_take(set_t set);
int set_cardinality(const set_t set);

#endif /* LIB_SET_H_ */
