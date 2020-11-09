#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#define BGLIST "bglist"
#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0

typedef struct array{
	int end;
	char *cmd;
}Array;

typedef struct node{
	char* pname;
	pid_t pid;
	bool isHead;
	bool isRunning;
	struct node * next;
}Node;



Node* createNode(char* pname, pid_t pid, bool isHead, bool run){
	Node * node = (Node *)malloc(sizeof(Node));
	if(node == NULL){
		perror("memory not allocated");
		exit(EXIT_FAILURE);
	}
       	node->pname = pname;
	node->pid = pid;
	node->next = NULL;
	node->isRunning = run;
	node->isHead = isHead;
	return node;	
}

void deleteNode(){}

void appendNode(Node** current, Node** next){
	Node * temp = *current;
	while(temp->next != NULL){

		temp = temp->next;	
	}
	temp->next = *next; 
	return;

}
void printList(Node ** node){
	int i = 1;
	Node * temp = *node;
	while(temp != NULL){
	
		printf("i is %d\n", i);
		printf("the %d node pid is %d\n", i, temp->pid);
		i++;
		temp = temp->next;
		
		//sleep(5);
		
	
	}
	printf("outside of while loop in printList\n");
	return;

}

bool extraSpace(char *string){
	int count = 0;
	int length = strlen(string);
	for(int i = 0; i <length; i++){
		if(string[i] == ' '){
			count++;
		}
	}
	if (count >=2 ){
		return true;
	}
	return false;
	
}
char** split(char*input){
	char **arg = (char**)malloc(2*sizeof(char*));
	int i = 0;

	char *token;

	token = strtok(input, " ");
	while(token != NULL){
		arg[i] = (char*)malloc(strlen(token)*sizeof(char));
		if(token != NULL){
			arg[i] = token;
		}
		token = strtok(NULL, " ");
		i++;
	}
	return arg;
}
bool isValidProcess(char* input){
	char * procs[3] = {"./inf", "./foo", "./bar"};
	for (int i = 0; i < 3; i++){
		if (strcmp(procs[i], input)==0){
			return true;
		} 
	}
	return false;
}

Array* getProcess(char* process){
	Array *proc = (Array*)malloc(sizeof(Array));
	proc->cmd = process;
	proc->end= 0;
	return proc;
}

char** getProcessVect(char* process){
	char** procVect = (char**)malloc(2*sizeof(char*));
	procVect[0] = (char*)malloc(strlen(process)*sizeof(char));
	procVect[0] = process;
	procVect[1] = (char*)malloc(sizeof(char));
	procVect[1] = NULL;
	return procVect;
}

bool isValidPID(Node ** node, pid_t pid){
	Node * temp = *node;
	while (temp!= NULL){
		if (temp->pid == pid){
			return true;
		}
		temp = temp->next;
	}
	return false;
}

void destroyNode(Node**node, pid_t pid){
	Node * temp = *node;
	Node * next;
	Node* grandC;
	//its only the head and the pid matches
	if (temp->next == NULL && temp->pid == pid){
		temp->isRunning = false;
		temp->pid = 0;
		temp->pname = '\0';
		printf("Terminated pid: %d\n", pid);
		return;
	}
	// the head has the pid but also has a child or grandchild
	if(temp->pid == pid && temp->next != NULL){
		if(temp->next->next !=NULL){
			 grandC = temp->next->next;
			 next = temp->next;
			
			temp->pname = next->pname;
			temp->pid = next->pid;
			temp->next = grandC;
			next->next = NULL;
			free(next);
			printf("Terminated pid: %d\n", pid);
			return;
		}else{
			 next = temp->next;
			
			temp->pname = next->pname;
			temp->pid = next->pid;
			temp->next = NULL;
			free(next);
			printf("Terminated pid: %d\n", pid);
			return;
		}
	}
	// when the pid is somewhere in the linked list
	while (temp->pid != pid){
		// in the middle of the linked list somewhere
		if (temp->next->pid == pid){
			//the next node is has the pid and also points to the end
			
			if(temp->next->next == NULL){
				
				next = temp->next; 
				temp->next = NULL;
				free(next);
				printf("Terminated pid: %d\n", pid);
				return;
			}else{
				next = temp->next;
				grandC = temp->next->next;

				temp->next = grandC;
				next->next = NULL;
				free(next);
				printf("Terminated pid: %d\n", pid);
				return;
			
			}

		} 
		temp = temp->next;
	}	
}
//check if a node is executing
bool isStopped(Node **node, pid_t pid){

	Node * temp = *node;
	if(temp->pid == pid){
		if(temp->isRunning){
			return true;
		}else{
			return false;
	
		}
	}	
	while(temp->pid != pid){
	
		if (temp->next->pid == pid){
			if(temp->next->isRunning){
				return true;
			}else{
				return false;
			}
		}
	temp = temp->next;
	}


}
void pauseNode(Node** node, pid_t pid){
	Node * temp = *node;
      	while(temp != NULL){
		if (temp->pid == pid){
			temp->isRunning = false;
			printf("Process with pid: %d has been stopped\n", pid);
			return;
		}

	temp = temp->next;
	}
}
void contNode(Node ** node, pid_t pid){

	Node * temp = *node;
	while(temp != NULL){
		if (temp->pid == pid){
			temp->isRunning = true;
			printf("Process with pid: %d has been resumed\n", pid);
			return;
		}
	
	temp = temp->next;
	}


}
int isIn(int i, int vals[]){
	int j = 0;
	for(j; j<5; j++){
		if(i == vals[j]){
			return j;
		}
	
	}
	return 0;
}

void checkZombieProcess();


void getPstat(Node** node, char* pid);
void bgSignal(char* signal, Node** node, pid_t pid);

void startBG(char ** array, Node** node);
void bgList(Node ** node);
int main(){
	Node *head = createNode('\0', 0, true, false);
	int sig;
	while (1){
		checkZombieProcess(&head);
			char* cmd = readline("PMan: >");
			int cmdleng = strlen(cmd);
		if(extraSpace(cmd)){
			printf("%s: command not found\n", cmd);
			continue;
		}
		char** result = split(cmd);
		if (strcmp(result[0], "bglist")==0){
			bgList(&head);
		
		} else if(strcmp(result[0], "bg")==0){
			if(!(isValidProcess(result[1]))){
				printf("%s: program not found\n", result[1]);
				}
			startBG(getProcessVect(result[1]), &head);
		} else if (strcmp(result[0], "bgkill")==0 || strcmp(result[0], "bgstop") ==0 || strcmp(result[0], "bgstart") == 0){
			checkZombieProcess(&head);
			if (cmdleng == 6 || cmdleng==7){	
				printf("Error: enter a valid pid\n");
				continue;
							
			}else if ((strcmp(result[0], "bgstop") ==0 && cmdleng == 8) ||(strcmp(result[0], "bgkill")==0 && cmdleng==8 )){	
				sig = atoi(result[1]);
				if(sig)
				printf("Error: Enter a valid pid\n");
				continue;	
			
				
				
			} else if(strcmp(result[0], "bgstart") == 0 && cmdleng == 8){
			
				printf("Error: Enter a valid pi\n");
				continue;
			}
			
			
			sig = atoi(result[1]);	
	
			bgSignal(result[0], &head, sig);
		
		}else if(strcmp(result[0], "pstat")==0){
			if(cmdleng==5 || cmdleng == 6){
				printf("Error: Enter a valid pid\n");
				continue;
			}
			sig = atoi(result[1]);	
			if(!(isValidPID(&head, sig))){
				printf("Error: process %s does not exist\n", result[1]);
				continue;
			}
				
			getPstat(&head, result[1]);
		}else{
		
			printf("%s: command not found\n", cmd);
		
		}
	
		checkZombieProcess(&head);
	}
	return 0;
	
	
}


	
void getPstat(Node** node, char* pid){
	char *proc = "/proc/";
	char* stat = "/stat";
	char* status = "/status";
	const size_t N = 1024;
	char procdir[N];
	char dirtostatus[N];
	char dirtostat[N];

	snprintf(procdir, sizeof(procdir), "%s%s", proc, pid);	
	snprintf(dirtostat, sizeof(dirtostat), "%s%s%s", proc, pid, stat);
	snprintf(dirtostatus, sizeof(dirtostatus), "%s%s%s", proc, pid, status);

	
	FILE* pfile;
	char buff[1000];
	
	
	DIR *dir = opendir(procdir);
	struct dirent *dp;
	if (dir == NULL){
		perror("cannot open");
		return;
		exit(1);
	}
	
	dp = readdir(dir);
	int i = 0;
	FILE *file;
	char * infoStat[] = {"comm", "state", "utime", "stime", "rss"};
	int val[] = {1, 2, 13, 14, 23};
	pfile = fopen(dirtostat, "r");
	while(fscanf(pfile, "%s", buff)>0){
		if(isIn(i, val)|| i-1 == 0){
			printf("%s : %s\n", infoStat[isIn(i, val)], buff);	
		}
		i++;
		buff[0] = '\0';
	}
	fclose(pfile);
	file = fopen(dirtostatus, "r");
	i = 0;
	while(fgets(buff, sizeof(buff), pfile)){
		if(i == 52){
				printf("%s", buff);
		}else if ( i == 53){
				printf("%s\n", buff);
		}
		i++;
		
	}

	
	fclose(file);
	closedir(dir);
}


void bgSignal(char* signal, Node** node, pid_t pid){
	int retVal;
	int retWait;
	int status;
	if(!(isValidPID(node, pid))){
		printf("%d: pid not found\n", pid);
		return;
	}
	
	if(strcmp(signal, "bgkill") ==0){
		while(1){
			retVal = kill(pid, SIGTERM);
			retWait = waitpid(pid, &status, WNOHANG);			
			usleep(1000);
			if (retWait == -1){
				perror("wait failed:");
				return;
			}
			
			if (WIFSIGNALED(status)){
				destroyNode(node, pid);
				break;	
			}
				
		}	
	
	} else if (strcmp(signal, "bgstop") == 0){
		while(1){
			
			if(!(isStopped(node, pid))){
				break;
			}
			retVal = kill(pid, SIGSTOP);
			retWait = waitpid(pid, &status, WUNTRACED);			
			usleep(1000);
			if (retWait == -1){
				perror("wait failed:");
				return;

			}
			
			if (WIFSTOPPED(status)){
				pauseNode(node, pid);
				break;	
			}		
		}	
	
	} else if (strcmp(signal, "bgstart") == 0){
	
		while(1){
			if((isStopped(node, pid))){
			break;
			}
			retVal = kill(pid, SIGCONT);
			retWait = waitpid(pid, &status, WCONTINUED);			
			usleep(1000);
			if (retWait == -1){
				perror("wait failed:");
				return;

			}
			if (WIFCONTINUED(status)){
				contNode(node, pid);
				break;	
			}
			
		}	
	} else{
	
		printf("%s: command not found\n",signal);
	}
	
}

void bgList(Node** node){		
	char path[100];
	if(getcwd(path, sizeof(path)) != NULL){
		int jobs = 0;
		Node * temp = *node;
		while(temp != NULL){
			if(!(temp->isRunning)){
				temp = temp->next;
				continue;
			}
			jobs++;
			printf("%d: %s/", temp->pid, path);
			for(int i = 2; i < strlen(temp->pname); i++){
				printf("%c", (temp->pname)[i]);
				
			}
			printf("\n");
			temp = temp->next;
	
		}
		printf("Total background jobs: %d\n", jobs);
		return;
	}else{
		perror("getcwd() errpor");
	}


}

void startBG(char**array, Node** node){
	pid_t pid;
	pid = fork();
	if(pid == 0){
		if(execvp(array[0], array) < 0){
			perror("Error on ececvp");

		}
		exit(EXIT_SUCCESS);
	
	}else if(pid > 0){
		if(!((*node)->isRunning)){
			(*node)->pname = array[0];
			(*node)->pid = pid;
			(*node)->isRunning = true;
			printf("Created process: %d\n", pid);
			
		}else{
			Node* child = createNode(array[0], pid, false, true);
			appendNode(node, &child);
			printf("Created process: %d\n", pid);
			
		}	
		//store info of the bg child process
	}else{
		perror("fork failed");
		exit(EXIT_FAILURE);
	}
	
}

void checkZombieProcess(Node** node){
	int status;
	int retVal = 0;
	while(1){
		usleep(1000);
		if(!((*node)->isRunning)){
			return;
		}
		retVal = waitpid(-1, &status, WNOHANG);
		if(retVal > 0){
			printf("Removing zombieprocess: %d\n", retVal);
			destroyNode(node, retVal);
		}
		else if(retVal == 0){
			break;
		}
		else{
			perror("waitpid failed");
			exit(1);
		}
	
	}
	return;

}
