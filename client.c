
#include <stdio.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include "unp.h"

int main(int argc, char *argv[])
{
    int i;
    char query_string[20]; 
    struct hostent *he;
    struct hostent *hostip;
    struct in_addr **addr_list;
    struct in_addr addr;
    pid_t childpid, pid;
    int stat;
    int pfd[2];
    char buf[1024];
    char ip_buf[1024];
    char fd_write[4];
    char fd_read[4];
    int nread;
    int fd[2];
    char choice;
    
    
    
    if(argc <2)
    {
        printf("Please provide an IP address or Domain Name");
        exit(1);
    }
    
    if (inet_aton(argv[1],&addr)!= 0) 
        
    {
        // IP address has been passed from the command line.
        
        
        he = gethostbyaddr((const char*)&addr,sizeof(addr),AF_INET); //gethostbyaddr resolved the IP to a domain name
        if(he==NULL)
            printf("Invalid hostname \n");
        printf("\ n The hostname has been resolved as :%s\n",he->h_name); // The hostname is printed.
        
        
    }
    
    else
    {
        //Domain name has been passed from the command line.
        
        he = gethostbyname(argv[1]);
        
        if (he == NULL)
        { 
            herror("gethostbyname"); // Error checking
            exit(1);
        }
        
        
        printf("Official name is: %s\n", he->h_name); //Details of the host are printed out.
        printf("IP address: %s\n", inet_ntoa(*(struct in_addr*)he->h_addr));
        printf("All alias addresses: \n ");
        addr_list = (struct in_addr **)he->h_addr_list;
        
        
        for(i = 0; addr_list[i] != NULL; i++) // Alias addresses are printed out.
        {
            printf("%s \n", inet_ntoa(*addr_list[i]));
        }
        printf("\n");
        
        
        
    }
    for(; ;) //Client polls infinitely as to which service is required
        
    {
        printf("\n Which service would you request.Enter the number of your choice - \n'1.time' \n'2 .echo '  \n'3. quit' \n");
        scanf(" %c",&choice);
        if(choice== '3') // Choice chosen was quit
            exit(0);
        
        else if(choice== '2') // Service chosen is echo.
        {
            printf("\n Service requested is echo \n");
            
            if (pipe(pfd) == -1) // Create a pipe for IPC
            {
                perror("pipe failed"); //Check for errors while creating a pipe.
                exit(1);
            }
            
            
            
            
            printf("forking echo child \n");
            
            if((childpid = fork())< 0) // A child is forked to handle the echo client process.
            {
                perror("fork failed \n");
                exit(2);
            }
            if (childpid == 0) // Running the child process.
            {
                close(pfd[0]); //Close the read end of the pipe.

                sprintf(fd_write,"%d",pfd[1]); //Converting the file descriptor to string to pass it in exec function
                
                if ( (execlp("xterm", "xterm", "-e", "./echo_cli","127.0.0.1",fd_write,(char *) 0)) < 0) //exec to handle the echo process
                
                {
                    strcpy(buf, "error in xterm - Echo"); //Incase of error, print the error message.
                    write(pfd[1], buf,strlen(buf)+1);
                    close(pfd[1]);
                    exit(1);
                }
            }
            
            if(childpid>0) // If the process is a parent.
            {
                close(pfd[1]); // Close the write end of the pipe.
                
                
                while((nread =read(pfd[0], buf, 1024)!=0))
                    printf(" \n Message from the child is :      %s\n", buf);
                
                pid = wait(&stat); // waiting for the child process to terminate
                
                close(pfd[0]); // Close the reading end of the pipe.
                printf("Echo child terminated \n");
                continue;  // Continue polling for service requested. Go back to the for loop.
            }
            
            
        }
        
        else if(choice== '1')  // If the service requested is time.
        {
            printf("\n Service requested is time \n"); // Create a pipe for IPC
            if (pipe(pfd) == -1)
            {
                perror("pipe failed"); //Check for errors while creating a pipe.
                exit(1);
            }
            
            
            
            
            printf("\n forking time child\n");
            
            if((childpid = fork())< 0) // A child is forked to handle the time client process.
            {
                perror("fork failed");
                exit(2);
            }
            if (childpid == 0) 
            {	close(pfd[0]); // Running the child process. Close the read end of the pipe
                
                
                sprintf(fd_write,"%d",pfd[1]);
                if ( (execlp("xterm", "xterm", "-e", "./time_cli","127.0.0.1",fd_write, (char *) 0)) < 0) // Pass the IP and file descriptor of pipe.
                    
                {
                    strcpy(buf, "error in xterm - time");
                    write(pfd[1], buf,strlen(buf)+1);
                    close(pfd[1]);
                    exit(1);
                }
            }
            
            if(childpid>0) 
            {
                close(pfd[1]); //Running the parent process. Close the write end of the pipe.
                while((nread =read(pfd[0], buf, 1024)!=0))
                    printf("\n Message from the child:  %s\n", buf);
                
                pid = wait(&stat);  // waiting for the child process to terminate
                close(pfd[0]);  // Close read end of the pipe.
                printf("Time child terminated \n");
                continue; // Continue polling for service requested. Go back to the for loop.
            }
            
            
        }
        else {
            printf("Invalid choice\n");  //Wrong choice entered
        }
    }
}
