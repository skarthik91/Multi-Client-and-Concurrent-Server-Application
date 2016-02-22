#include	"unp.h"


// Time client implementation

int main(int argc, char **argv)
{
    int					sockfd, n;
    char				recvline[MAXLINE + 1];
    struct sockaddr_in	servaddr;
    int fd;
    
    if (argc != 3)
        err_quit("usage: a.out <IPaddress>");

    fd = atoi(argv[2]); //Converting pipe descriptor from string to int
    
    write(fd,"Running time child \n ",30); //Print message to parent
    
    
    
    if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        write(fd,"socket error",20); // Print the error message to parent incase of socket error
        err_sys("socket error");
    }
    
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port   = htons(62353);	/* Binding to server port  */
    

	if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0)
    {
		write(fd,"inet_pton error",20); //Print error to parent
        err_quit("inet_pton error for %s", argv[1]);
    }
    
    if (connect(sockfd, (SA *) &servaddr, sizeof(servaddr)) < 0) //Checking for connection error and printing to parent
    {
        write(fd,"connect error",20);
        err_sys("connect error");
    }
    
    while ( (n = read(sockfd, recvline, MAXLINE)) > 0) //Checking for fputs error
    {
        recvline[n] = 0;	/* null terminate */
        if (fputs(recvline, stdout) == EOF)
        {
            write(fd,"fputs error",20);
            err_sys("fputs error");
        }
    }
    if (n < 0)
    {
        write(fd,"read error",20);  //Checking for read error and printing to parent
        err_sys("read error");
    }
    sleep(10);
    close(fd);
    exit(0);
}
