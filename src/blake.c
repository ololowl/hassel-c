#include "blake.h"

#ifndef ODD_MASK
#define ODD_MASK  ( (array_t) 0x5555555555555555ull )
#endif

#ifndef SIZE
#define SIZE(L) ( DIV_ROUND_UP (2 * (L), sizeof (array_t)) )
#endif

void add_info(struct new_elems *info) {
	if (info->used == info->alloc) {
		info->alloc *= 2;
		info->list = realloc(info->list, info->alloc * sizeof(*info->list));
		info->is_orig = realloc(info->is_orig, info->alloc * sizeof(*info->is_orig));
	}
	info->list[info->used] = 0;
	info->is_orig[info->used] = false;
	info->used++;
	if (info->used < 0) {
	printf("in info_add info->used is negative: %d", info->used);
	exit(1);
	}
}

void info_free(struct new_elems *info) {
	if (info->used < 0) {
	printf("in info_free info->used is negative: %d", info->used);
	exit(1);}
	free(info->list);
	free(info->is_orig);
	free(info);
}

void info_elem_free(struct new_elems *info, int i) {
	if (info->used < 0) {printf("in info_elem_free info->used is negative: %d", info->used);
	exit(1);}
	info->list[i] = info->list[info->used - 1];
	info->is_orig[i] = info->is_orig[info->used - 1];
	info->used--;
	info->list[i]++;
}

void my_print_hs(struct hs *h) {
	char *c = hs_to_str(h);
	printf("%s\n", c);
}

int merge(struct hs *h, int i, int j, struct new_elems *info) {
	array_t *a = (h->list).elems[i];
	array_t *b = (h->list).elems[j];
	array_t *extra = (h->list).elems[(h->list).used - 1];
    int idx = -1;
	if (array_is_sub(extra, b, h->len)) {
		//printf("b\n");
		my_free(&h->list, j);
		info_elem_free(info, j);
        idx = j;
	}
    if (array_is_sub(extra, a, h->len)) {
        //printf("a\n");
        my_free(&h->list, i);
        info_elem_free(info, i);
        idx = i;
    }
    return idx;
}

bool check_diffs(struct hs *h, int big_idx, int small_idx) {
	bool flag = false;
	struct hs_vec big = h->list.diff[big_idx];
	struct hs_vec small = h->list.diff[small_idx];
	for (int i = 0; i < big.used; ++i) {
		for (int j = 0; j < small.used; ++j) {
			if (array_is_sub(small.elems[j], big.elems[i], h->len)){
				flag = true;
				break;
			}
		}
		if (!flag){
			return false;
		}
		flag = false;
	}
	return true;
}

// returns idx of deleted element
int merge_check_diffs(struct hs *h, int i, int j, struct new_elems *info){
	array_t *a = (h->list).elems[i];
	array_t *b = (h->list).elems[j];
	int idx = -1;
	if (array_is_sub(a, b, h->len)){ // a eats b
		if (check_diffs(h, i, j)){	 // i eats j
			my_free(&h->list, j);
			info_elem_free(info, j);
			idx = j;
		}
	} else {					 	 // b eats a
		if (check_diffs(h, j, i)) {  // j eats i
			my_free(&h->list, i);
			info_elem_free(info, i);
			idx = i;
		}
	}
	return idx;
}

int merge_check_diffs_oneside(struct hs *h, int i, int j, struct new_elems *info){
	array_t *a = (h->list).elems[i];
	array_t *b = (h->list).elems[j];
	int idx = -1;
	if (array_is_sub(a, b, h->len)){ // a eats b
		if (check_diffs(h, i, j)){	 // i eats j
			my_free(&h->list, j);
			info_elem_free(info, j);
			idx = j;
		}
	}
	return idx;
}

array_t * create_new_elem(struct hs *h, int i, int j, int part, array_t mask1){
	struct hs_vec *v = &h->list;
	int len = h->len;
	array_t *tmp = array_create(len, BIT_X);
	bool correct = true;
	for (int p = 0; p < SIZE(len); ++p) {
		tmp[p] = v->elems[i][p] & v->elems[j][p];
		// correct z-bit from intersection of active bits
		if (p == part) {
			tmp[p] |= mask1;
		}
		if ((~tmp[p]) & ((~tmp[p]) >> 1) & ODD_MASK) {
			correct = false;
			array_free(tmp);
			break;
		}
	}
	if (correct){
		return tmp;
	} else {
		return NULL;
	}
}

bool make_new_elem (struct hs *h, int i, int j, int part, array_t mask1){
	struct hs_vec *v = &h->list;
	int len = h->len;
	array_t *tmp = array_create(len, BIT_X);
	bool correct = true;
	for (int p = 0; p < SIZE(len); ++p) {
		tmp[p] = v->elems[i][p] & v->elems[j][p];
		// correct z-bit from intersection of active bits
		if (p == part) {
			tmp[p] |= mask1;
		}
		if ((~tmp[p]) & ((~tmp[p]) >> 1) & ODD_MASK) {
			correct = false;
			array_free(tmp);
			break;
		}
	}
	if (correct) {	
	//	printf("lalala\n");
		hs_add(h, tmp);	//	my_print_hs(h);

		// add diffs
		struct hs_vec *d = &v->diff[v->used - 1];
   		my_diff(d, v->elems[v->used - 1], &v->diff[i], len);
      	my_diff(d, v->elems[v->used - 1], &v->diff[j], len);
      //	my_print_hs(h);
	}
	return correct;						
}

// for ELEMS
void blake_hs_vec (struct hs *h) {
	int len = h->len;
	struct hs_vec *v = &h->list;
	
	struct new_elems *info = malloc(sizeof(*info));
	info->alloc = v->used;
	info->list = malloc (info->alloc * sizeof(*(info->list)));
	info->is_orig = malloc (info->alloc * sizeof(*(info->is_orig)));
	for (int i = 0; i < v->used; ++i){
		info->list[i] = 1;
		info->is_orig[i] = true;
	}
	info->used = v->used;
	int deleted_idx = -1;
	bool made_new = false;
	// expanding
	for (int i = 0; i < v->used; ++i) {
		int j_init = 0;
		if (info->is_orig[i]){
			j_init = i + 1;
		}
		//printf("i=%d, j_init=%d\n", i, j_init);
		for (int j = j_init; j < v->used; ++j) {
			//printf("j=%d\n", j);
			// check every part in elems, searching for two different bits 1 and 0
			if (i == j) continue;
			for (int part = 0; part < SIZE(len); ++part) { // inside array_t*
				array_t mask1 = 3u;
				array_t mask2 = ~mask1;
				array_t tmp_i = 0;
				array_t tmp_j = 0;
				for (; mask1 > 0; mask1 <<= 2) { // inside array_t
                    //printf("i=%d j=%d part=%d h size=%d\n", i, j, part, hs_count(h));
                    //printf("%lx\n", mask1);
					tmp_i = ~(v->elems[i][part] & mask1 | mask2);
					tmp_j = ~(v->elems[j][part] & mask1 | mask2);
				    
					// if found - make new elem
					if (tmp_i != 0 && tmp_j != 0 && tmp_i != tmp_j){
						made_new = make_new_elem(h, i, j, part, mask1);
						if (made_new){
							my_compact(&v->diff[v->used - 1], NULL, len);
							add_info(info);		
							//my_print_hs(h);
							deleted_idx = merge(h, i, j, info);
							//my_print_hs(h);
							made_new = false;
							if (deleted_idx == -1) {
								for (int m = 0; m < v->used - 1; ++m){
									deleted_idx = merge_check_diffs_oneside(h, v->used - 1, m, info);
								}
							}
							if (deleted_idx == -1){
								free(v->elems[v->used - 1]);
								my_destroy(&v->diff[v->used - 1]);
								v->used--;
							}
							//printf("deleted_idx=%d\n", deleted_idx);
							break;
                        }
      				}
      				//printf("deleted idx=%d\n", deleted_idx);
					mask2 <<= 2;
					mask2 += 3;
				}
				if (deleted_idx != -1){
					break;
				}
			}
			if (deleted_idx <= i && deleted_idx >= 0) {
				//printf("--i\n");
				i = deleted_idx - 1;
				deleted_idx = -1;
				break;
			}
			if (deleted_idx <= j && deleted_idx >= 0) {
				//printf("--j\n");
				j = deleted_idx - 1;
				deleted_idx = -1;
			}
			//getchar();
		}
	}
	//my_print_hs(h);
	// need to check, if there's something, that could be eaten
	for (int i = 0; i < h->list.used; ++i){
		for (int j = i + 1; j < h->list.used; ++j){
			int idx = merge_check_diffs(h, i, j, info);
			if (idx == i) {
				--i;
				break;
			}
			if (idx == j) {
				--j;
			}
		}
	}
	//my_print_hs(h);
	//for (int i = 0; i < info->used; ++i) {
	//	printf("%d ", info->list[i]);
	//}
	//printf("\n");
	info_free(info);

}
