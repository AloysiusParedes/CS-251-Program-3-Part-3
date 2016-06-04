
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tbl.h"


int main(int argc, char *argv[] ){
Table *t, *t_min;
FILE *outstream = stdout;
FILE *instream = stdin;
char *input = "standard-in";

  t = tbl_from_stream(instream);
  if(t==NULL){
	fprintf(stderr, "Failed to read table.  Goodbye\n");
	return 0;
  }

  t_min = tbl_minima(t);

  fprintf(outstream, "\n# minima of table read from '%s'\n", input);
  fprintf(outstream, "# NUM_COMPARISONS:  %i\n\n", tbl_num_comparisons());

  tbl_print(t_min, outstream);

  // temporary guard while tbl_minima is unimplemented
  if(t_min != NULL) {
    tbl_free(t_min);
  }

  tbl_free(t);
  return 0;
}
