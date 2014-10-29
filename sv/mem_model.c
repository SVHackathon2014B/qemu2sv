#include <stdio.h>
#include "svdpi.h"

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

//#define DEBUG_PRINT(fmt, ...) printf("%s(): "fmt, __func__, ## __VA_ARGS__)
#define DEBUG_PRINT(fmt, ...)

typedef struct __attribute__((packed)) {   
    unsigned int offset;
    unsigned int data;
    int we; //Read 0 Write 1
} rw_packet_t; 


//System verilog function
extern void write_access(rw_packet_t *); 
extern unsigned long long read_access(rw_packet_t *); 

//global
struct sockaddr_in server;
int sock;

rw_packet_t rw_packet_get;
rw_packet_t rw_packet_put;
	
int init_socket() {
	DEBUG_PRINT("Start\n");
	int i;
	int ret;
	/* ソケットの作成 */
	sock = socket(AF_INET, SOCK_STREAM, 0);
	
	/* 接続先指定用構造体の準備 */
	server.sin_family = AF_INET;
	server.sin_port = htons(15785);
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	
	/* サーバに接続 */
	printf("Start server connect...\n");
	fflush(stdout);
	ret = -1 ;
	for (i = 0 ; i < 15 ; i++){
		ret = connect(sock, (struct sockaddr *)&server, sizeof(server));
		DEBUG_PRINT("connect ret = %d \n",ret);
		if (ret >= 0){
			return 0;
		}
		sleep (1);
	}
	
	printf("Server Connect\n");
	fflush(stdout);
	
	return 1;
}

int socket_start() {
	int ret;
	/* サーバからデータを受信 */
	memset(&rw_packet_get, 0, sizeof(rw_packet_get));
	ret = recv(sock, &rw_packet_get, sizeof(rw_packet_get), 0);
	if (ret < 0){
		return 1;
	}
	
	//write / read
	if (rw_packet_get.we == 1 ){
	//write access
		DEBUG_PRINT("C::Write Access\n");
		DEBUG_PRINT("C::---ret = %d, offset = 0x%x , data = 0x%x\n", ret, rw_packet_get.offset,rw_packet_get.data);
		write_access(&rw_packet_get);
	} else {
		DEBUG_PRINT("C::Read Access\n");
		DEBUG_PRINT("C::---ret = %d, offset = 0x%x\n", ret, rw_packet_get.offset);
		rw_packet_put.we = 0;
		rw_packet_put.offset = rw_packet_get.offset;
		rw_packet_put.data = read_access(&rw_packet_get);
		ret = send(sock,&rw_packet_put, sizeof(rw_packet_put), 0);
		DEBUG_PRINT("C::ret = %d,data = 0x%x\n", ret, rw_packet_put.data);
		if (ret < 0){
			return 1;
		}
	}
	DEBUG_PRINT("---------------------------------------------------\n");
	fflush(stdout);
	return 0;
	
}
