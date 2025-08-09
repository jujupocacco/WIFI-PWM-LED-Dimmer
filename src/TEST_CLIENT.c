#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

int main(){

	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	char* cmd = "LED1";
	struct sockaddr_in serverinfo;
	memset(&serverinfo, 0, sizeof(serverinfo));
	serverinfo.sin_family = AF_INET;
	serverinfo.sin_port = htons(8080);

	if(inet_pton(AF_INET, "192.168.4.1", &serverinfo.sin_addr)<=0){
		printf("%s\n", "FAILED TO CONVERT IP");
	}

	sendto(sockfd, cmd, strlen(cmd), 0, (struct sockaddr *) &serverinfo, sizeof(serverinfo));
	close(sockfd);

	return 0;

}
