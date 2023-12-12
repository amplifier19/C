#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int AttrtoInt(char *line, int len, int *pos, char delimiter);
int GetData(char **array, char **remain, int r, int c, FILE *f);
int GetWords(char **words, int w, FILE *f);
void bsort(char **word_array, int words);
void PrintMatrix (char **char_array, int r, int c);
int FindWords(char **char_array, char **rem_letters, char *word, int xstart, int ystart, char opx, char opy);
void FreePointers (char **array, char **remain, int r, char **dict, int w, FILE *f);


int main(int argc, char *argv[])
{
	int i, j, rowlen, wordlen, rows, columns, words, linelen;
	char **array;
	char **remain;
	char **dict;
	char *line = NULL;
	size_t len = 0;
	
// Έλεγχος των παραμέτρων της γραμμής ενολών
	if(argc!=2)
	{
		printf("Χρήση: ./ask <Αρχείο>\n");
		exit(-1);
	}

// Άνοιγμα Αρχείου
	FILE *fp;
	fp = fopen(argv[1], "r");
	if (fp == NULL)
	{
		printf("Σφάλμα κατά το άνοιγμα αρχείου.\n");
		fclose(fp);
		exit(-1);
	}

// Διάβασμα πρώτης γραμμής αρχείου
	linelen = getline(&line, &len, fp);
	if (linelen  == -1)
	{
		printf("Άδειο αρχείο εισόδου.\n");
		FreePointers (NULL, NULL, 0, NULL, 0, fp);
		exit(-1);
	}

// Αποθήκευση πλήθους γραμμών κρυπτόλεξου
	i = 0;
	rows = AttrtoInt(line, linelen, &i, ',');
	if ( rows == -1)
	{
		printf("Μη αποδεκτή μορφή αριθμού γραμμών.\n");
		FreePointers (NULL, NULL, 0, NULL, 0, fp);
		exit(-1);
		
	}
		
// Αποθήκευση πλήθους στηλών κρυπτόλεξου
	i++;
	columns = AttrtoInt(line, linelen, &i, '\n');
	if ( columns == -1)
	{
		printf("Μη αποδεκτή μορφή αριθμού στηλών.\n");
		FreePointers (NULL, NULL, 0, NULL, 0, fp);
		exit(-1);
	
	}

// Αντιγραφή του κρυπτόλεξου από το αρχείο εισόδου στον πίνακα array[][] 
	array = (char **)malloc(rows*sizeof(char *));
	remain = (char **)malloc(rows*sizeof(char *));
	if (!GetData(array, remain, rows, columns, fp))
	{
		printf("Μη αποδεκτή μορφή κρυπτόλεξου.\n");
		FreePointers (array, remain, rows, NULL, 0, fp);
		exit(-1);
	}

// Αποθήκευση τους πλήθους των κρυμμένων λέξεων
	linelen = getline(&line, &len, fp);
	if (linelen == -1)
	{
		printf("Δεν περιέχεται ο αριθμός των κρυμμένων λέξεων.\n");
		FreePointers (array, remain, rows, NULL, 0, fp);
		exit(-1);
	}
	
	i = 0;
	words = AttrtoInt(line, linelen, &i, '\n');
	if (words == -1)
	{
		printf("Μη αποδεκτή μορφή αριθμού κρυμμένων λέξεων.\n");
		FreePointers (array, remain, rows, NULL, 0, fp);
		exit(-1);
	}

// Αντιγραφή των κρυμμένων λέξεων από το αρχείο εισόδου στο πίνακα dict[][]	
	dict = (char **)malloc(words*sizeof(char *));
	if (!GetWords(dict, words, fp))
	{
		printf("Μη αποδεκτή μορφή κρυμμένων λέξεων ή πλήθος.\n");
		FreePointers (array, remain, rows, dict, words, fp);
		exit(-1);
	}
	
// Λεξικογραφική ταξινόμηση του πίνακα κρυμμένων λέξεων dict[][]
	bsort(dict, words);

// Τύπωση πίνακα-κρυπτόλεξου array[][]
	printf("\n");
	printf("Πίνακας Κρυπτόλεξου\n  ");
	PrintMatrix (array, rows, columns);


// Έλεγχος πιθανών κατευθύνσεων αναζήτησης και εύρεση κρυμμένων λέξεων προς αυτές 
	int w, r, c, n;
	int rowup, colup, low;
	for (w=0; w<words; w++)
	{
		rowup = rows - strlen(dict[w]);
		colup = columns - strlen(dict[w]);
		low = strlen(dict[w]) - 1;
		
		for (r=0; r<rows; r++)
		{
			for (c=0; c<columns; c++)
			{	
				if (array[r][c] == dict[w][0])
				{	
					if (c <= colup)
					{
						// Οριζόντια προς τα Δεξιά
						n = FindWords(array, remain, dict[w], r, c, ' ', '+');					
						if (n == strlen(dict[w]))
						{
							printf("Λέξη: %s, Μεταξύ (%d,%d)-(%d,%d), Κατεύθυνση: Οριζόντια προς τα Δεξιά\n", dict[w], r+1, c+1, r+1, c+n);
						}
					}
					if (c >= low)
					{
						// Οριζόντια προς τα Αριστερά
						n = FindWords(array, remain, dict[w], r, c, ' ', '-');						
						if (n == strlen(dict[w]))
						{
							printf("Λέξη: %s, Μεταξύ (%d,%d)-(%d,%d), Κατεύθυνση: Οριζόντια προς τα Αριστερά\n", dict[w], r+1, c+1, r+1, c+2-n);
						}
						if (r <= rowup)
						{
							// Δευτερεύον Διαγώνιος προς τα Κάτω
							n = FindWords(array, remain, dict[w], r, c, '+', '-');							
							if (n == strlen(dict[w]))
							{
								printf("Λέξη: %s, Μεταξύ (%d,%d)-(%d,%d), Κατεύθυνση: Δευτερεύον Διαγώνιος προς τα Κάτω\n", dict[w], r+1, c+1, r+n, c-n+2);
							}
						}
					}
					if (r <= rowup) 
					{
						// Κάθετα προς τα Κάτω
						n = FindWords(array, remain, dict[w], r, c, '+', ' ');						
						if (n == strlen(dict[w]))
						{
							printf("Λέξη: %s, Μεταξύ (%d,%d)-(%d,%d), Κατεύθυνση: Κάθετα προς τα Κάτω\n", dict[w], r+1, c+1, r+n, c+1);
						}
						if (c <= colup)
						{
							// Κύρια Διαγώνιος προς τα Κάτω	
							n = FindWords(array, remain, dict[w], r, c, '+', '+');
							if (n == strlen(dict[w]))
							{
								printf("Λέξη: %s, Μεταξύ (%d,%d)-(%d,%d), Κατεύθυνση: Κύρια Διαγώνιος προς τα Κάτω\n", dict[w], r+1, c+1, r+n+1, c+n);
							}
						}
					} 
					if (r >= low)
					{
						// Κάθετα προς τα Πάνω
						n = FindWords(array, remain, dict[w], r, c, '-', ' ');
						if (n == strlen(dict[w]))
						{
							printf("Λέξη: %s, Μεταξύ (%d,%d)-(%d,%d), Κατεύθυνση: Κάθετα προς τα Πάνω\n", dict[w], r+1, c+1, r+2-n, c+1);
						}			
						if (c >= low)
						{
							// Κύρια Διαγώνιος προς τα Πάνω
							n = FindWords(array, remain, dict[w], r, c, '-', '-');
							if (n == strlen(dict[w]))
							{
								printf("Λέξη: %s, Μεταξύ (%d,%d)-(%d,%d), Κατεύθυνση: Κύρια Διαγώνιος προς τα Πάνω\n", dict[w], r+1, c+1, r-n+2, c-n+2);
							}
						}
						if (c <= colup)
						{
							// Δευτερεύον Διαγώνιος προς τα Πάνω
							n = FindWords(array, remain, dict[w], r, c, '-', '+');
							if (n == strlen(dict[w]))
							{
								printf("Λέξη: %s, Μεταξύ (%d,%d)-(%d,%d), Κατεύθυνση: Δευτερεύον Διαγώνιος προς τα Πάνω\n", dict[w], r+1, c+1, r-n+2, c+n);
							}	
						}
					}
				}
			}
		}
	}
	
// Τύπωση πίνακα Εναπομεινάντων Γραμμάτων remain[][]
	printf("\n");
	printf("Πίνακας Εναπομεινάντων Γραμμάτων\n  ");
	PrintMatrix (remain, rows, columns);
	
// Αποδέσμευση μνήμης
	FreePointers (array, remain, rows, dict, words, fp);
}

int AttrtoInt(char *line, int len, int *pos, char delimiter)
{
	int number, digit;
	number = 0;
	if (delimiter == ',')
		len = len-3;
	do
	{
		if (line[*pos] < '0' || line[*pos] > '9')
			return -1;
			
		digit = line[*pos] - '0';
		number = number*10 + digit;
		*pos = *pos + 1;
	} while (*pos<len && line[*pos]!= delimiter);
	if (line[*pos] != delimiter )
		return -1;
	return number;
}

int GetData(char **array, char **remain, int r, int c, FILE *f)
{
	int i, j, rowlen;
	char line[128];
	size_t len;
	i = 0;
	while (i<r)
	{
		rowlen = getline(&line, &len, f);
		if(rowlen-1 != c)
			return 0;
		
		array[i] = (char *)malloc(c);
		memcpy(array[i], line, c);
		for(j=0; j<rowlen-1; j++)
		{
			if (array[i][j]<'A' || array[i][j]>'Z')
			{
				return 0;
			}
		}
		remain[i] = (char *)malloc(c);
		memcpy(remain[i], line, c);
		i++;
	}
	return 1;
}

int GetWords(char **words, int w, FILE *f)
{
	int i, j, wordlen;
	size_t len;
	char line[128];
	i = 0;
	wordlen = getline(&line, &len, f);
	while (wordlen != -1 && i<w)
	{
		words[i] = (char *)malloc(wordlen);
		memcpy(words[i], line, wordlen-1);
		words[i][wordlen-1] = '\0';
		for (j=0; j<wordlen-1; j++)
		{
			if (words[i][j]<'A' || words[i][j]>'Z')
				return 0;
		}
		wordlen = getline(&line, &len, f);
		i++;
	}
	if (i != w)
		return 0;
	return 1;
}
void bsort(char **word_array, int words)
{
	int i, j;
	char *tmp;
	for (i=0; i<words-1; i++)
	{
		for (j=0; j<words-i-1; j++)
		{
			if (strcmp(word_array[j], word_array[j+1]) > 0)
			{
				tmp = (char *)malloc(strlen(word_array[j]));
				strcpy(tmp, word_array[j]);
				strcpy(word_array[j], word_array[j+1]);
				strcpy(word_array[j+1], tmp);
				free(tmp);
			}	
		}
	}
}

void PrintMatrix (char **char_array, int r, int c)
{
	int i, j;
	for(j=0; j<c && j<10; j++)
	{
		printf("%d ", j+1);
	}
	for(i=0; i<r; i++)
	{
		printf("\n");
		if (i<9) 
			printf("%d ", i+1);
		else
			printf("  ");
		for(j=0; j<c; j++)
		{
			printf("%c ", char_array[i][j]);
		}
	}
	printf("\n\n");
}

int FindWords(char **char_array, char **rem_letters, char *word, int xstart, int ystart, char opx, char opy)
{
	int n, x, y;
	n = 0;
	x = xstart;
	y = ystart;
	while (n < strlen(word) && char_array[x][y] == word[n])
	{
		n++;
		switch (opx)
		{
			case '-':
				x--;
				break;
			case '+':
				x++;
				break;			
		}
		switch (opy)
		{
			case '-':
				y--;
				break;
			case '+':
				y++;
				break;
		}
	}
	if (n == strlen(word))
	{
		int i;
		x = xstart;
		y = ystart;
		for (i=0; i<n; i++)
		{
			rem_letters[x][y] = ' ';
			switch (opx)
			{
				case '-':
					x--;
					break;
				case '+':
					x++;
					break;			
			}
			switch (opy)
			{
				case '-':
					y--;
					break;
				case '+':
					y++;
					break;
			}
		}
		 
	}
	return n;
}

void FreePointers (char **array, char **remain, int r, char **dict, int w, FILE *f)
{
	int i;
	if (f !=NULL)
	{
		fclose(f);
	}
	if (array != NULL)
	{
		for(i=0; i<r; i++)
		{
			if (array[i] != NULL)
				free(array[i]);
		}
		free(array);			
	}
	if (remain != NULL)
	{
		for(i=0; i<r; i++)
		{
			if (remain[i] != NULL)
				free(remain[i]);
		}
		free(remain);			
	}
	if (dict != NULL)
	{
		for(i=0; i<w; i++)
		{
			if (dict[i] != NULL)
				free(dict[i]);
		}
		free(dict);			
	}
}

