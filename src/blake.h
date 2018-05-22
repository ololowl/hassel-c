#pragma once

#include "hs.h"

// 0 if no merging was performed with new elem
// >0 if original elem or merging had place
struct new_elems {
	int *list;
	bool *is_orig;
	int alloc, used;
};

void add_info(struct new_elems *info);
void info_free(struct new_elems *info);
void info_elem_free(struct new_elems *info, int i);

void my_print_hs(struct hs *h);

bool make_new_elem (struct hs *h, int i, int j, int part, array_t mask1);

int merge(struct hs *h, int i, int j, struct new_elems *info);
bool check_diffs(struct hs *h, int big_idx, int small_idx);
int merge_check_diffs(struct hs *h, int i, int j, struct new_elems *info);

void blake_hs_vec (struct hs *h);