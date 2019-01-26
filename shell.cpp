/*
Group : 6
Name : Aurghya Maiti (16CS10059)
Name : Aditya Rastogi (16CS30042)
*/
#include<bits/stdc++.h>
#include<sys/wait.h>
#include<unistd.h>
#include<fcntl.h>
#include<errno.h>
#include<dirent.h>
#include<sys/stat.h>
#include<time.h>
#include<grp.h>
#include<pwd.h>

#define LEN 1000
#define MAX 100
using namespace std;

void executeExternal(char *input){

	//for all commands and arguments
	char *args[MAX];

	//get all args
	int i=0;
	args[i]=strtok(input," \n");		//Read the filename and individual arguments
	while(args[i]!=NULL){
		// printf("%s",args[i]);
		i++;
		args[i]=strtok(NULL," \n");
	}

	//fork this process to run the command in the child process
	pid_t p=fork();
	
	if(p<0){
		//fork error
		printf("Terminal process could not be created\n");
	}
	else if(p==0){
		//child process
		execvp(args[0],args);			//execute the  other program
		
		//if execvp doesn't work
		printf("Enter a valid program! \n");
		kill(getpid(),SIGTERM);
	}
	else {
		//wait for the child process
		wait(NULL);
	}
}

void executePipe(char** args, int N, int backFlag){
	//define N-1 pipes
	int p[N-1][2];

	for(int i=0;i<N-1;i++){
		//create pipe
		if(pipe(p[i])<0){
			printf("Pipe creation failed!");
			return;
		}	
	}

	//create N child processes
	
	for(int i=0;i<N;i++) 
    { 
        if(fork() == 0) 
        { 
        	//this is the child process

        	//read from the previous pipe if it exists
        	if(i!=0){
        		//it doesn't write to the previous pipe
        		close(p[i-1][1]);	

        		//redirects STDIN to read end of the previous pipe
        		dup2(p[i-1][0], 0); 
        	}
        	
        	//write to the next pipe if it exists
        	if(i!=N-1){
        		//it doesn't read from the next pipe
        		close(p[i][0]);		

        		//redirects STDOUT to write end of the next pipe
        		dup2(p[i][1], 1);	
        	}

        	//execute the i-th command
        	executeExternal(args[i]);

        	//exit from the child process	
            exit(0); 
        } 
        else{
        	//sleep
    		usleep(100000);
        }
    } 

	if(backFlag==0)
    	wait(NULL);     
}

int main(int argc, char *argv[]){

	//print welcome string in green colour
	cout<<"\033[1;32mWelcome to kgpsh :\033[0m\n";

	while(1){

		char pwd[LEN]="";
		char command[LEN]="";
		char input[LEN]="";
		char *ptr;

		//Get current directory for printing in the bash
		ptr = getcwd(pwd, sizeof(pwd));		

		//error in getcwd
		if(ptr==NULL) {
			perror("getcwd error");
			continue;
		}
		//printf("%s$ ", pwd);

		//print present working directory in blue colour
		cout<<"\033[1;34m"<<pwd<<"\033[0m"<<"$ ";

		//get command from user
		fgets(input, 1000, stdin);	

		if(strlen(input)>=4){
			if(input[0]=='e' && input[1]=='x' && input[2]=='i' && input[3]=='t'){
				int k = 4;
				while(input[k]==' '){
					k++;
				}
				if(input[k]=='\n'){
					printf("Exiting from kgp-shell...\n");	
					exit(0);
				}
			}
		}

		//for the pipe commands' arguments
		char *args[MAX];
		int i=0;

		//checking the existence of pipes in the entered command

		//separating individual commands by |
		args[i]=strtok(input,"|");
	
		while(args[i]!=NULL){
			i++;
			args[i]=strtok(NULL,"|");
		}

		// regex b("(.)*&( )*\n");

		int backFlag = 0;

		int j;
		for(int k=0;input[k]!='\0';k++){
			if(input[k]=='&'){
				j = k;
				j++;
				while(input[j]==' '){
					j++;
				}
				if(input[j]=='\n'){
					backFlag = 1;
					input[k++] = '\n';
					input[k] = '\0';
				}
			}
		}

		executePipe(args, i, backFlag);
	}
}
