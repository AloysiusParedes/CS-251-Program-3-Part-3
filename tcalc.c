/* name:	Aloysius Paredes
   netid:	apared5 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tbl.h"

double eval(char **toks, Row row, int *tokIndex, int maxcol){
	double a, b, c, temp;
	
	//increment token index
	(*tokIndex)++;
	
	//get next token
	char *currTok = toks[*tokIndex];
	
	/*** BASE CASE 1 ***/
	//check to see if t can be parsed as a double (SSCANF)
	if(sscanf(currTok, "%lf", &temp))
		return temp;
	
	/*** BASE CASE 2 ***/
	//check to see if t begins with '$'
	if(currTok[0] == '$'){
		char *val = strtok(currTok, "$");
		//parse integer c after '$'
		int col = atoi(val);
		col--;
		
		/* check to see if column is within the bounds */
		if(col < 1 || col > maxcol){
			printf("\n\nOut of bounds column: %d\nExiting...\n\n", col+1);
			exit(1);
		}
		col++;
		return row.vals[col - 2];
	}
	
	/*** ARITHMETIC OPERATORS ***/
	//ADDITION
	if(currTok[0] == '+'){
		a = eval(toks, row, tokIndex, maxcol);
		b = eval(toks, row, tokIndex, maxcol);
		return a + b;
	}
	//SUBTRACTION
	if(currTok[0] == '-'){
		a = eval(toks, row, tokIndex, maxcol);
		b = eval(toks, row, tokIndex, maxcol);
		return a - b;
	}
		
	//MULTIPLICATION
	if(currTok[0] == '*'){
		a = eval(toks, row, tokIndex, maxcol);
		b = eval(toks, row, tokIndex, maxcol);
		return a * b;
	}
	
	//DIVISION
	if(currTok[0] == '/'){
		a = eval(toks, row, tokIndex, maxcol);
		b = eval(toks, row, tokIndex, maxcol);
		//check to see if b is 0
		if(b == 0.0){
			//print error message and stop program
			printf("\n\nERROR - CANNOT DIVIDE BY 0\nExiting...\n\n");
			exit(1);
		}
		else
			return a / b;
	}
		
	/*** LOGICAL AND COMPARATOR OPERATORS ***/
	//LESS THAN
	if(currTok[0] == '<'){
		a = eval(toks, row, tokIndex, maxcol);
		b = eval(toks, row, tokIndex, maxcol);
		if(a < b)
			return 1.0;
		else
			return 0.0;
	}
	
	//GREATER THAN
	if(currTok[0] == '>'){
		a = eval(toks, row, tokIndex, maxcol);
		b = eval(toks, row, tokIndex, maxcol);
		if(a > b)
			return 1.0;
		else
			return 0.0;
	}
	
	//EQUAL
	if(currTok[0] == '='){
		a = eval(toks, row, tokIndex, maxcol);
		b = eval(toks, row, tokIndex, maxcol);
		if(a == b)
			return 1.0;
		else
			return 0.0;
	}
	
	//CONDITIONAL TERNARY OPERATORS
	if(currTok[0] == '?'){
		a = eval(toks, row, tokIndex, maxcol);
		b = eval(toks, row, tokIndex, maxcol);
		c = eval(toks, row, tokIndex, maxcol);
		
		if(a != 0)
			return b;
		else
			return c;
	}
	
	//AND
	if(currTok[0] == '&'){
		a = eval(toks, row, tokIndex, maxcol);
		b = eval(toks, row, tokIndex, maxcol);
		if(a != 0 && b != 0)
			return 1.0;
		else
			return 0.0;
	}
	
	//OR
	if(currTok[0] == '|'){
		a = eval(toks, row, tokIndex, maxcol);
		b = eval(toks, row, tokIndex, maxcol);
		if(a != 0 || b != 0)
			return 1.0;
		else
			return 0.0;
	}
	
	//NOT
	if(currTok[0] == '!'){
		a = eval(toks, row, tokIndex, maxcol);
		if(a == 0.0)
			return 1.0;
		else
			return 0.0;
	}
	
	/* token is invalid */
	printf("\n\nInvalid Token!\nExiting...\n\n");
	exit(1);
}

void tbl_print_tcalc(Table *t, FILE *stream, char *colnew, char **tokens, int *firstTok, int maxcol) {
	int i, j, k;
	double val;
	int counter = 0;
	if(t==NULL) {
		fprintf(stream, "# attempted to print non-existing table\n");
		return;
	}
	/* print out column labels */
	for(k = 0; k <= t->ncols; k++)
		fprintf(stream, "%10s:", t->colname[k]);
	/* print out new column */
	fprintf(stream, "%10s:\n", colnew);

	/* print out row data */	
	for(i = 0; i < t->nrows; i++){
		for(j = 0; j < t->ncols; j++) {
			/* print out row name */
			if(j == 0)
				fprintf(stream, "%10s:", t->rows[i].rowname);
			for(k = -1; k < counter; k++){
				fprintf(stream, " ");
			}
			fprintf(stream, "%10.2f", t->rows[i].vals[j]);
			
		}
		counter = 0;
		val = eval(tokens, t->rows[i], firstTok, maxcol);
		//print out result (call eval() function)
		fprintf(stream, " %10.2lf\n", val);
		//reset token index for next row
		*firstTok = -1;
	}
}

int main(int argc, char *argv[]){
	Table *t;
	char buf[MAX_LINE_LEN + 1];
	
	char **tokens = malloc(sizeof(char*) * MAX_LINE_LEN); 
	int i = 0, counter = -1;
	
	/* READING THE FIRST ARGUEMENT */
	FILE *pFile = fopen(argv[1], "r");
	if(pFile == 0){
		printf("Count not open file\n");
		exit(1);
	}
	else{
		//read entire line
		while(fgets(buf, MAX_LINE_LEN, pFile) != '\0'){
			//do not count lines that begin with '#'
			if(strlen(buf) != 0 || buf[0] == '#')
				continue;
		}
	}
	
	/* PARSING THE STRING */
	char buf2[MAX_LINE_LEN + 1]; 
	strcpy(buf2, buf);
	
	char *newCol = strtok(buf2, ":"); //to hold the new column name
	//trim the column name
	trim_string(newCol);
	char *expression = strtok(NULL, ":");

	//continue to read in the rest of the tokens separated by ' '
	char *toks = strtok(expression, " ");
	while(toks != NULL){
		tokens[i] = malloc(sizeof(char) * 3);
		strcpy(tokens[i], toks);
		toks = strtok(NULL, " ");
		i++;
	}
	
	/* check if formula can even be evaluated */
	int j = 0, ops = 0, vals = 0, needed = 0, tern = 0;
	for(j = 0; j < i; j++){
		/* check if negative number */
 		if(tokens[j][0] == '-'){
			if(isdigit(tokens[j][1])){
				vals++;
				if((vals % 2) == 0 && (vals / ops) != 2)
					needed--;
			}
			continue;
		} 
		
		/* check for operators */
		if(tokens[j][0] == '+' || tokens[j][0] == '*' ||
		   tokens[j][0] == '/' || tokens[j][0] == '<' ||
		   tokens[j][0] == '>' || tokens[j][0] == '=' ||
		   tokens[j][0] == '?' || tokens[j][0] == '&' ||
		   tokens[j][0] == '|' || tokens[j][0] == '!'){
			
			if(tokens[j][0] == '?'){
				needed++;
				tern++;
			}
			ops++;
			needed += 2;
			if(tokens[j][0] == '!'){
				needed--;
			}
		}
		
		/* check for values */
		if(tokens[j][0] == '$' || isdigit(tokens[j][0])){
			vals++;
			if((vals % 2) == 0 && (vals / ops) != 2)
				needed--;
		}
	} 

	if(needed != vals){
		printf("Invalid expression\nExiting...\n\n");
		exit(1);
	}
	t = tbl_from_stream(stdin);
	
	int firstTok = -1;
	
	if(t==NULL){
		fprintf(stderr, "Goodbye\n");
		return 0;
	}

	/* check to see if there are no tokens */
	if(i == 0){
		printf("There are no tokens in the expression!\n");
		exit(1);
	}
	else{
		/* print table with new column */
		int maxcols = t->ncols; //to hold number of columns
		printf("\n");
		tbl_print_tcalc(t, stdout, newCol, tokens, &firstTok, maxcols);
		printf("\n");
	}

	/* free tokens */
	while(i <= 0)
		free(tokens[i]);
	free(tokens);
	tbl_free(t);
}