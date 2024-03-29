#include <stdio.h>
#include <string.h>   //strlen
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>   //close
#include <arpa/inet.h>    //close
#include <sys/types.h>
#include <sys/socket.h>
#include <time.h>
#include <netinet/in.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros

#define TRUE   1
#define FALSE  0



int crcCheck(char* input)
{
    int i,j,keylen,msglen,rem_len;
    char  key[30],temp[30],quot[100],rem[30],key1[30],inp[300];
    strcpy(key,"100000111");
    strcpy(inp,input);
    keylen=strlen(key);
    msglen=strlen(input);
    strcpy(key1,key);
    char *temp_input=(char *)malloc(256*sizeof(char));
	strcpy(temp_input,input);

    for(int j=0;j<msglen;j++)
    {
       if(temp_input[j]=='1')
       { 
            for(int i=0;i<keylen;i++)
            {
                if(temp_input[i+j]==key[i])
                    temp_input[i+j]='0';
                else 
                    temp_input[i+j]='1';
            }
        }      
    }

    for(i=0;i<strlen(temp_input);i++)
    {
        if(temp_input[i]=='1')
       {
       	return 1;
       } 
    }
    return 0;
}

char* stringToBinary(char* s) 
{
    if(s == NULL) return 0; /* no input string */
    size_t i;
    int j;
    size_t len = strlen(s);
    char *binary =(char *) malloc(len*8 + 1); // each char is one byte (8 bits) and + 1 at the end for null terminator
    binary[0] = '\0';
    for(i = 0; i < len; ++i) 
    {
        char ch = s[i];
        for(j = 7; j >= 0; --j)
        {
            if(ch & (1 << j)) 
            {
                strcat(binary,"1");
            } 
            else 
            {
                strcat(binary,"0");
            }
        }
    }
    return binary;
}

void binary_to_string(char * s, char **temp_s)
{
	int mssg_len=strlen(s);
	char output[256];
	int character=0;
	char str_char;
	for(int i=0;i<(mssg_len/8)-1;i++)
	{
		character=0;
		for(int j=0;j<8;j++)
		{
			character*=2;
			character+=(int)s[8*i+j]-48;
		}
		str_char=character;

		printf("%c",str_char);
	}
	*temp_s=output;
}
int main(int argc , char *argv[])
{
    srand(time(NULL));
    if(argc!=2)
    {
    	printf("Error in the no of args\n");
    }
    int PORT=atoi(argv[1]);
    // close(PORT);
    int opt = TRUE;
    int master_socket , addrlen , newsockfd , client_socket[30] ,max_clients = 30 , activity, i  , sd,n,error_flag=1;
    ssize_t valread;
    int max_sd;
    struct sockaddr_in address;

    char buffer[256],ack[256],nack[256],err[256],copy[256],str_buffer_check[256];  //data buffers
    strcpy(ack,"010000010100001101001011");
    strcpy(nack,"01001110010000010100001101001011");
    //set of socket descriptors
    fd_set readfds;

    //a message
    char *message = "Server side response\r\n";

    //initialise all client_socket[] to 0 so not checked
    for (i = 0; i < max_clients; i++)
    {
        client_socket[i] = 0;
    }

    //create a master socket
    if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    //set master socket to allow multiple connections ,
    //this is just a good habit, it will work without this
    if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt,
          sizeof(opt)) < 0 )
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    //type of socket created
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );

    //bind the socket to localhost port 8888
    if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    printf("Listener on port %d \n", PORT);

    //try to specify maximum of 3 pending connections for the master socket
    if (listen(master_socket, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    //accept the incoming connection
    addrlen = sizeof(address);
    puts("Waiting for connections ...");

    while(TRUE)
    {
    	int decision=0;
    	if(decision==1)
    		exit(1);
        //clear the socket set
        FD_ZERO(&readfds);

        //add master socket to set
        FD_SET(master_socket, &readfds);
        max_sd = master_socket;

        //add child sockets to set
        for ( i = 0 ; i < max_clients ; i++)
        {
            //socket descriptor
            sd = client_socket[i];

            //if valid socket descriptor then add to read list
            if(sd > 0)
                FD_SET( sd , &readfds);

            //highest file descriptor number, need it for the select function
            if(sd > max_sd)
                max_sd = sd;
        }

        //wait for an activity on one of the sockets , timeout is NULL ,
        //so wait indefinitely
        activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);

        if ((activity < 0) && (errno!=EINTR))
        {
            printf("select error");
        }

        //If something happened on the master socket ,
        //then its an incoming connection
              
      
        if (FD_ISSET(master_socket, &readfds))
        {
            if ((newsockfd = accept(master_socket, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
            {
                perror("accept");
                exit(EXIT_FAILURE);
            }
            // printf("%d--newsockfd\n", newsockfd);

            //inform user of socket number - used in send and receive commands
            printf("New connection , socket fd is %d , ip is : %s , port : %d\n" , newsockfd , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));

            //send new connection greeting message
           /* if( send(newsockfd, message, strlen(message), 0) != strlen(message) )
            {
                perror("send");
            }*/

           // puts("Welcome message sent successfully");

            //add new socket to array of sockets


            // for (i = 0; i < max_clients; i++)
            // {

            //     //if position is empty
            //     if( client_socket[i] == 0 )
            //     {
            //         client_socket[i] = newsockfd;
            //         printf("Adding to list of sockets as %d\n" , i);

            //         break;
            //     }
            // }
        // printf("Here");

        }
        int pid = fork();
        if(pid!=0)
        {
            close(sd);
        }
        while(pid==0)
        {
        // printf("Here");
     
        //else its some IO operation on some other socket
        // for (i = 0; i < max_clients; i++)
        // { 
        	// printf("client is %d\n",i );
            sd = newsockfd;
            // printf("%d is the sd",sd);

            // if (FD_ISSET( sd , &readfds))
            // {
                //Check if it was for closing , and also read the
                //incoming message
                //printf("ktry\n");
                // for(int i=0;i<256;i++)
                // 	buffer[i]='0';
                // flush(buffer);
                // printf("here");
            memset(buffer,0,sizeof(buffer));
            if ((valread = read( sd , buffer, 256)) == 0)
            {
                    //Somebody disconnected , get his details and print

                getpeername(sd , (struct sockaddr*)&address , (socklen_t*)&addrlen);
                printf("Host disconnected , ip %s , port %d \n" ,
                      inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
                printf("type 0 to continue and 1 to exit the server:-");
                scanf("%d",&decision);
                if(decision==1)
                	exit(1);
                //Close the socket and mark as 0 in list for reuse
                close( sd );
                client_socket[i] = 0;
            }

                //Echo back the message that came in
            else
            {
                	// printf("%ld is the valread\n",valread);
                int co=0;
                    // printf("read");
                do
                {

                       // buffer[valread] = '\0';
                        //send(sd , buffer , strlen(buffer) , 0 );
            		char temp[256];
            		strcpy(temp,buffer);
                    int le=strlen(buffer);
                            // for(int i=0;i<strlen(buffer);i++)
                            // 	if(buffer[i]!='0' && buffer[i]!='1')
                            // 	{
                            // 		temp[i]='\0';
                            // 		break;
                            // 	}
                    strcpy(buffer,temp);
                    printf("\ndata received in binary = %s\n",buffer );
                    char * temp_buffer=(char*)malloc(sizeof(char)*256);
                    printf("data received as string is =");
                    binary_to_string(buffer,&temp_buffer);
                    printf("\n");
                    int check=crcCheck(buffer);
                    if (check==1)
                    	printf("%d message received is wrong\n",check );
                    else
                    	printf("%d message received is right\n",check);
                    co=0;                                                                              //chk
                          // do
                          //  {
                    sleep(1);
                    co++;
                    int ran=rand()%3;
                    if(check==0)
                    {
                        while(check==0)
                        {
                            ran=rand()%3;
                            printf("Message recieved wihtout error\n");
                            if(ran==1)
                            {
                            	printf("error in ack sent\n");
                            	strcpy(err,ack);
                                int bit_error_rate=rand()%strlen(ack);
                                printf("bit error rate in ack is %d\n ",bit_error_rate);
                                int i,x,count[strlen(ack)];
    					        for(i=0;i<strlen(ack);i++)
    					        {
    					            count[i]=0;
    					        }
                                for(i=0;i<bit_error_rate;i++)
    					        {
    					            x=rand()%strlen(ack);
    					            while(count[x]!=0 && i!=0)
    					            {
    					         	   x=rand()%strlen(ack);
    						        }
    					            count[x]=1;
    					            if(err[x]=='0')
    					                err[x]='1';
    					            else
    					                err[x]='0';

    						   } 
                                printf("ack is %s but sent ack is %s\n",ack,err);

                                n = write(newsockfd,err,24);
    	                    }
     
                            else
                            {
                                n = write(newsockfd,ack,24);
                                printf("no error in ack sent\n" );
                                error_flag=0;
                                break;
                            }
                        }
                    }
                    else
                    {
                        printf("Error during transmission\n");
                        error_flag=1;
                        if(ran==1)
                        {
                            // printf("Yo\n");
                            error_flag=1;
                            int x=rand()%strlen(nack);
                            strcpy(err,nack);

                            if(err[x]=='0')
                                err[x]='1';
                            else
                                err[x]='0';

                            n = write(newsockfd,err,25);
                        }

                        else
                        {
                            n = write(newsockfd,nack,33);
                        }
                        if ((valread = read( sd , buffer, 256)) == 0)
                        {
                            //Somebody disconnected , get his details and print

                            getpeername(sd , (struct sockaddr*)&address , \
                                (socklen_t*)&addrlen);
                            printf("Host disconnected , ip %s , port %d \n" ,
                                  inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
                            printf("type 0 to continue and 1 to exit the server:-");
                            scanf("%d",&decision);
                            if(decision==1)
                                exit(1);
                                //Close the socket and mark as 0 in list for reuse
                            close( sd );
                            client_socket[i] = 0;
                        }
                    }
                    if (n < 0) perror("ERROR writing to socket\n");
                }while(error_flag==1);
            }
        }  
    }
    return 0;
}
