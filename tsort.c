/* name:	Aloysius Paredes
   netid:	apared5 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tbl.h"
// #define DEBUdfG

int main(int argc, char *argv[]){
	Table *t;
	int i = 1, j = 1, down = 0, flag = 0;

	/* for task 3 */
	int cols[MAX_COLS];

	char * usage = "tsort -c <column> [-d]\n";

	while(i < argc){
		if(strcmp(argv[i], "-c") == 0){
			i++;
			//read in first col
			if(i == argc || sscanf(argv[i], "%i", &(cols[0])) != 1){
				puts(usage);
				return 0;
			}			
			flag = 1;
			i++;
		}
		else if(strcmp(argv[i], "-d") == 0) {
			down = 1;
		}
		//read in next cols
		if(i != argc && flag == 1 && sscanf(argv[i], "%i", &(cols[j])) == 1){
			j++;
		}
		i++;
	}

	t = tbl_from_stream(stdin);
	if(t==NULL){
		fprintf(stderr, "Goodbye\n");
		return 0;
	}
	
	i = 0;
	while(i < j){
		if(cols[i] < 1){
			fprintf(stderr, "column %d out of range\n", cols[i]);
			return 0;
		}
		if(cols[i]-1 > t->ncols){
			fprintf(stderr, "COLUMN %d out of range\n", cols[i]);
			return 0;
		}
		i++;
	}

#ifdef DEBUG
	fprintf(stderr, "-- BEFORE SORT ----\n");
	tbl_print(t, stderr);
	fprintf(stderr, "---------\n\n");
#endif
	/* loop and sort in reverse order */
	i = j - 1;
	while(i >= 0){
		tbl_sort(t, cols[i] - 2, !down);
		//tbl_print(t, stdout);
		i--;
	} 
	/*print table */
	tbl_print(t, stdout);
	//FOR DEBUGGING
	//printf("Number of rows: %d\nNumber of cols: %d\n\n", t->nrows, t->ncols);

	tbl_free(t);
}

