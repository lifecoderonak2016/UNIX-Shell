// Ronak Kumar (2015080)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <ctype.h>
#include <signal.h>
#include <stdbool.h>
#include <sys/dir.h>
#include <sys/param.h>
#define TRUE 1
#define FALSE 0
char pathDirectory[100];
void welcomeScreen()
{
	printf("\n===============================================================================================================================\n");
    printf("\t               	      			Welcome to My UNIX Shell\n");
    printf("===============================================================================================================================\n");
    printf("\n");
}
void terminate_function(int number)
{
	signal(number, SIG_IGN);
	fflush(stdout);
}
void help_function()
{
	FILE* fpointer = fopen("help.txt", "r");
	int ch;
	if(fpointer)
	{
		while((ch = fgetc(fpointer)) != EOF)
		{
			printf("%c", ch);
		}
		fclose(fpointer);
	}
}
void clear_output()
{
	pid_t pid = fork();
	if(pid == 0)
	{
		char *cmd = "clear";
		char *argv[2];
		argv[0] = "clear";
		argv[1] = NULL;
		int var = execvp(cmd, argv);
		exit(var);
	}
	else
	{
		wait(NULL);
	}	
}
int file_select(struct direct *entry)
{
    if((strcmp(entry->d_name, ".") == 0) || (strcmp(entry->d_name, "..") == 0))
        return FALSE;
    else
        return TRUE;
}
void change_directory_function(char tokenize[100][100])
{
	if(chdir(tokenize[1]) < 0)
	{
		printf("%s is not a valid directory\n", tokenize[1]);
		return;
	}
	else
	{
		char* aux[100];
		pid_t pid = fork();
		if(pid == 0)
		{
			aux[0] = "pwd";
			aux[1] = NULL;
			printf("The directory has been changed to:\n");
			int var = execvp(aux[0], aux);
			exit(var);
		}
		else
			wait(NULL);
	}
}
void history_write(char* arr)
{
	FILE* fpointer = fopen("historyfile.txt", "a");
	if(fpointer != NULL)
	{
		fprintf(fpointer, "%s", arr);
		fprintf(fpointer, "\n");
		fclose(fpointer);
	}	
}
void history_read()
{
	FILE* fpointer = fopen("historyfile.txt", "r");
	int ch;
	if(fpointer)
	{
		while((ch = getc(fpointer)) != EOF)
		{
			printf("%c", ch);
		}
		fclose(fpointer);
	}
}
bool checkPipe(char* input)
{
	for(int i = 0; i < strlen(input); i++)
	{
		if(input[i] == '|')
			return TRUE;
	}
	return FALSE;
}
void split(char input[], char tokenize[100][100])
{
  	char *pch;
  	pch = strtok(input, " ");
  	int i = 0;
  	int j = 0;
  	int k = 0;
  	while(pch != NULL)
  	{
    	j = 0;
    	for(i = 0; i < strlen(pch); i++)
    	{
    		tokenize[k][j++] = pch[i];
    	}
    	k++;
		pch = strtok(NULL, " ");
  	}
  	tokenize[k][j] = '\0';
}
void pwd()
{
	char cwd[1000];
	if(getcwd(cwd, sizeof(cwd)) != NULL)
	    fprintf(stdout, "%s\n", cwd);
	else
	    perror("Not able to display current directory");
}
void file_write(char a[], char b[])
{
	FILE* fpointer = fopen(b, "w");
	if(fpointer != NULL)
	{
		fprintf(fpointer, "%s", a);
		fprintf(fpointer, "\n");
		fclose(fpointer);
	}
}
void func(){}
void file_read(char b[])
{
	FILE* fpointer = fopen(b, "r");
	int c;
	int tot_lines = 0;
	int tot_chars = 0;
	int tot_words = 0;
	int last = '\n';
	func();
	int in_space = 1;
	func();
	if(fpointer)
	{
		while((c = getc(fpointer)) != EOF)
		{
			last = c;
       		tot_chars++;
        	if(isspace(c)) 
        	{
            	in_space = 1;
            	if(c == '\n') 
                	tot_lines++;
            	func();
        	}	 
        	else 
        	{
            	func();
            	tot_words += in_space;
            	in_space = 0;
        	}
		}
		if(last != '\n') 
        	tot_lines++;
    	func();
		printf("%d %d %d\n", tot_lines, tot_words, tot_chars);
		fclose(fpointer);
	}
}
extern int alphasort();
void list_files()
{
	struct direct **files;
	if(getcwd(pathDirectory, sizeof(pathDirectory)) == NULL)
    	perror("Pathname error\n");
	int number_of_files = scandir(pathDirectory, &files, file_select, alphasort);
	if(number_of_files <= 0)
    	perror("No files in this directory\n");
    for(int i = 1; i < number_of_files+1; i++)
        printf("%s\n", files[i-1]->d_name);
}
int main(int argc, char *argv, char **envp)
{
	signal(SIGINT, terminate_function);
	welcomeScreen();
	char input[100];
	pid_t pid = -1;
	int aux[2];
	while(TRUE)
	{
		char tokenize[100][100];
		char auxillary[100];
		printf("Command>> ");
		fgets(input, 100, stdin);
		strcpy(auxillary, input);
		auxillary[strlen(auxillary)-1] = '\0';
		input[strlen(input)-1] = '\0';
		history_write(input);
		for(int i = 0; i < 100; i++)
		{
			for(int j = 0; j < 100; j++)
			{
				tokenize[i][j] = '\0';
			}
		}
		split(input, tokenize);
		int flag = FALSE;
		int value = -1;
		int pos;
		if(pid = fork() == 0)
		{
			if(checkPipe(input) == TRUE)
			{
				FILE *f;
				extern FILE *popen();
				f = popen(input, "w");
				fclose(f);
				exit(0);
			}
			else
			{
				if(!strcmp(tokenize[0], "clear"))
					clear_output();
				else if(!strcmp(input, "kill"))
					goto LABEL;
				else if(!strcmp(tokenize[0], "history"))
					history_read();
				else if(!strcmp(tokenize[0], "cd"))
					change_directory_function(tokenize);
				else if(!strcmp(tokenize[0], "pwd"))
					pwd();
				else if(!strcmp(tokenize[0], "help"))
					help_function();
				else if(!strcmp(tokenize[0], "ls"))
					list_files();
				else
				{
					for(int i = 0; i < strlen(auxillary); i++)
					{
						if(auxillary[i] == '>')
						{
							flag = TRUE;
							value = 1;
							pos = i;
						}
						else if(auxillary[i] == '<')
						{
							flag = TRUE;
							value = 0;
							pos = i;
						}
					}
					if(flag == TRUE)
					{
						char a[50], b[50];
						if(value == 1)
						{
							int j = 0;
							int i = 6;
							for(int i = 6; auxillary[i] != '\"'; i++)
							{
								a[j++] = auxillary[i];
							}
							a[j] = '\0';
							int k = 0;
							for(int i = pos+2; auxillary[i] != '.'; i++)
							{
								b[k++] = auxillary[i];
							}
							b[k++] = '.'; b[k++] = 't'; b[k++] = 'x'; b[k++] = 't';
							file_write(a, b);
						}
						else if(value == 0) 
						{
							int k = 0;
							for(int i = 5; auxillary[i] != '\"'; i++)
							{
								b[k++] = auxillary[i];
							}
							b[k] = '\0';
							file_read(b);
						}
					}
				}
			}
		}	
		else
		{
			wait(NULL);
			LABEL: return 0;
		}	
	}
	return 0;
}