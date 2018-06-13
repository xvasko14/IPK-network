                //////////////////////
                ///   Michal Vasko ///
                ///  xvasko14     ///
                ///   2BIT BIB    ///
                ///    IPK2       ///
                /////////////////////

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <signal.h>


#define SIZE 2000
#define MSG_SIZE 80
#define MYPORT 21011

// globalne premene 
char *name_chat;
bool First = true;
int sockfd;

 
//funkcia na uzatvorenie clienta a vypis logged out
void endingclient(int signalis ){
    char prem[SIZE];
    strcpy(prem, name_chat);
    strcat(prem, " logged out\r\n");
    send(sockfd, prem, strlen(prem), 0);
    exit(1);
}

// hlavne telo programu
int main(int argc, char *argv[]) {
  // moje premene potrebne k chodu programu
   int port;
   int fd;
   fd_set readfds, testfds, clientfds,master;
   char msg[MSG_SIZE + 1];     
   char kb_msg[MSG_SIZE + 10]; 
   int result;
   const char *server_hostname;
   struct hostent *hostinfo;
   struct sockaddr_in address;
   
   // argumenty 
   //prvy argument -i kde piseme nasu ip adresu
   //druhy argument -u kde urcime nasu meno pod ktorym chceme chatovat
  if(argc == 5){
        if ((strcmp(argv[1], "-i") == 0) && (strcmp(argv[3], "-u") == 0))  {
            server_hostname = argv[2];
            port=MYPORT;

            name_chat = malloc(strlen(argv[4]) + 1);
            memset(name_chat, '\0', strlen(name_chat));
            strcpy(name_chat, argv[4]);
        }

        else if ((strcmp(argv[1], "-u") == 0) && (strcmp(argv[3], "-i") == 0))  {
            server_hostname = argv[4];
            port=MYPORT;

            name_chat = malloc(strlen(argv[2]) + 1);
            memset(name_chat, '\0', strlen(name_chat));
            strcpy(name_chat, argv[2]);
        }
        //ak zadame zle argumenty potom chyba (napr -a atd..)
        else{
              fprintf(stderr,"Zle zadany argument, pouzi -i ip_adresa  -u vase_meno\n");
              exit(EXIT_FAILURE);
           }
    }
   //ak zadame nespravny pocet argumentov 
    else {
      fprintf(stderr,"Zle zadany argument, pouzi -i ip_adresa  -u vase_meno\n");
      exit(EXIT_FAILURE);
    }
     fflush(stdout);
     
     //vytvarame sokety pre klienta
     sockfd = socket(AF_INET, SOCK_STREAM, 0);

     // menovanie socketov a tvorenie ip adresu aby isla so serverom
     // funguje to aj pre cisto zadany iba localhost bez ip adresy
     hostinfo = gethostbyname(server_hostname);  // moznost toho localhostu
     address.sin_addr = *(struct in_addr *)*hostinfo -> h_addr_list;
     address.sin_family = AF_INET;
     address.sin_port = htons(port);

     /// pripojenie ku soketom a serverovym soketom
     if(connect(sockfd, (struct sockaddr *)&address, sizeof(address)) < 0){
       perror("connecting");
       exit(1);
     }
     
     fflush(stdout);
     
     //deskriptor
     FD_ZERO(&master);
     FD_ZERO(&clientfds);
     FD_ZERO(&readfds);
     FD_SET(sockfd,&clientfds);
     FD_SET(0,&master);
     FD_SET(0,&clientfds);
     
     // signal ktory vola aj funckciu endingclient ktora po stiknuti danej klavesi nas vypoji
     signal(SIGINT, endingclient);

     // hlavny proces chodu programu
     while (1) {

      if(First){
        // ked sa niekto pripoji vypise nam Michal Logged in...
          First = false;
          char logas[SIZE];
          strcpy(logas, name_chat);
          strcat(logas, " logged in\r\n");
          send(sockfd, logas, strlen(logas), 0);
      }
       testfds=clientfds;
       select(FD_SETSIZE,&testfds,NULL,NULL,NULL);

       //cyklus
       for(fd=0;fd<FD_SETSIZE;fd++){
          if(FD_ISSET(fd,&testfds)){
            //citanie sprav 
            // prijimame data zo socketu
             if(fd==sockfd){  
                //precitame data zo socketu
                result = read(sockfd, msg, MSG_SIZE);
                msg[result] = '\0';  
                printf("%s", msg );
                                        
             }
             // moznost pre klavesnicu, mozeme pisat spravu
             else if(fd == 0){ 
                    fgets(kb_msg, MSG_SIZE+1, stdin);
                    //pre posialnei sprav ako Michal:ahoj
                    char prem[SIZE];
                    memset(prem,0,SIZE);
                    strcpy(prem, name_chat);
                    strcat(prem, ":");
                    strcat(prem,kb_msg);
                    strcat(prem, "\r");
                    write(sockfd, prem, strlen(prem));
                                                                 
                }          
          }
       }      
     }   

}
