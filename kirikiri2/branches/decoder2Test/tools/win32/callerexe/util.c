/* some compatibility functions
   (standerd runtime library cannot be used) */
void zeromemory(void *ptr, unsigned long count)
{
	char *p = (char*)ptr;
	unsigned long i;
	for(i = 0; i < count; i++) p[i] = 0;
}

int comparememory(const void *ptr1, const void *ptr2, unsigned long count)
{
	const char *p1 = (const char*)ptr1;
	const char *p2 = (const char*)ptr2;
	while(count--) if(*(p1++) != *(p2++)) return 0;
	return 1;
}

long me_strlen(const char *s)
{
	const char *o = s;
	while(*s) s++;
	return s - o;
}


void me_strcpy(char *d, const char *s)
{
	while((*(d++) = *(s++)) != '\0');
}

