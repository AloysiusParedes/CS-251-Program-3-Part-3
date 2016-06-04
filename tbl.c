/* name:	Aloysius Paredes
   netid:	apared5 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tbl.h"

void trim_string(char *s) {
	//check if string is empty
	if(strlen(s) - 1 == 0)
		return;
	
	int i, start = 0, end = strlen(s) - 1;

	//find the index where the word starts
    while(isspace(s[start]))
        start++;

	//find the index where the word ends
    while((end >= start) && isspace(s[end]))
        end--;

    //shift all characters back to the start of the string array.
    for(i = start; i <= end; i++)
        s[i - start] = s[i];

	//null terminate string
    s[i - start] = '\0'; 
}

static char * str2row(Table *t, char *buf); 

Table * tbl_init() {
int r, c;

  Table *t = malloc(sizeof(Table));

  t->sort_by = 0; // set it to something
  t->direction = 1; // default increasing

  t->nrows = 0;
  t->ncols = 0;
  t->colname = NULL; //set column labels

  t->rows = malloc(MAX_ROWS*sizeof(Row));
  for(r=0; r<MAX_ROWS; r++){
	t->rows[r].rownum = r;
	t->rows[r].vals = NULL;
	t->rows[r].rowname = NULL; //set row names
	t->rows[r].sort_by_ptr = &(t->sort_by);
	t->rows[r].dir_ptr = &(t->direction);
	t->rows[r].numcols = t->ncols;
  }
  
  return t;
}

/* function to grow the size of the rows. will grow after reaching size 100. will grow by 2. So first grow the new size will be 200. Then 400 */
void tbl_grow(Table *t, int *size){
	int r;
	/* allocate space for larger size */
	Row *temp = malloc(sizeof(Row) * (*size) * 2);
	/* copy over existing values from old row array */
	for(r=0; r < (*size); r++){
		temp[r].rownum = t->rows[r].rownum;
		temp[r].vals = t->rows[r].vals;
		temp[r].rowname = t->rows[r].rowname;
		temp[r].sort_by_ptr = t->rows[r].sort_by_ptr;
		temp[r].dir_ptr = t->rows[r].dir_ptr;
		temp[r].numcols = t->rows[r].numcols;
    }
	/* initialize the empty new indices */
	for(; r < 2 * (*size); r++){
		temp[r].rownum = r;
		temp[r].vals = NULL;
		temp[r].rowname = NULL;
		temp[r].sort_by_ptr = &(t->sort_by);
		temp[r].dir_ptr = &(t->direction);
		temp[r].numcols = t->ncols;
	}
	/* update the size */
	(*size) *= 2;
	/* deallocate the old sized rows */
	free(t->rows);
	/* set new identifier to the new sized rows */
	t->rows = temp;
	return;
}

void tbl_free(Table *t) {
int i;

  for(i=0; i<t->nrows; i++) 
	free(t->rows[i].vals);
  for(i = 0; i < t->ncols; i++){
	  free(t->colname[i]);
  }
  free(t->rows);
  free(t->colname);
  free(t);
}

Table * tbl_from_stream(FILE *stream) {
	int lno;
	Table *t;
	char buf[MAX_LINE_LEN+1];
	char *res;
	int n, i = 0;
	char *p; //for strtok
	int counter = -1; //to keep track of iterations

	t = tbl_init();
	int maxsize = MAX_ROWS; //to keep track of the size of the rows
	lno = 0;
	while(fgets(buf, MAX_LINE_LEN, stream)!=NULL){
		lno++;
		/* grow baby grow */
		if(lno == maxsize){ //when 100 line numbers are reached, grow
			tbl_grow(t, &maxsize);
		}

		n = strlen(buf);
		if(buf[n-1] != '\n') {
			tbl_free(t);		
			fprintf(stderr, "line %i too long\n", lno);
			fprintf(stderr, "line: %s\n", buf);

			return NULL; 
		}
		// skip blank lines and lines beginning with '#'
		//    which are interpreted as comments
		if(strlen(buf) != 0 && buf[0] != '#'){
			/* first pass to read in column labels
			   will only occur once to ensure second lines are not columns */
			if(counter == -1){
				//allocate space for dynamic array
				t->colname = malloc(sizeof(char*) * MAX_COLS);
				
				//read first line then read strings into array
				p = strtok(buf, ":");
				char *name = p;
				
				while(p != NULL){
					//allocate space for a max size string of 30
					t->colname[i] = malloc(sizeof(char) * 30);
					//remove any trailing or leading whitespace
					trim_string(p);
					//insert word into array
					strcpy(t->colname[i], p);
					//look at next word
					p = strtok(NULL, ":");
					i++; //to keep track of the number of columns
				}
				//increment counter
				counter++; //will be 0 after first pass
				//set number of columns based on how many columns read in
				t->ncols = i; 
				t->rows[0].numcols = i;
			}
			else{
				//allocate space of string size of 30
				t->rows[counter].rowname = malloc(sizeof(char) * 30);
				/* get row name */
				p = strtok(buf, ":");   
				//add to column label array
				char *name = p;
				//remove any trailing or leading whitespace
				trim_string(name);
				//inseart word into rowname struct
				strcpy(t->rows[counter].rowname, name);
				//increment counter
				counter++;
				//look at next word
				name = strtok(NULL, ":");
				
				res = str2row(t, name);
				if(res != NULL) {
					fprintf(stderr, "line %i: %s\n", lno, res);
					tbl_free(t);
					return NULL;
				}
			}
		}
	}
	return t;
}

static void tbl_set_rownums(Table *t) {
	int i;

	for(i=0; i<t->nrows; i++){ 
		t->rows[i].rownum = i;
		t->rows[i].numcols = t->ncols;
	}
}

static int cmp(const void *a, const void *b) {
	Row *ra = (Row *)a;
	Row *rb = (Row *)b;
	int col = *(ra->sort_by_ptr);
	int dir = *(ra->dir_ptr);

	if(ra->vals[col] < rb->vals[col])
		return -dir;
	if(rb->vals[col] < ra->vals[col])
		return dir;
	// return 0;  // not nec stable
	return ra->rownum - rb->rownum;  // ensures stable sort
}

/* comparitor to compare names for task 1. will sort row names */
static int namecmp(const void *a, const void *b) {
	Row *ra = (Row *)a;
	Row *rb = (Row *)b;
	int col = *(ra->sort_by_ptr);
	int dir = *(ra->dir_ptr);

	/* use strcmp to see which rowname is greater or less than */
	if(strcmp(ra->rowname, rb->rowname) < 0)
		return -dir;
	if(strcmp(ra->rowname, rb->rowname) > 0)
		return dir;
	// return 0;  // not nec stable
	return ra->rownum - rb->rownum;  // ensures stable sort
}
  
void tbl_print(Table *t, FILE *stream) {
	int i, j, k;

	if(t==NULL) {
		fprintf(stream, "# attempted to print non-existing table\n");
		return;
	}
	/* print out column labels */
	for(k = 0; k <= t->ncols; k++)
		fprintf(stream, "%10s", t->colname[k]);
	/* print out column divider */
	fprintf(stream, "\n");
	for(k = 0; k <= t->ncols; k++)
		fprintf(stream, "==========", t->colname[k]);
	fprintf(stream, "\n");
	/* print out row data */	
	for(i = 0; i < t->nrows; i++){
		for(j = 0; j < t->ncols; j++) {
			/* print out row name */
			if(j == 0)
				fprintf(stream, "%10s", t->rows[i].rowname);
			fprintf(stream, "%10.2f", t->rows[i].vals[j]);
		}
		fprintf(stream, "\n");
	}
}

void tbl_sort(Table *t, int col, int incr){
	t->direction = incr ? 1 : -1;
	t->sort_by = col;
	/* check to see if user inputed 1 */
	if(col < 0){
		/* sort by row name */
		qsort(t->rows, t->nrows, sizeof(Row), namecmp);
	}
	else{
		/* sort by other columns */
		qsort(t->rows, t->nrows, sizeof(Row), cmp);
	}
	tbl_set_rownums(t);
}   
/**
* success:  returns NULL
*
* failure:  returns error message
*/
static char * str2row(Table *t, char *buf) {
	int r, j;
	int nchars;
	double x;

	t->nrows++;

	r = t->nrows-1;
	j=0;
	
	//allocate space for new value to be read in
	t->rows[r].vals = malloc(t->ncols*sizeof(double));
	
	while( sscanf(buf, "%lf%n", &x, &nchars) == 1){
		//printf("Looking for number\n");
		if(j==MAX_COLS || (r>0 && j==t->ncols))
			return "line too long";
		t->rows[r].vals[j] = x;
		buf += nchars;
		j++;
	}
    if(r==0){
		t->ncols = j; 
	} 
	if(j < t->ncols) // too few columns
		return "line too short";
	return NULL;
}
  
static int NumCmps = 0;

ROW_CMP cmp_rows(Row *a, Row *b) {

	NumCmps++;    // DO NOT DELETE THIS LINE!!!

	/**
	*  TODO
	*  replace code below with your solution...
	* fprintf(stderr, "cmp_rows not implemented... returning 'equal'\n");
	*/
	
	int i = 0, count1 = 0, count2 = 0;
	/* loop through values and compare to see which domainates which */
	while(a->vals[i]){
		//check if a will dominate
		if(a->vals[i] < b->vals[i])
			count1++;
		//check if b will dominate
		if(a->vals[i] > b->vals[i])
			count2++;
		i++;
	}
	
	/* find out which row dominates the other */
	if(count1 >= 1 && count2 == 0)
		return a_dom_b;
	if(count2 >= 1 && count1 == 0)
		return b_dom_a;
	if(count1 == 0 && count2 == 0)
		return equal;
	
	/* if reached here, then incomparable */
	return incomparable;
	
	//return equal;  // placeholder...
}


/** TODO 
*   
* function: tbl_minima
*
* desc:  constructs a table containing only the non-dominated rows
*	of the given table t -- also known as the "minima"
*
*	The new table is returned.
*
*	A row is non-dominated if there is no other row that dominates it
*/
Table * tbl_minima(Table *t) {

	NumCmps = 0;   // DO NOT DELETE THIS LINE!!!

	/**
	*  TODO
	*  replace code below with your solution...
	*/
	//fprintf(stderr, "tbl_minima not implemented... returning NULL\n");
	//return NULL;
	
	int i = 0, j = 0, count = 0, incomparable = 0, index = 0, size = 100;
	
	//make new table
	Table *temp = tbl_init();
	//set number of columns to be the same
	temp->ncols = t->ncols;
	//allocate space for column labels array
	temp->colname = malloc(sizeof(char*) * MAX_COLS);
	
	//loop to allocate string size
	for(i = 0; i < temp->ncols; i++)
		temp->colname[i] = malloc(sizeof(char) * 30);
	
	//loop to copy existing values to temp
	for(i = 0; i < temp->ncols; i++)
		strcpy(temp->colname[i], t->colname[i]);
	
	for(i = 0; i < t->nrows; i++){
		for(j = 0; j < t->nrows; j++){
			if(j != i){
				//find row compare value
				ROW_CMP cmp = cmp_rows(&t->rows[i], &t->rows[j]);
				
				if(cmp == b_dom_a){
					incomparable++;
					break;
				}
			}
		}
		
		if(incomparable == 0){
			//allocate space for value
			temp->rows[index].vals = malloc(sizeof(double) * temp->ncols);
			//allocate space for string
			temp->rows[index].rowname = malloc(sizeof(char));
			//set new row index
			temp->rows[index] = t->rows[i];
			//increment the index
			index++;
			//update number of rows to index
			temp->nrows = index;
			//grow if rows reached 100, 200, ...
			if(temp->nrows >= size)
				tbl_grow(t, &size);	
		}
		//reset incomparable
		incomparable = 0;
	}
	
	//check to see if index is still 0
	if(index == 0)
		printf("No minima found\n\n");
	return temp;
}

// DO NOT CHANGE THIS FUNCTION!!
int tbl_num_comparisons() {

	return NumCmps;

}

/* FUNCTIONS FOR TCALC */


