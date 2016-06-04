
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
* toy program demoing strtok
*/

void p_char_arr(char *s, int n) {
int i;

  for(i=0; i<n; i++) {
     if(s[i] == '\0') 
        printf("  <end-of-str>\n");
     else if(s[i] == ' ') 
        printf("  <space>\n");
     else 
        printf("  '%c'\n", s[i]);
  }
}


int main() {
char s[] = "1 :  23:89:99 :  44 xyz";
char *p;

  printf("sizeof(s) = %lu\n", sizeof(s));
  printf("strlen(s) = %lu\n", strlen(s));

  printf("array BEFORE strtok:\n\n");
  p_char_arr(s, sizeof(s)/sizeof(char));

  printf("tokens from strtok:\n");
  p = strtok(s, ":");   

  while(p != NULL) {
	printf("token: '%s'\n", p);
	p = strtok(NULL, ":");     // NULL means "keep scanning same string"
  }
  printf("array AFTER strtok:\n\n");
  p_char_arr(s, sizeof(s)/sizeof(char));

}



