README file

==================================================
Name - Karthikeyan Swaminathan	
SBU ID - 110562357
Login ID CSE533-3
email - karthikeyan.swaminathan@stonybrook.edu
==================================================


=================================
User and Testing Documentation  :
=================================

The C source code files attached are the following:
	•	server.c  - Contains the source code for the time and echo server.
	•	client.c -   Contains the client source code which calls the respective client source files
	•	echo_cli - Contains the source code for  the echo client
	•	time_cli - Contains the source code for the time client

The corresponding executables that will be generated are as follows
	•	server.o
	•	client.o
	•	echo_cli.o
	•	time_cli.o

A make file of the name ‘Makefile’ is also attached.

Running  and Testing Instructions:
==================================



1) Download all the attached files into one folder.
2) cd into the source directory of these files in terminal.
3) Issue a ‘make’ command to generate the executables.
5) Start the server by issuing the ./server instruction.
6) On a separate xterm initialize the client by giving the following command :
	./client <ip_address/domain name>
	Example: ./client 8.8.8.8
7) The client will then resolve the domain name or IP address and then request the server to choose one of the three services :
	1.time
	2.echo
	3.quit
8) Choose one of the services by entering the corresponding serial number.

9) time service : Enter ‘1’ when the client requests you.
	time will be displayed every 5 seconds in a new xterm window child process.
	Meanwhile the parent will receive status messages from the child.
	To end the process use Ctrl+C.
10) Echo service : Enter ‘2’ to choose echo service.
		A new xterm window will open up with the child process.
		Meanwhile the parent will receive status messages from the child.
		enter any string to echo in the same ‘xterm’ and it will echo back to client.
		echo process will terminate on using Ctrl+D or Ctrl+C
11) choose ‘3’ to quit and exit the client.

12) The server prints messages on termination of each client.




==========================================
System Documentation and Implementation:
==========================================


Client
======


1. Command line arguments processing - The client is evoked with a command line argument giving either the server IP address in dotted decimal notation, or the server domain name. The client
handles either mode and figure out which of the two is being passed to it. If it is given the IP address, it calls the gethostbyaddr function to get the domain name, and prints it out. If the domain name is entered,the function gethostbyname,returns the IP address that corresponds to a given domain name and all other alias IP addresses. 

The source code of this is present in the file ‘client.c’



2. The client Parent - The client continuously queries the user as to what service he requires:
			1.time
			2.echo
			3.quit
			On choosing one of the serial numbers corresponding to the services, the client forks of a child process for each service. The parent continuously receives messages from the child using a half duplex pipe. Once the child process is terminated, the parent process prints an appropriate message and the querying cycle continues until the user chooses - ‘3.quit’.

The source code of this is present in the file ‘client.c’

3. The child client - The service requested by each user is executed by a separate child on a new xterm window. The source codes for each service are present in echo_time and echo_cli for time and client respectively. The child sends status messages to the parent using a half duplex pipe.

Ports Used:
62354 - Echo
62353 - Time



4. Service request termination and program robustness - Echo Client is terminated either using CTRL D or CTRL C. Time Client is terminated using CTRL C. On termination, messages are sent to the parent.Echo client is implemented using threads for robustness.

5. IPC using a pipe - Half duplex has been established between child and parent. Child communicates with the parent using this pipe to send status messages.

6. More robustness - Partially handled.




Server 
======


1.	Multi-threading and server services: Server is able handle multiple clients using threads and select. The daytime thread service sits in an infinite loop sending the daytime, sleeping for 5 seconds and repeating using the select function.
	The source code for this can be found in the ‘server.c’ file.

2.	Relation to inetd superserver: The server is loosely modelled based on the way, the inter daemon works. The primary difference is this server uses threads to execute different services.

3.	Listening to multiple services: Server creates 2 listening sockets for echo and time. Select() is used to multiplex between the two services.

4.	Robustness: Handled only partially.

5.	Time server Implementation: select() is used to make the server sleep for 5 seconds and send the daytime again.

6.	Proper status message: The server prints out status messages.

7.	SO_REUSEADDR socket option: SO_REUSEADDR is implemented to enable re use of the socket.







