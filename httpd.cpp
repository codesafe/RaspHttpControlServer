#include "httpd.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <signal.h>

#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>


#define CONNMAX 1000

static int listenfd, clients[CONNMAX];
static void error(char *);
static void startServer(const char *);
static void respond(int);

typedef struct { char *name, *value; } header_t;
static header_t reqhdr[17] = { {"\0", "\0"} };
static int clientfd;
static char *buf;


char* method, *uri, *qs, *prot;
char* payload;
int   payload_size;
std::string myipaddress;

// ´ÜÀ§´Â Second
void	Sleep(float t)
{
	usleep((unsigned int)(t * 1000000.f));
}

void controlCameraApp(char* cmd)
{
	fprintf(stderr, "---> %s\n", cmd);

    if (strcmp(cmd, "/stop") == 0)
    {
        fprintf(stderr, "STOP!!!\n");
        kill_app();
    }
    else if (strcmp(cmd, "/start") == 0)
    {
        fprintf(stderr, "START!\n");
        run_app();
    }
	else if (strcmp(cmd, "/restart") == 0)
	{
		fprintf(stderr, "RESTART!\n");
        kill_app();
        Sleep(1);
        run_app();
	}
}

void returntomain(char *msg)
{
    getmyIpAddr("wlan0");

    std::string address = "http://" + myipaddress + ":12345";
	printf(
		"<!DOCTYPE html>\n"\
		"<html>\n"\
		"<head>\n"\
		"<meta charset=\"utf-8\">\n"\
		"<meta http-equiv=\"refresh\" content=\"3; url=%s\">"\
		"</head>\n"\
		"<body>\n"\
        "<h2>%s</h2>\n"\
		"</body>"\
		"</html>"
		, address.c_str(),
        msg);

/*
	printf(
		"<!DOCTYPE html>\n"\
		"<html>\n"\
		"<head>\n"\
		"<meta charset = \"utf-8\">\n"\
        "<meta http-equiv=\"refresh\" content=\"3; url=http://www.netmarble.net/\">"\
		"</head>\n"\
		"<body>\n"\
		"</body>"\
		"</html>"
        , );
*/
}

std::string ExecCmd(std::string command)
{
	char buffer[128];
    std::string cmd_out = "";

	// open pipe to file and execute command in linux terminal
	FILE* pipe = popen(command.c_str(), "r");
	if (!pipe)
		return "popen failed!";

	// read till end of process:
	while (!feof(pipe)) 
    {
		// read output of the sent command and add to result
		if (fgets(buffer, 128, pipe) != NULL)
			cmd_out += buffer;
	}
	pclose(pipe);
	// returns the output of terminal in string format
	return cmd_out;
}


void run_app()
{
    returntomain("Start CamaraController..");

	pid_t pid = fork();
	if (pid == 0)
	{
        //system("./run_camera.sh &");
        //system("./RaspCamControl.out");
        //char* argv[] = { NULL , NULL };
		//execl("./RaspCamControl.out &", NULL);
		//exit(1);
        //ExecCmd("nohup ./RaspCamControl.out &");
        //ExecCmd("./RaspCamControl.out &");
        ExecCmd("./run_camera.sh");
	}
 }

void kill_app()
{
    returntomain("Try kill CamaraController..");
	//system("killall -9 ./RaspCamControl.out");
    ExecCmd("killall -9 ./RaspCamControl.out");
}

void getmyIpAddr(char *adapter)
{
	int fd;
	struct ifreq ifr;
    char myaddr[20] = { 0, };

	fd = socket(AF_INET, SOCK_DGRAM, 0);
	ifr.ifr_addr.sa_family = AF_INET;
	//strncpy(ifr.ifr_name, "eth1", IFNAMSIZ - 1);
    strncpy(ifr.ifr_name, adapter, IFNAMSIZ - 1);
	ioctl(fd, SIOCGIFADDR, &ifr);
	close(fd);

	sprintf(myaddr, "%s", inet_ntoa(((struct sockaddr_in*)&ifr.ifr_addr)->sin_addr));
    myipaddress = myaddr;
}

void serve_forever(const char *PORT)
{
    struct sockaddr_in clientaddr;
    socklen_t addrlen;
    char c;    
    
    int slot=0;
 
    //getmyIpAddr("eth1", myaddr);
    getmyIpAddr("wlan0"); 

    //printf("Server started %shttp://127.0.0.1:%s%s\n", "\033[92m",PORT,"\033[0m" );
    printf("Server started %s:%s\n", myipaddress.c_str(), PORT);

    // Setting all elements to -1: signifies there is no client connected
    int i;
    for (i=0; i<CONNMAX; i++)
        clients[i]=-1;

    startServer(PORT);
    
    // Ignore SIGCHLD to avoid zombie threads
    signal(SIGCHLD,SIG_IGN);

    // ACCEPT connections
    while (1)
    {
        addrlen = sizeof(clientaddr);
        clients[slot] = accept (listenfd, (struct sockaddr *) &clientaddr, &addrlen);

        if (clients[slot]<0)
        {
            perror("accept() error");
        }
        else
        {
            if ( fork()==0 )
            {
                respond(slot);
                exit(0);
            }
        }

        while (clients[slot]!=-1) slot = (slot+1)%CONNMAX;
    }
}

//start server
void startServer(const char *port)
{
    struct addrinfo hints, *res, *p;

    // getaddrinfo for host
    memset (&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if (getaddrinfo( NULL, port, &hints, &res) != 0)
    {
        perror ("getaddrinfo() error");
        exit(1);
    }
    // socket and bind
    for (p = res; p!=NULL; p=p->ai_next)
    {
        int option = 1;
        listenfd = socket (p->ai_family, p->ai_socktype, 0);
        setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
        if (listenfd == -1) continue;
        if (bind(listenfd, p->ai_addr, p->ai_addrlen) == 0) break;
    }
    if (p==NULL)
    {
        perror ("socket() or bind()");
        exit(1);
    }

    freeaddrinfo(res);

    // listen for incoming connections
    if ( listen (listenfd, 1000000) != 0 )
    {
        perror("listen() error");
        exit(1);
    }
}


// get request header
char *request_header(const char* name)
{
    header_t *h = reqhdr;
    while(h->name) 
    {
        if (strcmp(h->name, name) == 0) 
            return h->value;
        h++;
    }
    return NULL;
}

//client connection
void respond(int n)
{
    int rcvd, fd, bytes_read;
    char *ptr;

    buf = (char*)malloc(65535);
    rcvd=recv(clients[n], buf, 65535, 0);

    if (rcvd<0)    // receive error
        fprintf(stderr,("recv() error\n"));
    else if (rcvd==0)    // receive socket closed
        fprintf(stderr,"Client disconnected upexpectedly.\n");
    else    // message received
    {
        buf[rcvd] = '\0';

        method = strtok(buf,  " \t\r\n");
        uri    = strtok(NULL, " \t");
        prot   = strtok(NULL, " \t\r\n"); 

        fprintf(stderr, "\x1b[32m + [%s] %s\x1b[0m\n", method, uri);

        if (qs = strchr(uri, '?'))
        {
            *qs++ = '\0'; //split URI
        } else {
            qs = uri - 1; //use an empty string
        }

        header_t *h = reqhdr;
        char *t, *t2;
        while(h < reqhdr+16) 
        {
            char *k,*v,*t;
            k = strtok(NULL, "\r\n: \t"); 
            if (!k) break;
            v = strtok(NULL, "\r\n");     
            while(*v && *v==' ') v++;
            h->name  = k;
            h->value = v;
            h++;
            fprintf(stderr, "[H] %s: %s\n", k, v);
            t = v + 1 + strlen(v);
            if (t[1] == '\r' && t[2] == '\n') 
                break;
        }

        t++; // now the *t shall be the beginning of user payload
        t2 = request_header("Content-Length"); // and the related header if there is  
        payload = t;
        payload_size = t2 ? atol(t2) : (rcvd-(t-buf));

        // bind clientfd to stdout, making it easier to write
        clientfd = clients[n];
        dup2(clientfd, STDOUT_FILENO);
        close(clientfd);

        // call router
        route();

        //
        controlCameraApp(uri);

        // tidy up
        fflush(stdout);
        shutdown(STDOUT_FILENO, SHUT_WR);
        close(STDOUT_FILENO);
    }

    //Closing SOCKET
    shutdown(clientfd, SHUT_RDWR);         //All further send and recieve operations are DISABLED...
    close(clientfd);
    clients[n]=-1;
}
