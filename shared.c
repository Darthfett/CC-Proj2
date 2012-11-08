/* shared.c
 *
 * Implements all shared subroutines
 */

#include "shared.h"

int new_unique_id_ = 0;

int get_unique_id(void)
{
    int id = new_unique_id_;
    new_unique_id_++;
    return id;
}

/* -----------------------------------------------------------------------
 * Returns a new name for a new temporary type (for 3-address code)
 * -----------------------------------------------------------------------
 */

int new_type_num = 0;
char *user_type_prefix = "__usr_t";

char *new_type()
{
    char *name;
    char num[20];
    snprintf(num, 20, "%d", new_type_num);

    name = (char*) malloc(sizeof(char) * (strlen(user_type_prefix) + strlen(num) + 1));
    strcpy(name, user_type_prefix);
    strcat(name, num);
    new_type_num++;
    return name;
}

/* ----------------------------------------------------------------------- 
 * Returns a hashkey value for the given lexeme
 * ----------------------------------------------------------------------- 
 */
int makekey(char* lexeme, int max_hashkeys)
{
  int len;
  int i;
  long charsum = 0;

  len = strlen(lexeme);
  for (i = 0; i < len; i++){
    charsum += lexeme[i];
  }

  return charsum % max_hashkeys;
}



/* ----------------------------------------------------------------------- 
 * Converts a string to lowercase
 * ----------------------------------------------------------------------- 
 */

/*
char * tolower(char *s)
{
  int len;
  int i;
  char *new;

  if (s != NULL)
    len = strlen(s);
  else
    len = 0;

  new = (char *) malloc(len + 1); // +1 for '\0'

  for (i = 0; i < len; i++) {
    // if an uppercase character
    if (s[i] >= 65 && s[i] <=  90)
      new[i] = s[i] + 32;
    else
      new[i] = s[i];
  }

  new[len] = '\0';

  return new;
}
*/

/* ----------------------------------------------------------------------- 
 * Prints the specified amount of tabs
 * ----------------------------------------------------------------------- 
 */
void print_tabs(int numOfTabs)
{
  int i = 0;
  while (i < numOfTabs) {
    printf("\t");
    i++;
  }
}



/* ----------------------------------------------------------------------- 
 * Returns a string representation of the int value
 * ----------------------------------------------------------------------- 
 */
char *inttostring(int value)
{
  char *s;

  s = (char *) malloc(MAX_NEW_CHAR_SIZE);
  sprintf(s, "%d", value);

  return s;
}



/* ----------------------------------------------------------------------- 
 * Returns a string representation of the long value
 * ----------------------------------------------------------------------- 
 */
char *longtostring(long value)
{
  char *s;

  s = (char *) malloc(MAX_NEW_CHAR_SIZE);
  sprintf(s, "%ld", value);

  return s;
}
