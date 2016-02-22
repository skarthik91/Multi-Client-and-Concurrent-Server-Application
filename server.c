#include	"unpthread.h"
#include	<time.h>
static void	*tim(void *);        //Time thread
static void	*doit(void *);      //Echo thread
int main(int argc, char **argv)
{
    int			listenfd_time, *iptr_time;
    int			listenfd_echo, *iptr_echo;
    pthread_t		tid_echo;
    pthread_t		tid_time;
    socklen_t		clilen;
    const int 		on = 1;
    const int 		on1 =1;
    struct sockaddr_in	cliaddr, servaddr_echo,servaddr_time;
    int			maxfdp1;
    fd_set			rset;

    listenfd_echo = Socket(AF_INET, SOCK_STREAM, 0);   //Listening on echo socket
    listenfd_time = Socket(AF_INET, SOCK_STREAM, 0);   //Listening on time socket
    
    bzero(&servaddr_echo, sizeof(servaddr_echo));		//Setting up the echo socket for listening on port number 62354
    servaddr_echo.sin_family      = AF_INET;
    servaddr_echo.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr_echo.sin_port        = htons(62354);
    Setsockopt(listenfd_echo, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    
    
    bzero(&servaddr_time, sizeof(servaddr_time));
    servaddr_time.sin_family      = AF_INET;
    servaddr_time.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr_time.sin_port        = htons(62353);	// Setting up the time socket for listening on port number 62353
    Setsockopt(listenfd_time, SOL_SOCKET, SO_REUSEADDR, &on1, sizeof(on1));
    
    Bind(listenfd_echo, (SA *) &servaddr_echo, sizeof(servaddr_echo));
    Bind(listenfd_time, (SA *) &servaddr_time, sizeof(servaddr_time));
    
    Listen(listenfd_echo, LISTENQ);
    Listen(listenfd_time, LISTENQ);
    
    FD_ZERO(&rset);
    
    for ( ; ; ) {
        FD_SET(listenfd_echo, &rset);
        FD_SET(listenfd_time, &rset);
        maxfdp1 = max(listenfd_time, listenfd_echo) + 1;
        Select(maxfdp1, &rset, NULL, NULL, NULL);
        
        if (FD_ISSET(listenfd_echo, &rset)) /* socket is echo */
        {	
            
            
            clilen = sizeof(cliaddr);
            
            iptr_echo = Malloc(sizeof(int));
            *iptr_echo = Accept(listenfd_echo, (SA *) &cliaddr, &clilen);
            Pthread_create(&tid_echo, NULL, &doit, iptr_echo); //Creating echo thread.
            
        }
        
        if (FD_ISSET(listenfd_time, &rset))	/* socket is time server socket */
        { 
            
            iptr_time = Malloc(sizeof(int));
            *iptr_time = Accept(listenfd_time, (SA *) NULL, NULL);
            Pthread_create(&tid_time, NULL, &tim, iptr_time); //Creating thread for time.
            
        }
    }
}

static void *doit(void *arg) //Thread for echo server
{
    int connfd;
    char recv[MAXLINE];
    
    connfd = *((int *) arg);
    free(arg);
    
    Pthread_detach(pthread_self());  //Detaching the echo thread
    str_echo(connfd);
    
    if (Readline(connfd, recv, MAXLINE) == 0)  //Checking for EOF.
    {
        fprintf(stdout, "\n Echo client terminated \n"); //Print termination message to server
        
        fflush(stdout);
        close(connfd);
        return;
    }			
    Close(connfd);			
    return(NULL);
}

static void *tim(void *arg) // Thread for time server
{
    int connfd;
    char buff[MAXLINE];
    time_t	ticks;
    fd_set	rset;
    int	maxfdp1;
    struct timeval tv;
    char recv[MAXLINE];
    connfd = *((int *) arg);
    free(arg);
    
    
    FD_ZERO(&rset);
    
    tv.tv_sec = 5;  //Intializing the timeout value as 5s.
    tv.tv_usec = 0;

    Pthread_detach(pthread_self()); //Detaching the time thread
    while(1){
        ticks = time(NULL);
        snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks));
        Write(connfd, buff, strlen(buff));
        maxfdp1 = connfd+ 1;
        FD_SET(connfd, &rset);
        Select(maxfdp1, &rset, NULL, NULL,&tv); //Implementing the select function to print the time every 5 seconds
        
        if (FD_ISSET(connfd, &rset)) 
        {
            if (Readline(connfd, recv, MAXLINE) == 0) //Checking for EOF.
            {
                fprintf(stdout, "\n Time client terminated \n"); //Print termination message to server
                
                fflush(stdout);
                close(connfd);
                return;
            }	
            
            
            
        }
        
    }
    
    Close(connfd);
    return (NULL);
    
}
