#include	"unp.h"



//Echo client implementation



void	*copyto(void *);
void str_cli(FILE *, int); //str_cli function implemented using threads

static int	sockfd;		
static FILE	*fp;


int main(int argc, char **argv)
{
    int					sockfd;
    struct sockaddr_in	servaddr;
    int fd = atoi(argv[2]);  //Converting the pipe descriptors to int
    
    
    
    
    if (argc != 3)
	{	 write(fd," Usage: tcpcli <IPaddress>",40); //Error message is sent to the parent.
        err_quit("usage: tcpcli <IPaddress>"); 
    }
    sockfd = Socket(AF_INET, SOCK_STREAM, 0); //Creating a socket
    
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(62354);
    Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
    
    Connect(sockfd, (SA *) &servaddr, sizeof(servaddr)); //Connecting to the server socket
    
    write(fd," Starting echo process",40); //write to the parent
    close(fd);
    
    str_cli(stdin, sockfd);		// Calling str_cli function to execute the echo process
    close(sockfd);
    exit(0);
}


void str_cli(FILE *fp_arg, int sockfd_arg) 		// Implemetation of str_cli function
{
    char		recvline[MAXLINE];
    pthread_t	tid;
    
    sockfd = sockfd_arg;	/* copy arguments to externals */
    fp = fp_arg;
    
    Pthread_create(&tid, NULL, copyto, NULL);  //Create the thread for str_cli
    
    while (Readline(sockfd, recvline, MAXLINE) > 0) 
        Fputs(recvline, stdout);
}

void *copyto(void *arg)
{
    char	sendline[MAXLINE];
    
    while (Fgets(sendline, MAXLINE, fp) != NULL)
        Writen(sockfd, sendline, strlen(sendline));
    
    Shutdown(sockfd, SHUT_WR);	/* EOF on stdin, send FIN */
    
    return(NULL);
    /* return (i.e., thread terminates) when EOF on stdin */
}
