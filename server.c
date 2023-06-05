#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<string.h>
#include<stdlib.h>
#include<fcntl.h>
#include<netinet/in.h>
#include<unistd.h>

#define PORT 8000
#define MAXSZ 1000
#define MAX_LINE_LENGTH 1024
#define MAX_FIELD_LENGTH 256
#define DELIMITER ","

//Function to convert Integer into String
char* intToString(int number) {
        int length = snprintf(NULL, 0, "%d", number);
        char* str = (char*)malloc((length + 1) * sizeof(char));
        snprintf(str, length + 1, "%d", number);
        return str;
}

//Function to Return line number of given ID in the csv file
int checkLine(int n)
{
        FILE* file = fopen("data.csv", "r");
        int LineNo = 0;
        char line[MAX_LINE_LENGTH];
        char* str = intToString(n);
        while(fgets(line, sizeof(line), file))
        {
                if(line[0] != '#' && line[0] != '\n')
                {
                        char* token = strtok(line, DELIMITER);
                        LineNo++;
                        if(token != NULL)
                        {
                                if(!(strcmp(token, str)))
                                {
                                     break;
                                }
                        }
                }
        }
        fclose(file);
        return LineNo;
}

//function to check if given ID exist in the file or not
int checkID(int n)
{
        FILE* file = fopen("data.csv", "r");
        if (file == NULL)
        {
                printf("Error opening the file.\n");
                return 0;
        }
        int exist = 0;
        char line[MAX_LINE_LENGTH];
        char* str = intToString(n);
        while(fgets(line, sizeof(line), file))
        {
                if(line[0] != '#' && line[0] != '\n')
                {
                        char* token = strtok(line, DELIMITER);
                        if(token != NULL)
                        {
                                if(!(strcmp(token, str)))
                                {
                                        exist = 1;
                                        break;
                                }
                        }
                }
        }
        fclose(file);
        return exist;
}

//function to remove the given line number from the csv file
void removeLine(const char* filename, int lineToRemove)
{
        FILE* file = fopen(filename, "r+");
        FILE* tempFile = fopen("temp.csv", "w+");
        char buffer[1024];
        char ch;
        int currentLine = 1;
        while (fgets(buffer, sizeof(buffer), file))
        {
                if (currentLine != lineToRemove)
                {
                        fputs(buffer, tempFile);
                }
                currentLine++;
        }
        fseek(tempFile, 0, SEEK_SET);
        fseek(file, 0, SEEK_SET);
        while(fgets(buffer, sizeof(buffer), tempFile))
        {
                fputs(buffer, file);
                if(currentLine == 3)
                {
                        long position = ftell(file);
                        ftruncate(fileno(file), position);
                        break;
                }
                currentLine--;
        }

        fclose(file);
        fclose(tempFile);

        if (remove("temp.csv") != 0)
        {
                perror("Failed to remove the original file");
                return;
        }
     printf("Line %d removed successfully.\n", lineToRemove);
}

//structure to store marks
struct marks
{
        int maths;
        int science;
        int english;
};

//structure to store data
struct data
{
        int id;
        char name[10];
        struct marks m1;
};

//main function
int main()
{
        const char* filename = "data.csv";
        int sockfd;
        int newsockfd;
        struct data d1;
        char* str;
        char choice[1];

        struct sockaddr_in serverAddress;//server receive on this address
        struct sockaddr_in clientAddress;//server sends to client on this address
        int n;
        char msg[MAXSZ] = "Received Successfully";
        int clientAddressLength;

        //create socket
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        memset(&serverAddress,0,sizeof(serverAddress));

        //initialize the socket
    
         serverAddress.sin_family = AF_INET;
        serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
        serverAddress.sin_port = htons(PORT);

        //bid the socket with the server address and port
        bind(sockfd, (struct sockaddr *)&serverAddress, sizeof(serverAddress));

        //listen for connection from client
        listen(sockfd,5);

        //opening data.csv file
        int fd;
        fd = open("data.csv", O_RDWR, 0777);

        //accept a connection
        while(1)
        {
                printf("\n******Server Waiting For New Client Connection*****\n");
                clientAddressLength = sizeof(clientAddress);
                newsockfd = accept(sockfd,(struct sockaddr*)&clientAddress, &clientAddressLength);
                //receive from client
                printf("----------Connection Successful--------------\n");
                while(1)
                {
                        n = recv(newsockfd, choice, 1, 0);
                        n = recv(newsockfd, &d1, sizeof(d1), 0);
                        if(n == 0)
                        {
                                close(newsockfd);
                                break;
                        }
                        //lseek(fd,0,SEEK_END);
                        send(newsockfd, msg, n, 0);
                        if(!(checkID(d1.id)) && ((choice[0] == '1') || (choice[0] == '2')))
                        {
                                lseek(fd, 0, SEEK_END);
                                printf("Data Updated Successfully\n");
                                str = intToString(d1.id);
                                write(fd, str, strlen(str));
                                write(fd, ",", 1);
                                  write(fd, str, strlen(str));
                                write(fd, ",", 1);
                                write(fd, d1.name, strlen(d1.name));
                                write(fd, ",", 1);
                                str = intToString(d1.m1.maths);
                                write(fd, str, strlen(str));
                                write(fd, ",", 1);
                                str = intToString(d1.m1.science);
                                write(fd, str, strlen(str));
                                write(fd, ",", 1);
                                str = intToString(d1.m1.english);
                                write(fd, str, strlen(str));
                                write(fd, "\n", 1);
                        }
                        else if(choice[0] == '2')
                        {
                                lseek(fd,0,SEEK_SET);
                                int modify = checkLine(d1.id);
                                int lineNumber = 1;
                                char buf[MAXSZ];
                                read(fd, buf, MAXSZ);
                                lseek(fd, 0, SEEK_SET);
                                int i = 0;
                                while (buf[i] != '\0')
                                {
                                        if (buf[i] == '\n')
                                        {
                                                lineNumber++;
                                                if (lineNumber == modify)
                                                {
                                                         printf("Data Found on Line number: %d\n",modify);
                                                         str = intToString(d1.id);
                                                         lseek(fd, i+1, SEEK_SET);
                                                         write(fd, str, strlen(str));
                                                         write(fd, ",", 1);
                                                         write(fd, d1.name, strlen(d1.name));
                                                         write(fd, ",", 1);
                                                         str = intToString(d1.m1.maths);
                                                         write(fd, str, strlen(str));
                                                         write(fd, ",", 1);
                                                         str = intToString(d1.m1.science);
                                                         write(fd, str, strlen(str));
                                                         write(fd, ",", 1);
                                                         str = intToString(d1.m1.english);
                                                         write(fd, str, strlen(str));
                                                         break;
                                                }
                                        }
                                        i++;
                                }
                        }
                        else if(choice[0] == '3' && checkID(d1.id))
                        {
                                int check = checkLine(d1.id);
                                removeLine(filename, check);
                        }
                        else if(choice[0] == '3' && !checkID(d1.id))
                        {
                                printf("Entered ID does not exist\n");
                        }
                        else
                        {
                                printf("Data already exist for given ID\n");
                        }
                }
        }
        close(fd);
        return 0;
}
