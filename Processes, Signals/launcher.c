//launcher.c
//Shaan Barkat

#include <signal.h>
#include <string.h>
#include <stdio.h>
#include "assign2Headers.h"


pid_t guesserPid;
pid_t answererPid;
int shouldRun = 1;

//initialize global variables


void  INThandler1(int sig, siginfo_t* infoPtr, void* dataPtr)
{
        kill (answererPid, TIME_OVER_SIGNAL);
        kill (guesserPid, TIME_OVER_SIGNAL);
        shouldRun=0;

}
void  INThandler2(int sig, siginfo_t* infoPtr, void* dataPtr)
{
        int a;
        int pid;
        while( (pid=waitpid(1,&a,WNOHANG)) > 0)
                {}
        shouldRun=0;
}

int     main (int argc, char* argv[]){

        // SIGALRM
        struct sigaction sgalarm;
        memset(&sgalarm,'\0', sizeof(struct sigaction));
        sigemptyset(&sgalarm.sa_mask);
        sgalarm.sa_flags = SA_SIGINFO | SA_RESTART;
        sgalarm.sa_sigaction = INThandler1;
        sigaction(SIGALRM, &sgalarm, NULL);
        alarm(NUM_SECONDS);

        // SIGCHLD
        struct sigaction sgchild;
        memset(&sgchild,'\0', sizeof(struct sigaction));
        sigemptyset(&sgchild.sa_mask);
        sgchild.sa_flags = SA_SIGINFO | SA_RESTART;
        sgchild.sa_sigaction = INThandler2;
        sigaction(SIGCHLD, &sgchild, NULL);


        char    line[LINE_LEN];
        guesserPid = fork();
        answererPid = fork();

        if (answererPid == 0){
                execl("./answerer", ANSWERER_PROGNAME, (char*) NULL);

        }


        if (guesserPid == 0){
                snprintf(line,LINE_LEN,"%d",answererPid);
                execl("./guesser", GUESSER_PROGNAME, line, (char*) NULL);
        }



        while  (shouldRun){
                sleep(1);
        }
        sleep(1);
        sleep(1);

        printf("launcher finished\n");
        return(EXIT_SUCCESS);
}
