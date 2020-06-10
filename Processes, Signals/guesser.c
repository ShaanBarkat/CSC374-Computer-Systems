//guesser.c
//Shaan Barkat

#include "assign2Headers.h"

int shouldRun = 1;

void  timeOverHandler(int sig, siginfo_t* infoPtr, void* dataPtr)
{
         printf("Oh no!  The time is up!");
         shouldRun=0;
}

void  winHandler(int sig, siginfo_t* infoPtr, void* dataPtr)
{
         printf("Congratulations!  You found it!\n");
         shouldRun=0;
}

void  correctHandler(int sig, siginfo_t* infoPtr, void* dataPtr)
{
        printf("Yay!  That was right!\n");
}

void  incorrectHandler(int sig, siginfo_t* infoPtr, void* dataPtr)
{
        printf("Oops!  That was wrong.  Please restart from the beginning.\n");
        shouldRun=0;
}


int main (int argc, char* argv[]){
        struct sigaction x;
        int guess;

        memset(&x,'\0',sizeof(x));
        x.sa_flags = SA_SIGINFO | SA_RESTART;
        x.sa_sigaction = timeOverHandler;
        sigaction(TIME_OVER_SIGNAL,&x,NULL);

        x.sa_flags = SA_SIGINFO | SA_RESTART;;
        x.sa_sigaction = winHandler;
        sigaction(WIN_SIGNAL,&x,NULL);

        x.sa_flags = SA_SIGINFO | SA_RESTART;;
        x.sa_sigaction = correctHandler;
        sigaction(CORRECT_SIGNAL,&x,NULL);

        x.sa_flags = SA_SIGINFO | SA_RESTART;;
        x.sa_sigaction = incorrectHandler;
        sigaction(INCORRECT_SIGNAL,&x,NULL);


        pid_t pid = strtol(argv[1],NULL,10);

        while(shouldRun){
                printf("What would you like your next guess to be: 0 or 1? : ");
                scanf("%d", &guess);
                if (guess == 0) {
                        kill (pid, ZERO_SIGNAL);
                        sleep(1);
				} else if (guess == 1){
                        kill (pid, ONE_SIGNAL);
                        sleep(1);
                } else {}
                sleep(1);
        }

        printf("guesser finished\n");
        return(EXIT_SUCCESS);
}

