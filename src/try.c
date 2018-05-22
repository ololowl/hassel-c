#include <iostream>

static int
int_str (uint16_t x, char *out)
{
  if (x == UINT16_MAX) return sprintf (out, "DX,");
  x = (x >> 1) & 0x5555;
  x = (x | (x >> 1)) & 0x3333;
  x = (x | (x >> 2)) & 0x0f0f;
  x = (x | (x >> 4)) & 0x00ff;
  return sprintf (out, "D%d,", x);
}

int main () {
	char out[5];
	int_str (0xaaaa, out);
	std::cout << out;
	return 0;
}