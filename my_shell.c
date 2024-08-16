#include  <stdio.h>
#include  <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

#define MAX_INPUT_SIZE 1024
#define MAX_TOKEN_SIZE 64
#define MAX_NUM_TOKENS 64

/* Splits the string by space and returns the array of tokens
*
*/

int fg_child;
int procs[69];
int procount = 0;
char **tokenize(char *line)
{
  char **tokens = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
  char *token = (char *)malloc(MAX_TOKEN_SIZE * sizeof(char));
  int i, tokenIndex = 0, tokenNo = 0;

  for(i =0; i < strlen(line); i++){

    char readChar = line[i];

    if (readChar == ' ' || readChar == '\n' || readChar == '\t'){
      token[tokenIndex] = '\0';
      if (tokenIndex != 0){
	tokens[tokenNo] = (char*)malloc(MAX_TOKEN_SIZE*sizeof(char));
	strcpy(tokens[tokenNo++], token);
	tokenIndex = 0; 
      }
    } else {
      token[tokenIndex++] = readChar;
    }
  }
 
  free(token);
  tokens[tokenNo] = NULL ;
  return tokens;
  
}
//bg process checking
int checkAmpersand(char** args)
	{
		for (int i = 0; args[i] != NULL; i++) 
		{
			if (strcmp(args[i], "&") == 0) 
			{
				
				return i;
			}
		}

		return -1;
	}
//exit function handling
void exitfn(char **tokens)
	{
		for(int i = 0; i < procount; i++)
		{
			
				kill(procs[i],SIGKILL);
				waitpid(procs[i], NULL, 0);
			
		}
		
		exit(0);
	}
//signal handling forground process here	
void sig_handler(int sign){
	if(sign == SIGINT && fg_child == getpid()){
		kill(getpid(), SIGKILL);
		}
	}
	
int main(int argc, char* argv[]) {
	char  line[MAX_INPUT_SIZE];            
	char  **tokens;              
	int i,status;
	pid_t bg_proc;


	while(1) {			
		//pid_t x = waitpid(bg_proc, &status, WNOHANG);
		 //for printing bg pid after reaping
		//  if(tokens != NULL){
		// 		for(i=0;tokens[i]!=NULL;i++){
		// 		free(tokens[i]);
		// 		}
		// 		free(tokens);

			 //	}
//for bg process finished
			for(int i = 0; i < procount; i++)
			{
				pid_t bg_res = waitpid(procs[i], &status, WNOHANG);
				if (bg_res == procs[i]){
				printf("Shell: Background process(%d) finished \n",procs[i]);
				}



			}
		
				//if (x == bg_proc)
				//{
				//	printf("Shell: Background process finished\n");
				//}
		/* BEGIN: TAKING INPUT */
		bzero(line, sizeof(line));
		printf("$ ");
		scanf("%[^\n]", line);
		getchar();

		//printf("Command entered: %s (remove this debug output later)\n", line);
		/* END: TAKING INPUT */



		line[strlen(line)] = '\n'; //terminate with new line
		tokens = tokenize(line);
   
       //do whatever you want with the commands, here we just print them

		//for(i=0;tokens[i]!=NULL;i++){
		//	printf("found token %s (remove this debug output later)\n", tokens[i]);
		//}
		
		
		
		if(tokens[0] == NULL) 
			continue;
		//for exit commmand
		if(strcmp(tokens[0], "exit") == 0){	
			for(i=0;tokens[i]!=NULL;i++){
			free(tokens[i]);
		}
		free(tokens);
		exitfn(tokens);
		continue;
		}
		//for cd command
		
			// if(strcmp(tokens[1], "..") == 0)
			// {
			//     chdir("..");
			//     continue;
			// }
			// // else if(tokens[1] == NULL && tokens[2] == NULL){
			// //     printf("empty path!!!!\n");
			   
			// // }
			// else if(chdir(tokens[i]) != 0){
			//     printf("Shell: Incorrect command\n");
			// }
			// else if (tokens[1] != NULL && tokens[2] == NULL)
			// {
			// 	chdir(tokens[1]);
			// }
			// else
			// {
			// 	printf("empty path!!!!\n");
			// }
			// continue;
	//for cd command imp
			if(strcmp(tokens[0], "cd") == 0){
			if(tokens[1] == NULL && tokens[2] == NULL)
			{
				printf("Empty path.\n");
				continue;
			}
			else if (tokens[1] != NULL && tokens[2] == NULL)
			{
				chdir(tokens[1]);
				continue;
			}
			else
			{
				printf("Shell: Incorrect command \n");
				continue;
			}


		}
		//for bg processes
		int ampersandPresent = checkAmpersand(tokens);
		if(ampersandPresent > 0)
		{
			tokens[ampersandPresent] = NULL;
			bg_proc = fork();
			setpgid(0,0);
			//int status;
			if(bg_proc < 0)
				fprintf(stderr,"%s\n", "Unable to create child process!!!\n");
			else if(bg_proc  == 0){
				//setpgid(0,0);
				//sleep(5);
				execvp(tokens[0], tokens);
				printf("Shell: Incorrect command \n");
				_exit(1);
				}
			else
			{
			// 	int wt_bg;
			//  do{
			//  	wt_bg = waitpid(bg_proc, NULL, WNOHANG);
			//  	//sleep(1);
			//    }while(wt_bg == 0);
				procs[procount++]= bg_proc;
				pid_t x = waitpid(bg_proc, NULL , WNOHANG);
				//if (x == bg_proc)
				//{
				 //	printf(" Background finished");
				// }
				
			}
		}
		

			
		
		 
		 //for fg process
		else if(ampersandPresent < 0) 
		{
			signal(SIGINT, sig_handler);
			int fg_proc = fork();
			if(fg_proc < 0)
				fprintf(stderr,"%s\n", "Unable to create child process!!!\n");
			else if(fg_proc == 0){
				fg_child = getpid();
				//setpgid(0,0);
				//sleep(15);
				execvp(tokens[0], tokens);
				printf("exec execution failed!!!\n");

				_exit(1);
				}
			else {
				waitpid(fg_proc, NULL, WUNTRACED);
				}
			
		}
		//WRITE FOR AMPERSANDPRESENT=0 ERROR	
       	else {
       	  printf("erorr done\n");
       	}
		// Freeing the allocated memory	
		for(i=0;tokens[i]!=NULL;i++){
			free(tokens[i]);
		}
		free(tokens);

	}
	return 0;
}
