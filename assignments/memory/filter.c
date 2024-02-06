/*
 * Implementation of functions that filter values in strings.
 *****
 * YOU MAY NOT USE ANY FUNCTIONS FROM <string.h> OTHER THAN
 * strcpy() and strlen()
 *****
 */

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "filter.h"


/*
 * YOU MAY FIND THIS FUNCTION USEFUL.
 * Return true if and only if character <c> is in string <s>.
 */
// Remove due to unused warnings
// static bool includes(char c, char *s) {
// 	while( *s && c != *s ) {
// 		s++ ;
// 	}
// 	return c == *s ;
// }

/*
 * YOU MAY ALSO FIND THIS FUNCTION USEFUL.
 * Return true if and only if string <pre> is a prefix of string <s>.
 */
static bool prefix(char *s, char *pre) {
	while( *pre && *s == *pre ) {
		s++ ;
		pre++ ;
	}
	return *pre == '\0' ;
}


/*
 * Return a pointer to a string that is a copy of <string> with
 * all occurrences of <ch> removed. The returned string must occupy
 * the least possible dynamically allocated space.
 *****
 * YOU MAY *NOT* USE INTEGERS OR ARRAY INDEXING.
 *****
 */
char* filter_ch_ptr(char *string, char ch) {
	char* copy = (char*) malloc(strlen(string) + 1);
	if (copy == NULL)
		return NULL;

	char* copy_index = copy;
	char* index = string;
	while(*index != '\0') {
		if (*index == ch) {
			index++;
			continue;
		}
		*(copy_index) = *index;
		copy_index++;
		index++;
	}
	*(copy_index) = '\0';

	// Re-allocate copy to be minimum size
	copy = (char*) realloc(copy, strlen(copy) + 1);
	return copy;
}



/*
 * Return a pointer to a copy of <string> after removing all
 * occurrrences of any characters in <remove>.
 * The returned string must occupy the least possible dynamically
 * allocated space.
 *****
 * YOU MAY *NOT* USE INTEGERS OR ARRAY INDEXING.
 *****
 */
char* filter_any_ptr(char *string, char* remove) {
	char* copy = (char*) malloc(strlen(string) + 1);

	if(copy == NULL)
		return NULL;

	strcpy(copy, string);

	char* remove_index = remove;

	while(*remove_index != '\0') {
		copy = filter_ch_ptr(copy, *remove_index);
		remove_index++;
	}

	// Since filter_ch_ptr already re-allocates, we don't need to
	return copy;
}



/*
 * Return a pointer to a copy of <string> after removing all
 * occurrrences of the substring <substr>.
 * The returned string must occupy the least possible dynamically
 * allocated space.
 *****
 * YOU MAY *NOT* USE ARRAY INDEXING.
 *****
 */
char *filter_substr(char *string, char* substr) {
	char* copy = (char *) malloc(strlen(string) + 1);

	if(copy == NULL)
		return NULL;

	char* copy_indexer = copy;
	char* char_pointer = string;
	char* char_indexer = char_pointer;

	while(*char_indexer != '\0') {
		if(prefix(char_indexer, substr))
			char_indexer += strlen(substr);
		else
			*(copy_indexer++) = *(char_indexer ++);
	}
	
	*(copy_indexer) = '\0';
	copy = realloc(copy, strlen(copy) + 1); // No need to NULL check, since we're returning NULL even if it returns.

	return copy;
}

// char* indexOf(char* string, char* substr) {
// 	char* char_pointer = string;
// 	
// 	char* buffer = malloc(strlen(substr));
// 	char* buffer_index = buffer;
// 	while(*char_pointer != '\0') {
// 		if(*char_pointer == *(substr + (buffer_index - buffer))) {
// 			*(buffer_index++) = *char_pointer;
// 
// 			if(buffer_index - buffer == strlen(substr)) {
// 				free(buffer);
// 				return ++char_pointer - strlen(substr);
// 			}
// 		}
// 		char_pointer++;
// 	}
// 	// Substr is not in string
// 	free(buffer);
// 	return NULL;
// }
