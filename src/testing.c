#include "blake.h"
#include <time.h>

#ifndef SIZE
#define SIZE(L) ( DIV_ROUND_UP (2 * (L), 2 * sizeof (array_t)) )
#endif


enum {MAX_POS_MASKS = 100, N_EXPERIMENTS = 150};

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
		if (i % 8 == 0 && i != len - 1 && i != 0){
			s[i + i / 8 - 1] = ',';
		}
		int r = rand() % 3;
		switch(r){
			case 0: s[i + i / 8] = '0'; break;
			case 1: s[i + i / 8] = '1'; break;
			default: s[i + i / 8] = 'x'; ++ans;
		}
	}
	s[len + len / 8 - 1] = '\0';
	return ans;	
}

int generate_array_t(int len, array_t *a){
	int str_len = str_size(len);
	char s[str_len];
	int xs = generate_mask_str(len, s);
	a = array_from_str(s);
	return xs;
}

void generate_diff_array(int n, struct hs *h, int i, char *s, int len){
	struct hs_vec *v = &h->list.diff[i];
	for (int i = 0; i < 20; ++i){
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
		array_t *a = array_from_str(ss);
		my_append(v, a, true);
	}
}

struct hs *generate_hs(int n_positive, int len) {
	struct hs *h = hs_create(SIZE(len));
	for (int i = 0; i < n_positive; ++i){
		array_t *a;
		int str_len = str_size(len);
		char s[str_len];
		int xs = generate_mask_str(len, s);
		//printf("%s\n", s);
		a = array_from_str(s);
		hs_add(h, a);
		int max_neg = mypower(3, xs);
		int n_neg = rand() % max_neg;
		generate_diff_array(n_neg, h, i, s, len);
	}	
	return h;
}


int main() {
	srand(time(NULL));
	int len = 128;
	int n_positive = rand() % MAX_POS_MASKS;
	struct hs *h = generate_hs(n_positive, len);
	int x0 = hs_count(h) + hs_count_diff(h);
	//blake_hs_vec(h);
	int x1 = hs_count(h) + hs_count_diff(h);
	printf("%d %d\n", x0, x1);
	hs_free(h);
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