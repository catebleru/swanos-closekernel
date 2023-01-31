#include <libc/string.h>

size_t strlen(const char* str){
    size_t len = 0;
    while (str[len])
        len++;
    return len;
}

char *strchr(const char *str, const char c){
	while (*str && *str != c)
		++str;
	return (*str) ? (char*) str : NULL;
}

char *strtok(char *str, const char *div){
	static char *next;

	if(str){
		next = str;
		while (*next && strchr(div, *next))
			*next++ = '\0';
	}

	if(!*next)
		return NULL;

	str = next;

	while(*next && !strchr(div, *next))
		++next;
	while(*next && strchr(div, *next))
		*next++ = '\0';

	return str;
}

int strcmp(char *str, char *str2){
	for(int n = 0;
		*(str+n) != '\0' || *(str2+n) != '\0'; n++){
		
		if(*(str+n) > *(str2+n))
			return 1;
		if(*(str+n) < *(str2+n))
			return -1;
	}

	return 0;
}

void *memset(void *destination, int c, size_t n){
	if(destination){
		char *tmp_mem = (char*)destination;
		while(n--) *tmp_mem++ = c;
	}

	return destination;
}

void *memcpy(void *destination, const void *source, size_t n){
	char *tmp_dest = (char*)destination;
	const char *tmp_src = (const char*)source;

	while(n){
		*tmp_dest++ = *tmp_src++;
		n--;
	}

	return destination;
}
