#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <fcntl.h>

#define SERVER_ADDR "127.0.0.1"

#define handle_error(msg) \
	do { perror(msg); exit(EXIT_FAILURE); } while (0)


int main(int argc, char* argv[]){
	

	/* SERVER SOCKET */
	int s_server = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in s_server_addr;
	s_server_addr.sin_family = AF_INET;
	s_server_addr.sin_port = htons(atoi(argv[2]));
	inet_aton(argv[1], &s_server_addr.sin_addr);

	bind(s_server, (const struct sockaddr*) &s_server_addr, sizeof(s_server_addr));

	listen(s_server, 2);
	/* SERVER SOCKET */
	printf("[server] Listen socket for clients\n");
	fflush(stdout);

	int proc_id;

	int s_client;
	
	char buf[BUFSIZ] = {0};
	
	while(1){
		s_client = accept(s_server, NULL, NULL);
		if(s_client == -1)
			handle_error("[server] accept client");
		printf("[server] Client %d connected\n", s_client);
		sprintf(buf, "[info] data successfully writed into file\n");
		write(s_client, buf, sizeof(buf));

		proc_id = fork();
		if(proc_id > 0){
		}		
		else{
			FILE* fclient;
			char file[20] = {0};
			sprintf(file, "./client%d", s_client);
			fclient = fopen(file, "a+");
			while(1){
				char bufr[BUFSIZ] = {0};
				int nbytes = read(s_client, bufr, BUFSIZ);
				
				if(strcmp(bufr, "/close\n") == 0){
					sprintf(bufr, "[info] data successfully writed to file, connection closed\n");
					write(s_client, bufr, sizeof(bufr));
					printf("[server] closing connection\n");
					break;
				}
				else 
					if(strcmp(bufr, "/read\n") == 0){
						fseek(fclient, 0L, SEEK_SET);
						int nread;
						sprintf(bufr, "[info] **** start of file ****\n");
						write(s_client, bufr, sizeof(bufr));	
						char buftemp[BUFSIZ] = {0};
						while((nread = fread(buftemp, 1, BUFSIZ, fclient)) > 0){
							//fscanf(fclient, "%s\n", buftemp);
							buftemp[nread] = '\0';						
							write(s_client, buftemp, sizeof(buftemp));
						}
						sprintf(bufr, "[info] **** end of file ****\n");
						write(s_client, bufr, sizeof(bufr));
					}
					else{
						fseek(fclient, 0L, SEEK_END);
						bufr[nbytes] = '\0';
						fprintf(fclient, "%s", bufr);
						//write(s_client, bufr, nbytes);
					}
			}
			shutdown(s_client, SHUT_RDWR);		
			close(s_client);
			fclose(fclient);
		}		
	}
	close(s_server);
}
