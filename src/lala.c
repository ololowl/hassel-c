/* pow example */
#include <stdio.h>      /* printf */
#include <math.h>       /* pow */



int mypower(int a, int p) {
	int ans = 1;
	for (int i = 1; i <= p; ++i){
		ans *= a;
	}
	return ans;
}

int main ()
{
	printf("%d\n", mypower(2, 0));
  	return 0;
}