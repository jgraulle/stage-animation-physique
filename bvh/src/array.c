/* 
	tableaux dynamiques
 */

#include <stdlib.h>
#include <assert.h>

void array_add(void **parray, int *size, int n, int e_size, int e_more)
{
	if(n >= *size)
	{
		*size= ((n / e_more) + 1) * e_more;
		
		*parray= realloc(*parray, *size * e_size);
		assert(*parray!=NULL);
	}
}
