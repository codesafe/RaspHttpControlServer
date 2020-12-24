#include "httpd.h"
#include <string>
#include <stdio.h>
#include <fcntl.h>    
#include <sys/stat.h>        
#include <semaphore.h>
#include <unistd.h>
#include <errno.h>

#include <stdlib.h>	/* atexit, getenv, malloc */
#include <string.h>	/* memcpy */

/* Filename for only_one_instance() lock. */
#define INSTANCE_LOCK "rosetta-code-lock"

void
fail(const char* message)
{
	perror(message);
	exit(1);
}

/* Path to only_one_instance() lock. */
static char* ooi_path;

void
ooi_unlink(void)
{
	unlink(ooi_path);
}


/* Exit if another instance of this program is running. */
void only_one_instance(void)
{
	fprintf(stderr, "check only_one_instance!!\n");
	struct flock fl;
	size_t dirlen;
	int fd;
	char* dir;

	/*
	 * Place the lock in the home directory of this user;
	 * therefore we only check for other instances by the same
	 * user (and the user can trick us by changing HOME).
	 */
	dir = getenv("HOME");
	if (dir == NULL || dir[0] != '/') 
	{
		fprintf(stderr, "Bad home directory.\n");
		exit(1);
	}
	dirlen = strlen(dir);

	ooi_path = (char*)malloc(dirlen + sizeof("/" INSTANCE_LOCK));
	if (ooi_path == NULL)
		fail("malloc");

	memcpy(ooi_path, dir, dirlen);
	memcpy(ooi_path + dirlen, "/" INSTANCE_LOCK,
		sizeof("/" INSTANCE_LOCK));  /* copies '\0' */

	fd = open(ooi_path, O_RDWR | O_CREAT, 0600);
	if (fd < 0)
		fail(ooi_path);

	fl.l_start = 0;
	fl.l_len = 0;
	fl.l_type = F_WRLCK;
	fl.l_whence = SEEK_SET;
	if (fcntl(fd, F_SETLK, &fl) < 0) 
	{
		fprintf(stderr, "Another instance of this program is running.\n");
		exit(1);
	}

	/*
	 * Run unlink(ooi_path) when the program exits. The program
	 * always releases locks when it exits.
	 */
	atexit(ooi_unlink);
}


int main(int c, char** v)
{
	only_one_instance();
 	serve_forever("12345");
	return 0;
}

void route()
{
	ROUTE_START()
	ROUTE_GET("/")
	{
		printf("HTTP/1.1 200 OK\r\n\r\n");
		//printf("Usage :\n");
		//printf("/start --> start camera app\n");
		//printf("/stop --> stop camera app\n");
		//printf("/restart --> restart camera app\n");

		printf(
			"<!DOCTYPE html>\n"
			"<html lang = \"ja\">\n"
			"<head>\n"
			"<meta charset = \"utf-8\">\n"
			"</head>\n"
			"<body>\n"
			"<h1>Camera Controller</h1>\n"
			"</body>"
			"</html>");
		
		getmyIpAddr("wlan0");

		std::string address_start = "http://"+ myipaddress + ":12345/start";
		std::string address_stop = "http://" + myipaddress + ":12345/stop";
		std::string address_restart = "http://" + myipaddress + ":12345/restart";

		printf(
			"<button type = \"button\" onclick = \"location.href='%s'\">Camera Start</button>" \
			"<br><br>" \
			"<button type = \"button\" onclick = \"location.href='%s'\">Camera Stop</button>" \
			"<br><br>" \
			"<button type = \"button\" onclick = \"location.href='%s'\">Camera Restart</button>",
			address_start.c_str(),
			address_stop.c_str(),
			address_restart.c_str()
		);
/*
		printf(
			"<button type = \"button\" onclick = \"location.href='http://192.168.26.206/start'\">Camera Start</button>"
			"<br><br>"
			"<button type = \"button\" onclick = \"location.href='http://192.168.26.206/stop'\">Camera Stop</button>"
			"<br><br>"
			"<button type = \"button\" onclick = \"location.href='http://192.168.26.206/stop'\">Camera Restart</button>"
		);
*/
	}

	ROUTE_GET("/stop")
	{
		printf("HTTP/1.1 200 OK\r\n\r\n");
		//printf("Recv Stop!\n");
	}

	ROUTE_GET("/start")
	{
		printf("HTTP/1.1 200 OK\r\n\r\n");
		//printf("Recv Start!\n");
	}

	ROUTE_GET("/restart")
	{
		printf("HTTP/1.1 200 OK\r\n\r\n");
		//printf("Recv ReStart!\n");
	}

	ROUTE_POST("/")
	{
		printf("HTTP/1.1 200 OK\r\n\r\n");
		printf("Wow, seems that you POSTed %d bytes. \r\n", payload_size);
		printf("Fetch the data using `payload` variable.");
	}

	ROUTE_END()
}
