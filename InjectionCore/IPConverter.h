#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int GetIPFromString (const char *ip, unsigned long *plong)
{
char *next = NULL;
const char *curr = ip;
unsigned long tmp;
int i, err = 0;
*plong = 0;
for (i = 0; i < 4; i++) {
tmp = strtoul(curr, &next, 10);
if (tmp >= 256 || (tmp == 0 && next == curr))
{
err++;
break;
}
*plong = (*plong << 8) + tmp;
curr = next + 1;
}
if (err) {
return 1;
}
else {
return 0;
}
}
