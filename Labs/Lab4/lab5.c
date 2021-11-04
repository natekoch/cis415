//
// Created by Nate Koch on 10/29/21.
//

#include <signal.h>
#include <stdio.h>
#include <unistd.h>

int main(void) {
    sigset_t sigset;
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGUSR1);
    sigprocmask(SIG_BLOCK, &sigset, NULL);

    printf(" Pid %d\n", getpid());
    int sig;
    int result = sigwait(&sigset, &sig);
    if (result == 0)
        printf("sigwait got signal: %d\n", sig);

    return 0;
}
