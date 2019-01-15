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
int EXIT_FLAG=0;

int breakString(char **list,char *str,char *delim){
	int i=0;
	list[i]=strtok(str,delim);
	while(list[i]!=NULL){
		i++;
		list[i]=strtok(NULL,delim);
	}
	return i;
}

int findInput(char *input){
	for(int i=0;i<strlen(input);i++){
		if(input[i]=='<')
			return i;
	}
	return -1;
}

int findOutput(char *input){
	for(int i=0;i<strlen(input);i++){
		if(input[i]=='>')
			return i;
	}
	return -1;
}

void executeExternal(char *input){

	char *args[MAX];
	if(!strcmp(input,"exit")){		//If string is quit then quit
		printf("\nExiting from kgp-shell\n");
		EXIT_FLAG = 1;
		exit(0);
	}

	breakString(args,input," \n");

	execvp(args[0],args);			//execute the  other program
	printf("Error in executing the file! \n");
	kill(getpid(),SIGTERM);

}

void executeInputOutput(char *input){
	char *args[MAX];
	char *files[MAX];
	int i=0;

	int in=findInput(input);
	int out=findOutput(input);
	// printf("%d %d ",in,out);
	i=breakString(args,input,"<>\n");
	printf(" %d ",i);
	// printf("%s %s ",args[1],args[0]);

	pid_t p=fork();
	if(p==0){
		if(in==-1 && out==-1){
			if(i==1){
				executeExternal(args[0]);
			}
			else printf("Error encountered!\n");
		}
		else if(in>0 && out==-1){
			if(i==2){
				int f=breakString(files,args[1]," \n");
				// printf("%s %s %d\n",files[0],files[1],f);
				if(f!=1){
					printf("Input file error.");
					return;
				}
				int ifd = open(files[0], O_RDONLY);
				if(ifd<0){
					printf("Error when opening input file.\n");
					return;
				}
				close(0);
				dup(ifd);
				close(ifd);
			}
			else printf("Error!\n");
		}
		else if(in==-1 && out>0){
			if(i==2){
				int f=breakString(files,args[1]," \n");
				if(f!=1){
					printf("Output file error!");
					return;
				}
				int ofd = open(files[0], O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
				if(ofd<0){
					printf("Error when opening output file.\n");
					return;
				}
				close(1);
				dup(ofd);
				close(ofd);
			}
		}
		else if(in>0 && out>0 && i==3){
			int ifd,ofd,f;
			// printf("in out identified ");
			if(in>out){
				f=breakString(files,args[1]," \n");
				if(f!=1){
					printf("Error in output file\n");
					return;
				}
				ofd = open(files[0], O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

				f=breakString(files,args[2]," \n");
				if(f!=1){
					printf("Error in output file\n");
					return;
				}
				ifd = open(files[0], O_RDONLY);
			}
			else if(in<out){
				f=breakString(files,args[2]," \n");
				if(f!=1){
					printf("Error in output file\n");
					return;
				}
				printf("%s ",files[0]);
				ofd = open(files[0], O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

				f=breakString(files,args[1]," \n");
				if(f!=1){
					printf("Error in output file\n");
					return;
				}
				// printf("%s ",files[0]);
				ifd = open(files[0], O_RDONLY);				
			}
			close(0);
			dup(ifd);
			close(1);
			dup(ofd);
			close(ifd);
			close(ofd);
		}
		else return;
		executeExternal(args[0]);
	}
	else {
		wait(NULL);
		usleep(90000);
	}
	return;
	exit(0);
}

int main(int argc, char *argv[]){

	printf("Welcome to kgpsh :\n");

	while(1){

		char pwd[LEN]="",command[LEN]="",input[LEN]="",*ptr;

		ptr = getcwd(pwd, sizeof(pwd));		//Get current directory for printing in the bash
		if(ptr==NULL) {
			perror("getcwd ERROR ");
			continue;
		}
		printf("%s$ ", pwd);

		// getting command from user
		fgets (input, 1000, stdin);
		executeInputOutput(input);
		if(EXIT_FLAG==1)exit(0);
		printf("%d", EXIT_FLAG);
	}
}
