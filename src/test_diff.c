#include "hs.c"
#include "array.c"
#include <time.h>


enum {MASKLEN = 32};

array_t *generate_array (int len) {
	int slen = len + len / 8;
	int counter = 0;
	char s[slen];
	for (int i = 0; i < slen - 1; ++i) {
		if (counter == 8) {
			s[i] = ',';
			counter = 0;
			continue;
		}
		else {
			switch (rand() % 10) {
				case 0:  s[i] = '0'; break;
				case 1:  s[i] = '1'; break;
				default: s[i] = 'x'; break;
			}
		}
		++counter;

	}
	s[slen-1] = '\0';
	for (int i = 0; i < slen; ++i) {
		printf("%c", s[i]);
	}
	printf("\n");
	return array_from_str(s);
}

int main () {
	struct hs *hs1 = hs_create(2);
	struct hs *hs2 = hs_create(2);
	struct hs *hs3 = hs_create(2);
	struct hs *hs4 = hs_create(2);
	
	array_t *elemXX = array_from_str("xxxxxxxx,xxxxxxxx");
	array_t *elem11 = array_from_str("xxxxxxxx,xxxxxx11");
	array_t *elem00 = array_from_str("xxxxxxxx,xxxxxx00");
	array_t *elem01 = array_from_str("xxxxxxxx,xxxxxx01");
	array_t *elem10 = array_from_str("xxxxxxxx,xxxxxx10");
	hs_add(hs1, elemXX);
	hs_add(hs2, elem00);
	hs_minus(hs1, hs2);
	hs_print(hs1);
/*
	hs_add(hs2, elemXX);
	hs_diff(hs2, elem01);
	hs_print(hs2);


	hs_add(hs3, elemXX);
	hs_diff(hs3, elem10);
	hs_print(hs3);

	hs_add(hs4, elemXX);
	hs_diff(hs4, elem11);
	hs_print(hs4);
	
	hs1 = hs_isect_a(hs1, hs2);
	hs3 = hs_isect_a(hs3, hs4);
	hs_print(hs1);
	hs_print(hs3);
	hs1 = hs_isect_a(hs1, hs3);
	hs_print(hs1);
	
	if (hs_compact(hs1)) {
		hs_print(hs1);
	} else {
		printf("NULL");
	}
*/
	return 0;
}
/*
int main () {
	int n = 4;
	struct hs *elems1 = hs_create(n);
	struct hs *elems2 = hs_create(n);
	srand(time(NULL));
	array_t *elem1 = generate_array(MASKLEN);
	array_t *elem2 = generate_array(MASKLEN);
	array_t *elem3 = generate_array(MASKLEN);
	array_t *elem4 = generate_array(MASKLEN);
	array_t *elem5 = generate_array(MASKLEN);
	array_t *elem6 = generate_array(MASKLEN);
	
	printf("len of array_t - %lu\n", sizeof(array_t));
	
	hs_add(elems1, elem1);
	hs_add(elems1, elem2);
	hs_add(elems1, elem3);
	//hs_add(elems1, elem4);
	hs_print(elems1);
printf("\n");
	hs_diff(elems1, elem4);
	hs_diff(elems1, elem5);

	hs_print(elems1);

	//hs_add(elems2, elem2);
	//hs_add(elems2, elem4);
	//hs_add(elems2, elem5);
	hs_add(elems2, elem6);
	printf("length hs - %d\n", hs_count(elems1));
	printf("length hs - %d\n", hs_count(elems2));
	

	return 0;
}*/
