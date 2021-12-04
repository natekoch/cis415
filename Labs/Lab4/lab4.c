#include<stdio.h>
#include <sys/types.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>

void script_print (pid_t* pid_ary, int size);

int main(int argc,char*argv[]) {
    /* --- task 1 --- */
    /*
    pid_t child = fork();
    if (child == 0) {
        printf("hello this is the child.");
    }
    */

    /* --- task 2 --- */
    /*
    char* argList[] = {"./iobound", "-seconds", "10", NULL};
    if (execvp("./iobound", argList) == -1) {
        perror("Execvp: ");
    }
    */

    /* --- tasks 3 & 4 --- */
    if (argc == 1) {
        printf("Wrong number of arguments\n");
        exit(0);
    }

    int n = atoi(argv[1]);
    // process pool
    pid_t *pid_array;
    pid_array = malloc(sizeof(pid_t) * n);

    char *argList[] = {"./iobound", "-seconds", "10", NULL};
    for (int i = 0; i < n; i++) {
        pid_array[i] = fork();
        if (pid_array[i] < 0) {
            perror("fork");
        if (pid_array[i] == 0) {
            if (execvp("./iobound", argList) == -1) {
                perror("execvp");
            }
            exit(-1);
    }


    /* --- task 5 --- */
    script_print(pid_array, n);

    for (int i = 0; i < n; i++) {
        waitpid(pid_array[i], NULL, 0);
    }

    free(pid_array);
    exit(0);
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


