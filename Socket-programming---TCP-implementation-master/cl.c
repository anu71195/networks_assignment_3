#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

int getMin()
{
   int i,count=0;
   char ti[50];
   char a,b;
    time_t s;
    time(&s);
    //printf("%s\n",ctime(&s) );
    strcpy(ti,ctime(&s));
    int tim_len=strlen(ti);
    for(i=0;i<tim_len;i++)
    {
      if(ti[i]==':')
      {
        count++;
        if(count==2)
        {
         a=ti[i+1];
         b=ti[i+2];
         break;
       }
      }
    }
    return ((int)a-48)*10+((int)b-48);

}


char* stringToBinary(char* s) {
    if(s == NULL) return 0; /* no input string */
    size_t i;
    int j;
    size_t len = strlen(s);
    char *binary =(char *) malloc(len*8 + 1); // each char is one byte (8 bits) and + 1 at the end for null terminator
    binary[0] = '\0';
    for(i = 0; i < len; ++i) {
        char ch = s[i];
        for(j = 7; j >= 0; --j){
            if(ch & (1 << j)) {
                strcat(binary,"1");
            } else {
                strcat(binary,"0");
            }
        }
    }
    return binary;
}

char* crc(char *real_input) {
 int i,j,keylen,msglen;
 char key[30],temp[30],quot[100],rem[30],key1[30];

 //clrscr();
char *input=(char *)malloc(256*sizeof(char));
 input=stringToBinary(real_input);
 strcpy(key,"10000111");
 keylen=strlen(key);
 msglen=strlen(input);
 char buff[msglen+keylen];
 strcpy(key1,key);
 for (i=0;i<keylen-1;i++) {
   input[msglen+i]='0';
 }
 for (i=0;i<keylen;i++)
  temp[i]=input[i];
 for (i=0;i<msglen;i++) {
   quot[i]=temp[0];
   if(quot[i]=='0')
    for (j=0;j<keylen;j++)
    key[j]='0'; else
    for (j=0;j<keylen;j++)
    key[j]=key1[j];
   for (j=keylen-1;j>0;j--) {
     if(temp[j]==key[j])
      rem[j-1]='0'; else
      rem[j-1]='1';
   }
   rem[keylen-1]=input[i+keylen];
   strcpy(temp,rem);
 }
 strcpy(rem,temp);
 printf("\nQuotient is ");
 for (i=0;i<msglen;i++)
  printf("%c",quot[i]);
 printf("\nRemainder is ");
 for (i=0;i<keylen-1;i++)
  printf("%c",rem[i]);
 printf("\nFinal data is: ");
 for (i=0;i<msglen;i++)
 {
   buff[i]=input[i];
  printf("%c",input[i]);
}
 for (i=0;i<keylen-1;i++)
 {
   buff[msglen+i]=rem[i];
  printf("%c",rem[i]);
}

 return buff;
}

void error(char *msg)
{
    perror(msg);
    exit(0);
}

void main(int argc, char *argv[])
{

    srand(time(NULL));
    int sockfd, portno, n,t1,t2;
    int error_switch=1;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[256],buffe[256],ack[256],nack[256];
    strcpy(ack,"010000010100001101001011");
    strcpy(nack,"01001110010000010100001101001011");
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0)
        error("ERROR connecting");
    printf("Please enter the message: ");
    bzero(buffe,256);
    fgets(buffe,255,stdin);
    char * bu=(char *)malloc(256*sizeof(char));
    bu=crc(buffe);
    printf("\nFinal Data is: %s\n",bu );
    int co=0;
    do
    {
      error_switch=rand()%2;
      //memset(buffer,0,sizeof(buffer));
    //  memset(copy,0,sizeof(copy));
      char copy[256];
      co++;
      strcpy(copy,bu);
    if(error_switch==1)
    {
        int  bit_error_rate=rand()%strlen(bu);
        //int  bit_error_rate=4;
        int i,x,count[strlen(bu)];
        for(i=0;i<strlen(bu);i++)
        {
            count[i]=0;
        }
        for(i=0;i<bit_error_rate;i++)
        {
            x=rand()%strlen(bu);
            while(count[x]!=1 && i!=0)
            x=rand()%strlen(bu);

            count[x]=1;
            if(copy[x]=='0')
                copy[x]='1';
            else
                copy[x]='0';

        }

    }
    if(strcmp(copy,bu)==0)
    printf("error bits present "  );
  //  printf("\ndata sent : %s\n",copy );
    n = write(sockfd,copy,strlen(copy));
    if (n < 0)
         error("ERROR writing to socket");
         t1=getMin();
    do{
      t2=getMin();
    bzero(buffer,256);
    n = read(sockfd,buffer,255);
    if (n < 0)
         error("ERROR reading from socket");
    printf("\n%s %d ",buffer,strlen(buffer));
    if(strcmp(buffer,ack)==0)
    printf("ACK\n" );
    else if(strcmp(buffer,nack)==0)
    printf("NACK\n" );
    else
    printf("Unknown response\n");
    if(t1-t2>3 || t2-t1 >3)
    strcpy(buffer,nack);
    }while(strcmp(buffer,ack)!=0 && strcmp(buffer,nack)!=0);

  }while(strcmp(buffer,nack)==0 && co<8);
    //return 0;
}
