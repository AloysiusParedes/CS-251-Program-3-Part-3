
#define MAX_ROWS 100
#define MAX_COLS 20
#define MAX_LINE_LEN 201

// structs NOT HIDDEN FROM CLIENT
// Not technically an ADT... but we should still
//   try to apply the ADT princdfples unless we have
//   a very good reason...

typedef struct {
  int rownum;
  int numcols;
  double *vals;
  int *sort_by_ptr;
  int *dir_ptr;
  
  char *rowname; //to hold row name (string)
} Row;

typedef struct {
  int nrows; 
  int ncols;
  int sort_by;   // column to sort by
  int direction; // +1 for increasing; 
		 // -1 for decreasing
  Row *rows;
  
  char **colname; //to hold column labels (array of strings)
} Table;

extern Table * tbl_from_stream(FILE *stream); 
extern void tbl_free(Table *t);
extern void tbl_sort(Table *t, int col, int incr);
extern void tbl_print(Table *t, FILE *stream); 

enum row_cmp {
	equal, a_dom_b, b_dom_a, incomparable
};

typedef enum row_cmp ROW_CMP;

/** TODO 
*   
* function: cmp_rows
*
* desc:  takes two rows throug pointers a and b and returns:
*df
*        equal if the rows are identical in every column (dimension)
*        a_dom_b  if row given by a dominates that given by b:
*		they are not equal and
*		in every column, row-a is no larger than row-b (in that
*			same column)
*	 b_dom_a  if b dominates a 
*	 incomparable if neither a dominates b or vice-versa (and
*		they are not equal).
*
*/
extern ROW_CMP cmp_rows(Row *a, Row *b);

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
extern Table * tbl_minima(Table *t);

extern int tbl_num_comparisons();




