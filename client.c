#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<string.h>
#include<stdlib.h>
#include<netinet/in.h>

#define PORT 8000
#define SERVER_IP "127.0.0.1"
#define MAXSZ 1000

char* intToString(int number)
{
        int length = snprintf(NULL, 0, "%d", number);
        char* str = (char*)malloc((length + 1) * sizeof(char));
        snprintf(str, length + 1, "%d", number);
        return str;
}

struct marks
{
        int maths;
        int science;
        int english;
};

struct data
{
        int id;
        char name[10];
        struct marks m1;
};

int main()
{
                int sockfd;
                struct data d1;
                struct sockaddr_in serverAddress;

                int n;
                int choice;
                char msg1[MAXSZ];
                char msg2[MAXSZ];

                //create socket
                sockfd = socket(AF_INET, SOCK_STREAM, 0);
                //initialize the socket address
                memset(&serverAddress,0,sizeof(serverAddress));
                serverAddress.sin_family = AF_INET;
                serverAddress.sin_addr.s_addr = inet_addr(SERVER_IP);
                serverAddress.sin_port = htons(PORT);
                //client connect to server on port
                connect(sockfd, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
                //send to server and receive from server
                while(1)
                {
                        printf("\nEnter your choice\n 1. INSERT\n 2. MODIFY\n 3. DELETE:\n");
                        scanf("%d", &choice);
                        send(sockfd, intToString(choice), 1, 0);
                        switch(choice)
                        {
                                case 1:
                                case 2:
                                        printf("Enter ID: ");
                                        scanf("%d",&d1.id);
                                        printf("Enter name: ");
                                        scanf("%s",&d1.name);
                                        printf("Enter marks: ");
                                        scanf("%d%d%d", &d1.m1.maths, &d1.m1.science, &d1.m1.english);
                                        send(sockfd, &d1 ,sizeof(d1),0);
                                        n = recv(sockfd,msg2,MAXSZ,0);
                                        printf("Receive message from server::%s\n",msg2);
                                        break;
                                case 3:
                                         printf("Enter ID: ");
                                        scanf("%d", &d1.id);
                                        send(sockfd, &d1, sizeof(d1),0);
                        }
                }
                return 0;
}
