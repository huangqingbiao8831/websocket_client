#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "wsclient.h"
#include "cJSON.h"

int onclose(wsclient *c) {
	fprintf(stderr, "onclose called: %d\n", c->sockfd);
	printf("onclose called: %d\n", c->sockfd);
	return 0;
}

int onerror(wsclient *c, wsclient_error *err) {
	fprintf(stderr, "onerror: (%d): %s\n", err->code, err->str);
	printf("onerror: (%d): %s\n", err->code, err->str);
	if(err->extra_code) {
		errno = err->extra_code;
		perror("recv");
	}
	return 0;
}

int onmessage(wsclient *c, wsclient_message *msg) {
	//fprintf(stderr, "onmessage: (%llu): %s\n", msg->payload_len, msg->payload);
	if (msg->payload_len < 1024)printf("onmessage: (%llu): %s\n", msg->payload_len, msg->payload);
        cJSON *json = cJSON_Parse(msg->payload);
        cJSON *sub_json = cJSON_GetObjectItem(json,"result");
        cJSON *sub2_json = cJSON_GetObjectItem(sub_json,"hypotheses");
        if (cJSON_IsTrue(cJSON_GetObjectItem(sub_json,"final")))
        {
           printf("the final result........\n");
           if (NULL != sub2_json)
           {
              cJSON *sub3_json = cJSON_GetArrayItem(sub2_json,0);
              if (NULL != sub3_json)
              {
                 printf("asr msg:%s \n",cJSON_GetObjectItem(sub3_json,"transcript")->valuestring);
                 printf("msg:%s \n",cJSON_Print(json));
              }
           } 
        }
        else
        {
           printf("not final result.......\n");
        }

        cJSON_Delete(json);
	return 0;
}

int onopen(wsclient *c) {
	fprintf(stderr, "onopen called: %d\n", c->sockfd);
	printf("onopen called: %d\n", c->sockfd);
	//libwsclient_send(c, "Hello onopen",strlen("hello noopen"),0x01);
	return 0;
}

void printHex(char *pbuf,int length)
{
  int i = 0;
  printf("send msg:\n\n");
  for (i = 1;i < (length+1);i++)
  {
    if (i%16 == 0) printf("\n");
    printf(" %02x",(unsigned char)pbuf[i]);
  } 
  printf("\n"); 
}

int readWav2sendWebws(wsclient *c,char *filename)
{
  char fileBuf[1024*2] = {0};
  FILE *fp = NULL;
  int n = 0;
  
  if (filename == NULL||NULL == c)
  {
    printf("finename is NULL or wsclient is NULL\n");
    return -1;
  }

 fp = fopen(filename,"rb");
 if (NULL == fp)
 {
   printf("readWav2sendWebws is NULL\n");
   return -2;
 }
 fseek(fp,0x2cL,SEEK_SET);/*skip file header*/
 while((n = fread(fileBuf,sizeof(char),320,fp)) >0)
 {
   //printHex(fileBuf+0x2c,n-0x2c);
   printHex(fileBuf,n);
   //libwsclient_send(c,fileBuf + 0x2c,(n-0x2c),0x02); 
   libwsclient_send(c,fileBuf,(n),0x02); 
   memset(fileBuf,0,sizeof(fileBuf));
   //usleep(100);
   //break;
 }

 libwsclient_send(c,"EOS",strlen("EOS"),0x01); 
 printf("wav file is send successful!!!!!\n");
 fclose(fp);
 return 0;
  
}

int main(int argc, char **argv) {
	//Initialize new wsclient * using specified URI
	char psendMsg[] = {0x1a,0x2b,0x3c,0x4d,0x00,0x5c,0x6d,0xdd,0xaa,0xac };
	//wsclient *client = libwsclient_new("ws://echo.websocket.org");
        //wsclient *client = libwsclient_new("ws://10.21.19.30:7014/client/ws/speech?content-type=audio/x-raw,+layout=(string)interleaved,+rate=(int)16000,+format=(string)S16LE,+channels=(int)1");
	wsclient *client = libwsclient_new("ws://10.1.124.15:7014/client/ws/speech?content-type=audio/x-raw,+layout=(string)interleaved,+rate=(int)16000,+format=(string)S16LE,+channels=(int)1");
	if(!client) {
		fprintf(stderr, "Unable to initialize new WS client.\n");
		exit(1);
	}
	//set callback functions for this client
	libwsclient_onopen(client, &onopen);
	libwsclient_onmessage(client, &onmessage);
	libwsclient_onerror(client, &onerror);
	libwsclient_onclose(client, &onclose);
	//bind helper UNIX socket to "test.sock"
	//One can then use netcat (nc) to send data to the websocket server end on behalf of the client, like so:
	// $> echo -n "some data that will be echoed by echo.websocket.org" | nc -U test.sock
	//libwsclient_helper_socket(client, "test.sock");
	//starts run thread.
	libwsclient_run(client);
	//blocks until run thread for client is done.i
	//send msg...
	//_libwsclient_write(client,"hello world",strlen("hello_world"));
	readWav2sendWebws(client,"hello_world.wav");
        while(1)sleep(1);
        //libwsclient_send(client,"hello world",strlen("hello world"),0x01); 
        //libwsclient_send(client,(char *)psendMsg,10,0x02); 
	libwsclient_finish(client);
	return 0;
}

