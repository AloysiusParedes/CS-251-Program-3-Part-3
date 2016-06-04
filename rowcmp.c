
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tbl.h"


char translate_row_cmp(ROW_CMP result) {

  switch(result) {
	case equal:  return '=';
	case a_dom_b:  return '<';
	case b_dom_a:  return '>';
	default:  return '?';
  }
}

int main(int argc, char *argv[] ){
Table *t, *t_min;
FILE *outstream = stdout;
FILE *instream = stdin;
char *input = "standard-in";
int r1, r2;
ROW_CMP result;

  t = tbl_from_stream(instream);
  if(t==NULL){
	fprintf(stderr, "Failed to read table.  Goodbye\n");
	return 0;
  }

  fprintf(outstream, "PRINTING ALL PAIR_WISE ROW COMPARISIONS\n\n");

  fprintf(outstream, "\n      ");
  for(r1 = 0;  r1 < t->nrows; r1++) {
	fprintf(outstream, "r%-4i", r1);
  }
  printf("\n");
  for(r1 = 0;  r1 < t->nrows; r1++) {
	fprintf(outstream, "r%-5i", r1);
  	for(r2 = 0;  r2 < t->nrows; r2++) {
	    result = cmp_rows(&(t->rows[r1]), &(t->rows[r2]));
	    fprintf(outstream, "%-5c", translate_row_cmp(result));
	}
	fprintf(outstream, "\n");
  }
  fprintf(outstream, "\n");

  tbl_free(t);
  return 0;
}
