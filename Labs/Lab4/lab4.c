#include<stdio.h>
#include <sys/types.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>

void script_print (pid_t* pid_ary, int size);

int main(int argc,char*argv[])
{
	if (argc == 1)
	{
		printf ("Wrong number of arguments\n");
		exit (0);
	}
    printf(" Process id %d\n", getpid());
    fflush(stdout);
    /*
    // step 1
    pid_t child = fork();
    if (child == 0) {
        printf("hello this is the child.");
    }
    */

    // step 2

    /*
    int n = atoi(argv[1]);
    pid_t* pid_array; // process pool
    //n = atoi(arg[1]);
    pid_array = malloc(sizeof(pid_t)*n);
    pid_array[0] = getpid();

    char* argList[] = {"./iobound", "-seconds", "10", NULL};
    for (int i = 1; i < n; i++) {
        if ((pid_array[i] = execvp("./iobound", argList)) == -1) {
            perror("Execvp: ");
        }
    }

    script_print(pid_array, n);
    //pause();



    free(pid_array);
     */
    exit(0);

	/*
	*	TODO
	*	#1	declare child process pool
	*	#2 	spawn n new processes
	*		first create the argument needed for the processes
	*		for example "./iobound -seconds 10"
	*	#3	call script_print
	*	#4	wait for children processes to finish -waitpid?
	*	#5	free any dynamic memories
	*/

	//return 0;
}


void script_print (pid_t* pid_ary, int size)
{
	FILE* fout;
	fout = fopen ("top_script.sh", "w");
	fprintf(fout, "#!/bin/bash\ntop");
	for (int i = 0; i < size; i++)
	{
		fprintf(fout, " -p %d", (int)(pid_ary[i]));
	}
	fprintf(fout, "\n");
	fclose (fout);

	char* top_arg[] = {"gnome-terminal", "--", "bash", "top_script.sh", NULL};
	pid_t top_pid;

	top_pid = fork();
	{
		if (top_pid == 0)
		{
			if(execvp(top_arg[0], top_arg) == -1)
			{
				perror ("top command: ");
			}
			exit(0);
		}
	}
}


