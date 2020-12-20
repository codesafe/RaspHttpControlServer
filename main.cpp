#include "httpd.h"
#include <string>


int main(int c, char** v)
{
	//run_app();
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
