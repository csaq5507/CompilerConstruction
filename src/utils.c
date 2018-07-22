//
// Created by ivan on 17.05.18.
//
#include "mCc/utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <assert.h>
#include <zconf.h>

#define ARRAY_SIZE 2048

char *new_string(const char *pattern, ...)
{
	char help[ARRAY_SIZE] = {0};
	va_list args;
	va_start(args, pattern);
	vsprintf(help, pattern, args);
	va_end(args);
	char *dest = malloc(sizeof(char) * (strlen(help) + 1));
	strcpy(dest, help);
	return dest;
}

char *copy_string(const char *src)
{
	char *dest = malloc(sizeof(char) * (strlen(src) + 1));
	if (dest == NULL) {
		printf("Error malloc");
		assert(NULL);
	}
	strcpy(dest, src);
	return dest;
}

char *copy_string_realloc(char *dest, const char *src)
{
	dest = realloc(dest, sizeof(char) * (strlen(src) + 1));
	if (dest == NULL) {
		printf("Error malloc");
		assert(NULL);
	}
	strcpy(dest, src);
	return dest;
}

char *replace(char const *const original, char const *const pattern,
			  char const *const replacement)
{
	size_t const replen = strlen(replacement);
	size_t const patlen = strlen(pattern);
	size_t const orilen = strlen(original);

	size_t patcnt = 0;
	const char *oriptr;
	const char *patloc;

	// find how many times the pattern occurs in the original string
	for (oriptr = original; (patloc = strstr(oriptr, pattern)) != NULL;
		 oriptr = patloc + patlen) {
		patcnt++;
	}

	{
		// allocate memory for the new string
		size_t const retlen = orilen + patcnt * (replen - patlen);
		char *const returned =
				(char *)malloc(sizeof(char) * (retlen + 1));

		if (returned != NULL) {
			// copy the original string,
			// replacing all the instances of the pattern
			char *retptr = returned;
			for (oriptr = original;
				 (patloc = strstr(oriptr, pattern)) != NULL;
				 oriptr = patloc + patlen) {
				size_t const skplen = patloc - oriptr;
				// copy the section until the occurence of the
				// pattern
				strncpy(retptr, oriptr, skplen);
				retptr += skplen;
				// copy the replacement
				strncpy(retptr, replacement, replen);
				retptr += replen;
			}
			// copy the rest of the string.
			strcpy(retptr, oriptr);
		}
		return returned;
	}
}