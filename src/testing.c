#include "blake.h"
#include <time.h>

#ifndef SIZE
#define SIZE(L) ( DIV_ROUND_UP (2 * (L), sizeof (array_t)) )
#endif


enum {MAX_POS_MASKS = 100, MAX_NEG = 30};

int str_size(int len) {
	return len + len / 8;
}

int mypower(int a, int p) {
	int ans = 1;
	for (int i = 1; i <= p; ++i){
		ans *= a;
	}
	return ans;
}

//returns amount of x's
int generate_mask_str(int len, char *s) {
	int ans = 0;
	for (int i = 0; i < len; ++i){
		if (i % 8 == 0 && i < str_size(len) - 1 && i != 0){
			s[i + i / 8 - 1] = ',';
		}
		int r = rand() % 3;
		switch(r){
			case 0: s[i + i / 8] = '0'; break;
			case 1: s[i + i / 8] = '1'; break;
			default: s[i + i / 8] = 'x'; ++ans;
		}
	}
	s[str_size(len) - 1] = '\0';
	return ans;	
}

void generate_diff_array(int n_neg, struct hs *h, int idx, char *s, int len){
	struct hs_vec *v = &h->list.diff[idx];
	array_t *a;
	for (int t = 0; t < n_neg; ++t){
		char ss[str_size(len)];
		for (int j = 0; j < str_size(len); ++j){
			if (s[j] == '1' || s[j] == '0' || s[j] == ',') {
				ss[j] = s[j];
			} else {
				int r = rand() % 3;
				switch(r){
					case 0: ss[j] = '0'; break;
					case 1: ss[j] = '1'; break;
					default: ss[j] = 'x';
				}
			}
		}
		ss[str_size(len) - 1] = '\0';
		a = array_from_str(ss);
		//printf("%s\n", ss);
		my_append(v, a, true);
	}
}

void generate_hs(struct hs *h, int n_positive, int len) {
	array_t *a;
	for (int i = 0; i < n_positive; ++i){
		char s[str_size(len)];
		int xs = generate_mask_str(len, s);
		//printf("s %s\n", s);
		a = array_from_str(s);
	//	printf("a %d %s\n", i, array_to_str(a, SIZE(len), true));
		hs_add(h, a);
		int n_neg = rand() % MAX_NEG;

		generate_diff_array(n_neg, h, i, s, len);
		//printf("\n");
	}	
}

int main() {
	srand(time(NULL));
	int len =256;
	int n_positive = rand() % MAX_POS_MASKS;
	//printf("%d\n", n_positive);
	struct hs *h = hs_create(len / 8);
	generate_hs(h, n_positive, len);
	int x0 = hs_count(h) + hs_count_diff(h);
	//hs_compact(h);
	//my_print_hs(h);
	//printf("\n");
	
	//printf("AA \n");
	int x01 = hs_count(h) + hs_count_diff(h);
	printf("pos-%d neg-%d\n", hs_count(h), hs_count_diff(h));
	
	//struct hs *orig;
	//orig = hs_create(len / 8);
	//hs_copy (orig, h);

	blake_hs_vec(h);
	int x1 = hs_count(h) + hs_count_diff(h);
	//my_print_hs(h);
	printf("res pos-%d neg-%d\n", hs_count(h), hs_count_diff(h));
	//printf("%d %d %d\n", x0, x01, x1);
	//struct hs *tmp, *tmp2;
	//tmp = hs_create(len / 8);
	//tmp2 = hs_create(len / 8);
	//hs_copy (tmp, h);
	//hs_copy (tmp2, orig);
	//
    //
	//hs_minus(tmp2, h);
	//hs_minus(tmp, orig);
    //
	//if (hs_compact(tmp2) == 0 && hs_compact(tmp) == 0){
	//	printf("YES\n");
	//} else {
    //    printf("NO\n");
    //}
	//hs_free(h);
	return 0;
}

/*
int main (){
	array_t *a = array_from_str("xxx1xxx0");
	
	int len = SIZE(8);
	struct hs *h = hs_create(len);
	struct hs_vec *v = &h->list;
	my_append(v, a, false);
	a = array_from_str("xxx11010");
	my_append(&v->diff[0], a, true);
	a = array_from_str("xxx11110");
	my_append(&v->diff[0], a, true);
	a = array_from_str("xxx101x0");
	my_append(&v->diff[0], a, true);
	
	a = array_from_str("xxx0xxxx");
	my_append(v, a, false);
	a = array_from_str("xxx0xx1x");
	my_append(&v->diff[1], a, true);
	a = array_from_str("xxx0xxx0");
	my_append(&v->diff[1], a, true);
	a = array_from_str("xxx00000");
	my_append(&v->diff[1], a, true);
	
	a = array_from_str("xxx1xx11");
	my_append(v, a, false);
	a = array_from_str("xxx10x11");
	my_append(&v->diff[2], a, true);

	my_print_hs(h);
		
	blake_hs_vec(h);
	
	my_print_hs(h);

	hs_compact(h);
	my_print_hs(h);

	hs_free(h);
	
	return 0;
}*/