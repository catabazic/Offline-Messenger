#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>

#define PORT 2024
#define  BUFFSIZE 1000
int logged=0;
pthread_mutex_t lock;

void *treat(void *arg){
    int sd= *(int*)arg;
    fflush(stdout);
    char buffcl[BUFFSIZE];

    while(logged==1){
        pthread_mutex_lock(&lock);
        if(write(sd,"notificari",100)<=0){
            perror("Eroare la write spre server\n");
        }
        bzero(buffcl,BUFFSIZE);

        if(read(sd,buffcl,BUFFSIZE)<0){
            perror("Eroare la read\n");
        }
        if(strlen(buffcl)>5){
            printf("%s\n",buffcl);
        }
        pthread_mutex_unlock(&lock);
        sleep(10);
    }
    pthread_detach(pthread_self());
    return(NULL);
}

void welcome(){
    printf("-----------------------------------------------------------\n");
    printf("--------------------Offline messenger----------------------\n");
    printf("-----------------------------------------------------------\n");
    printf("Aveti la dispozitie urmatoarele comenzi:\n\n");
    printf(">inregistrare [nume user] [parola]\n");
    printf(">login [nume user] [parola]\n");
    printf(">text to [nume user] [continut text]\n");
    printf(">reply [nume user] [numar mesaj] [continut text]\n");
    printf(">history [nume user]\n");
    printf(">logout\n");
    printf(">quit\n");
    printf("-----------------------------------------------------------\n");
    printf("Constrangeri:\n");
    printf(">[nume user] poate fi format doar dintr-un cuvant\n");
    printf(">nu trebuie sa existe spatii libere suplimentare");
    printf(">parola trebuie sa fie formata din mai mult de 5 caractere\n");

    printf("\n\n");
}

int main(){
    int sd;
    struct sockaddr_in server;
    char buff[100]; char buffcl[BUFFSIZE];

    if((sd=socket(AF_INET, SOCK_STREAM,0))==-1){
        perror("Eroare la socket\n");
        return 1;
    }

    server.sin_family=AF_INET;
    server.sin_addr.s_addr=inet_addr("10.0.2.15");
    server.sin_port=htons(PORT);

    if(connect(sd,(struct sockaddr*)&server,sizeof(struct sockaddr))==-1){
        perror("Eroare la conectare\n");
        return 2;
    }

    fflush(stdout);
    welcome();

    while(1){

        bzero(buff,100);
        read(0,buff,100);

        if(write(sd,buff,100)<=0){
            perror("Eroare la write spre server\n");
            return 3;
        }
        bzero(buffcl,BUFFSIZE);

        if(read(sd,buffcl,BUFFSIZE)<0){
            perror("Eroare la read\n");
            return 4;
        }
        printf("%s\n",buffcl);

        if(strstr(buffcl,"Logarea a fost realizata cu succes!")!=NULL){
            logged=1;
            pthread_t th;
            int *a=malloc(sizeof(sd));
            *a=sd;
            if(pthread_create(&th,NULL,&treat,a)!=0){
                perror("Eroare thread\n");
            }
        }
        if(strstr(buffcl,"Logout a fost realizata cu succes")!=NULL){
            logged=0;
        }
        if(strstr(buff,"quit")!=NULL && strstr(buffcl,"QUIT")!=NULL){
            break;
        }
    }
    close(sd);
}
