#ifndef UTILH
#define UTILH

/* some compatibility functions
   (standerd runtime library cannot be used) */
extern void zeromemory(void *ptr, unsigned long count);
extern int comparememory(const void *ptr1, const void *ptr2, unsigned long count);
extern long me_strlen(const char *s);
extern void me_strcpy(char *d, const char *s);


#endif
