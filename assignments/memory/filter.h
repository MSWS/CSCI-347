/*
 * Interface to functions that filter values from strings.
 */


/*
 * Return a pointer to a string that is a copy of <string> with
 * all occurrences of <ch> removed. The returned string must occupy
 * the least possible dynamically allocated space.
 */
char *filter_ch_ptr(char *string, char ch) ;


/*
 * Return a pointer to a copy of <string> after removing all
 * occurrrences of any characters in <remove>.
 * The returned string must occupy the least possible dynamically
 * allocated space.
 */
char *filter_any_ptr(char *string, char* remove) ;

/*
 * Return a pointer to a copy of <string> with all occurrences of
 * <substr> removed.
 * <substr> cannot be the empty string ("").
 * The returned string must occupy the least possible dynamically
 * allocated space.
 */
char *filter_substr(char *string, char* substr) ;
