//Shaan Barkat
//CSC374 - Assignment 4
/*-------------------------------------------------------------------------*
 *---                                                                   ---*
 *---           subPatServer.c                                          ---*
 *---                                                                   ---*
 *---       This file defines a C program that gets file-sys commands   ---*
 *---   from client via a socket, executes those commands in their own  ---*
 *---   threads, and returns the corresponding output back to the       ---*
 *---   client.                                                         ---*
 *---                                                                   ---*
 *---   ----    ----    ----    ----    ----    ----    ----    ----    ---*
 *---                                                                   ---*
 *---   Version 1a              2018 November 8         Joseph Phillips ---*
 *---                                                                   ---*
 *-------------------------------------------------------------------------*/

//      Compile with:
//      $ gcc subPatServer.c -o subPatServer -lpthread

//---           Header file inclusion                                   ---//

#include        "subPat.h"
#include        <pthread.h>     // For pthread_create()


//---           Definition of constants:                                ---//

#define         STD_OKAY_MSG            "Okay"

#define         STD_ERROR_MSG           "Error doing operation"

#define         STD_BYE_MSG             "Good bye!"

#define         THIS_PROGRAM_NAME       "subPatServer"

#define         OUTPUT_FILENAME         "out.txt"

#define         ERROR_FILENAME          "err.txt"

const int       ERROR_FD                = -1;


//---           Definition of global vars:                              ---//

//  PURPOSE:  To be non-zero for as long as this program should run, or '0'
//      otherwise.

extern void* handleClient(void* vPtr);
//extern int getServerFileDescriptor();

//---           Definition of functions:                                ---//

//  YOUR CODE HERE



//  PURPOSE:  To run the server by 'accept()'-ing client requests from
//      'listenFd' and doing them.
void            doServer        (int            listenFd
                                )
{
  //  I.  Application validity check:

  //  II.  Server clients:
  pthread_t             threadId;
  pthread_attr_t        threadAttr;
  int                   threadCount     = 0;

  pthread_attr_init(&threadAttr);
  pthread_attr_setdetachstate(&threadAttr, PTHREAD_CREATE_DETACHED);

  while  (1)
  {
        int sockets = accept(listenFd, NULL, NULL);
        int *intArray = (int*)malloc(2*sizeof(int)); //  YOUR CODE HERE
        intArray[0] = sockets;
        intArray[1] = threadCount++;
        pthread_create(&threadId, &threadAttr, handleClient,(void*)intArray);
        pthread_join(threadId, NULL);

  }

   pthread_attr_destroy(&threadAttr);


  //  III.  Finished:
}

void* handleClient(void* vPtr)
{
   int* intArray  = (int*)vPtr;
   int  clientFd  = intArray[0];
   int  threadNum = intArray[1];
   free(intArray);
   printf("Thread %d starting. \n", threadNum);


   // II.B. Read command:
   char buffer[BUFFER_LEN];
   char command;
   int fileNum;
   char text[BUFFER_LEN];
   char text2[BUFFER_LEN];
   int shouldContinue = 1;

   while(shouldContinue)
   {

        text[0] = '\0';
        text2[0] = '\0';

        read(clientFd, buffer, BUFFER_LEN);
        printf("Thread %d recieved: %s\n", threadNum, buffer);
        sscanf(buffer, "%c %d \"%[^\"]\" \"%[^\"]\"",&command,&fileNum,text,text2);

        if(command==DIR_CMD_CHAR)
        {
                DIR* dirPtr = opendir(".");
                struct dirent* entryPtr;
				if((dirPtr = opendir(".")) == NULL) {
                {
                        write(clientFd,STD_ERROR_MSG, sizeof(STD_ERROR_MSG));
                }

                if(dirPtr)
                {
                        while((entryPtr = readdir(dirPtr)) != NULL)
                        {
                                buffer[0] = '\0';
                                int i;
                                int sizebuf = sizeof(buffer);

                                for(i=0;i<sizebuf;i++)
                                {
                                        strcat(buffer,entryPtr->d_name);
                                        strcat(buffer,"\n");
                                }
                        }
                }
                closedir(dirPtr);
                }
        }

        else if(command == READ_CMD_CHAR)
        {
                fileNum = open(FILENAME_EXTENSION,O_RDONLY,0);
                if(fileNum<0)
                {
                        write(fileNum,STD_ERROR_MSG,sizeof(STD_ERROR_MSG));
                        exit(1);
                }
                char fileName[BUFFER_LEN];
                snprintf(fileName,BUFFER_LEN,"%d%s",fileNum,FILENAME_EXTENSION);

                fileNum = read(fileNum, fileName, BUFFER_LEN-1);
                fileName[fileNum] = '\0';
                printf("sending %s", fileName);
                write(fileNum, fileName, fileNum);
                close(fileNum);
        }

        else if(command == WRITE_CMD_CHAR)
        {
                fileNum = open(FILENAME_EXTENSION,O_WRONLY|O_CREAT|O_TRUNC,0660);

                if(fileNum<0)
                {
                        write(fileNum,STD_ERROR_MSG,sizeof(STD_ERROR_MSG));
                }

                if(fileNum>0)
                {
                        char fileName[BUFFER_LEN];
                        write(fileNum,fileName,sizeof(fileName));
                        write(fileNum,STD_OKAY_MSG,sizeof(STD_OKAY_MSG));
                }

                close(fileNum);
        }
		else if(command == DELETE_CMD_CHAR)
        {
                if(fileNum > 0)
                {
                        fileNum = unlink(buffer);
                        write(clientFd,STD_OKAY_MSG,sizeof(STD_ERROR_MSG));
                }
                else
                        write(clientFd,STD_ERROR_MSG,sizeof(STD_ERROR_MSG));
        }

        else if(command == QUIT_CMD_CHAR)
        {
                write(clientFd,STD_BYE_MSG,sizeof(STD_BYE_MSG));
                shouldContinue = 0;
        }
        }
        printf("Thread %d quitting.\n",threadNum);
        return(NULL);
    }



//  PURPOSE:  To decide a port number, either from the command line arguments
//      'argc' and 'argv[]', or by asking the user.  Returns port number.
int             getPortNum      (int    argc,
                                 char*  argv[]
                                )
{
  //  I.  Application validity check:

  //  II.  Get listening socket:
  int   portNum;

  if  (argc >= 2)
    portNum     = strtol(argv[1],NULL,0);
  else
  {
    char        buffer[BUFFER_LEN];

    printf("Port number to monopolize? ");
    fgets(buffer,BUFFER_LEN,stdin);
    portNum     = strtol(buffer,NULL,0);
  }

  //  III.  Finished:
  return(portNum);
}


//  PURPOSE:  To attempt to create and return a file-descriptor for listening
//      to the OS telling this server when a client process has connect()-ed
//      to 'port'.  Returns that file-descriptor, or 'ERROR_FD' on failure.
int             getServerFileDescriptor
                                (int            port
                                )
{
  //  I.  Application validity check:

  //  II.  Attempt to get socket file descriptor and bind it to 'port':
  //  II.A.  Create a socket
  //  II.A.  Create a socket
  int socketDescriptor = socket(AF_INET, // AF_INET domain
                                SOCK_STREAM, // Reliable TCP
                                0);

  if  (socketDescriptor < 0)
  {
    perror(THIS_PROGRAM_NAME);
    return(ERROR_FD);
  }

  //  II.B.  Attempt to bind 'socketDescriptor' to 'port':
  //  II.B.1.  We'll fill in this datastruct
  struct sockaddr_in socketInfo;

  //  II.B.2.  Fill socketInfo with 0's
  memset(&socketInfo,'\0',sizeof(socketInfo));

  //  II.B.3.  Use TCP/IP:
  socketInfo.sin_family = AF_INET;

  //  II.B.4.  Tell port in network endian with htons()
  socketInfo.sin_port = htons(port);

  //  II.B.5.  Allow machine to connect to this service
  socketInfo.sin_addr.s_addr = INADDR_ANY;

  //  II.B.6.  Try to bind socket with port and other specifications
  int status = bind(socketDescriptor, // from socket()
                    (struct sockaddr*)&socketInfo,
                    sizeof(socketInfo)
                   );

  if  (status < 0)
  {
    perror(THIS_PROGRAM_NAME);
    return(ERROR_FD);
  }

  //  II.B.6.  Set OS queue length:
  listen(socketDescriptor,5);

  //  III.  Finished:
  return(socketDescriptor);
}


int             main            (int    argc,
                                 char*  argv[]
                                )
{
  //  I.  Application validity check:

  //  II.  Do server:
  int         port      = getPortNum(argc,argv);
  int         listenFd  = getServerFileDescriptor(port);
  int         status    = EXIT_FAILURE;
  if  (listenFd >= 0)
  {
    doServer(listenFd);
    close(listenFd);
    status      = EXIT_SUCCESS;
  }

  //  III.  Finished:
  return(status);
}




