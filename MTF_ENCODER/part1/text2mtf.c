/*
Jasper Hale
SENG 265
V00888481
Assignment 1

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define BUFLEN 100
#define MAX_WORD_LEN 20
#define MAX_WORDS 120
#define MAX_LINES 50
int num_words = 0;//used for the this_line array. Counts how many words there are for a certian line
int num_lines = 0;// used for the structure array. 


char this_line[MAX_LINES][MAX_WORD_LEN]; // 2-D array that is used to store words at a specefic line
			// 50		20
char structure[MAX_WORDS][MAX_WORD_LEN]; // 2-D array that is the data structure that encodes the words and outputes them to the mtf file
			// 120		20

void make_structure(char *certain_word, FILE** thefile);

void tokenize_line (char *input_line)// function takes in one line of the txt file and puts each word on its own column 
{
	
	char *t;
	t = strtok (input_line, " ");
	while (t != NULL && num_words < MAX_WORDS) //100
	{
	
		strncpy (this_line[num_words], t, MAX_WORD_LEN);
		
		num_words++;
		
		t = strtok (NULL, " ");
		
		
	}
	
	int j = 0;
	while( j < MAX_WORD_LEN)//used for words at the end of line that contain \n. gets rid of the \n by replacing it with \0. therefore no new line will be added 
	{
		
		if(this_line[num_words-1][j]=='\n')
		{
			
			this_line[num_words-1][j]='\0';
			
			break;
		}
		
		j++;
	}
	
	return;
}




void make_structure(char *certain_word, FILE** thefile){	
	int i = 0;
	int cnt = 0;
	int j = 0;
	int m = 0;
	int p = 0;
	int char_len = strlen(certain_word);
	
	if(strlen(certain_word)==0)//if statement for blank spaces. Don't want to add them into the structure
	{
		return;
	}
	if(strlen(structure[0])==0){//used for storing the first word into structure
		
		strncpy(structure[0], certain_word, MAX_WORD_LEN);
		num_lines++;
		fputc(num_lines+128, *thefile);
		while(p < char_len)//copy every letter 
		{
		fputc(structure[0][p], *thefile);
		p++;
		}
		
		return;
	}else{
		
		while(i<num_lines){	//need to go through all the words in the array
		
			if(!(strcmp(structure[i], certain_word))){	
			cnt++;
			
				j = i;
				while (j>0){//moves everything down a spot in the array structure
					strncpy(structure[j], structure[j-1], MAX_WORD_LEN);
					j--;
					
				}
				
				strncpy(structure[0], certain_word, MAX_WORD_LEN);
				

				fputc(cnt+128,*thefile);
				
				return;
			}else{
				i++;
				cnt++;
				
			}
			
		}
		
		while (i>0){//once out of loop that compares to character arrays, you need to move everything down and add the input to the top of the array
			strncpy(structure[i], structure[i-1], MAX_WORD_LEN);
			i--;
					
			
		}
		strncpy(structure[0], certain_word, MAX_WORD_LEN);
		num_lines++;
		fputc(num_lines+128, *thefile);
		while(m < char_len)
		{
		fputc(structure[0][m], *thefile);
		m++;
		}
		return;
		
	}
		
	
	
}
	





int main(int argc, char *argv[]) 
{	
	char buffer[BUFLEN];
	
	
	if (argc < 2) 
	{
		fprintf(stderr, "You must provide a filename\n");
		exit(1);
	}

	FILE *data_fp = fopen(argv[1], "r");
	int length = strlen(argv[1]);
	char file[80];
	strncpy(file, argv[1], length-4);
	strncat(file, ".mtf", 5);
	
	FILE *new_file = fopen(file, "w");
	fputc(186, new_file);
	fputc(94, new_file);
	fputc(186, new_file);
	fputc(17, new_file);
	if (data_fp == NULL) 
	{
		fprintf(stderr, "unable to open %s\n", argv[1]);
		exit(1);
	}
	num_lines = 0;
	while (fgets(buffer, sizeof(char) * BUFLEN, data_fp)) 
	{
		tokenize_line(buffer);
	
		for(int i =0; i<num_words; i++)//puts a line that has been tokenized through to organize it in make structure
		{
		
			make_structure(this_line[i], &new_file);
		
		}
		
		for(int x =0; x<=num_words; x++)//resets this_line array. that way a new line can be passed without any complications 
		{
		
			this_line[x][0]='\0';
			
		
		}	
			num_words = 0;
		
		fputc('\n', new_file);	
			
	}
	fclose(data_fp); 
	fclose(new_file);
	
	
	

	return 0;
}