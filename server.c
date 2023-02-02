#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define PORT 2024
#define MAX_CLIENTI 9
#define BUFFSIZE 300
#define BUFFCL 1000
int notif(char* buf, char * raspuns, int lungime, int client);

int logged_users=0;

int isLogged(int cl){
    FILE* users=fopen("/home/catalina/rc/mess/users/loggedUsers.txt","r");

    int client;
    char line[100];
    while(!feof(users)){
        fscanf(users,"%d",&client);
        fgets(line,100,users);
        printf("line...%d %s\n",client,line);
        if(cl==client){
            return 1;
        }
    }

    fclose(users);
    return 0;
}

int userExist(char * nume){//user exista?
    FILE* user=fopen("/home/catalina/rc/mess/users/users.txt","r");
    char line[20]; char* numeFile;
    while(!feof(user)){
        fgets(line,20,user);
        numeFile=strtok(line," ");
        printf("nume...%s\n",numeFile);
        if(strcmp(numeFile,nume)==0){
            fclose(user);
            return 1;
        }
    }  
    return 0;
}

int regist(char* buff, char * raspuns, int lungime, int cl){
    if(isLogged(cl)!=0){
        strcpy(raspuns,"Nu puteti sa va inregistrati daca sunteti logati!\n");
        return 0;
    }

    //verificam nume valid
    printf("buff...%s\n",buff);    
    int last=0; int count=0;
    for(int i=0;i<strlen(buff);i++){
        if(buff[i]==' '|| buff[i]=='\n'){
            count++;
        }
    }
    printf("nr cuvinte..%d\n",count);
    if(count>3){
        strcpy(raspuns,"Username nu corespunde formatului\n");
        return 0;
    }    

    char* nume=strtok(buff," ");
    nume=strtok(NULL," ");
    printf("Numele este %s\n",nume);

    char* parola=strtok(NULL,"\n");

    FILE* users=fopen("/home/catalina/rc/mess/users/users.txt","r+");
    char nameLog[50]; char* nm;
    while(fgets(nameLog,50,users)!=NULL){
        nm=strtok(nameLog," ");
        printf("nume...%s\n",nm);
        if(strcmp(nm,nume)==0){
            printf("sunt in if\n");
            strcpy(raspuns,"Sunteti deja inregistrati\n");
            return 0;
        }
        printf("sunt in while\n");
    }
    fseek(users,0,SEEK_END);

    if(strlen(parola)<7){
        strcpy(raspuns,"Parola trebuie sa fie mai lunga de 5 caractere!\n");
        fclose(users);
        return 0;
    }

    if(fprintf(users,"%s %s\n",nume,parola)<0){
        perror("Eroare scriere fisier\n");
        return 0;
    }

    char info[50];
    strcpy(info,"/home/catalina/rc/mess/users/");
    strcat(info,nume);
    strcat(info,".txt");
    FILE* new=fopen(info,"w");
    
    strcpy(raspuns,"V-ati inregistrat cu succes!\n");
    printf("rasp...%s\n",raspuns);

    fclose(users);
    fclose(new);
    return 1;
}

int login(char* buffer, char * raspuns, int lungime, int cl){
    if(isLogged(cl)!=0){
        strcpy(raspuns,"Sunteti deja logati!\n");
        return 1;
    }

    char* nume=strtok(buffer," ");
    nume=strtok(NULL," ");
    char* parola=strtok(NULL,"\n");
    printf("Numele este %s, parola este %s\n",nume,parola);

    if(userExist(nume)==0){
        strcpy(raspuns, "Eroare la logare!\n");
        return 1;
    }  
    int ok=0;

    FILE* users=fopen("/home/catalina/rc/mess/users/users.txt","r+");
    char parolaReala[50]; char* nm; char* pr;
    while(fgets(parolaReala,50,users)!=NULL){
        nm=strtok(parolaReala," ");
        pr=strtok(NULL,"\n");
        printf("nume...%s, parola...%s\n",nm,pr);
        if(strcmp(nm,nume)==0){
            printf("sunt in if\n");
            if(strcmp(pr,parola)==0){
                ok=1;
                printf("parola corecta?\n");
            }
            break;
        }
    }
    
    FILE* logg=fopen("/home/catalina/rc/mess/users/loggedUsers.txt","r+");
    fseek(logg,0,SEEK_END);
    if(ok==1){
        strcpy(raspuns, "Logarea a fost realizata cu succes!\n");
        fprintf(logg,"%i %s\n",cl,nume);
    }else{
        strcpy(raspuns,"Parola incorecta!\n");
    }
    fclose(logg);
    
    return 0;
}

int history(char* buff, char * raspuns, int lungime, int cl){
    if(isLogged(cl)==0){
        strcpy(raspuns,"Nu sunteti logati\n");
        return 0;
    }

    //client
    char *nume=strtok(buff," ");
    nume=strtok(NULL,"\n");

    //verificare user existent
    if(userExist(nume)==0){
        strcpy(raspuns,"User inexistent!\n");
        return 0;
    } 

    FILE* logg=fopen("/home/catalina/rc/mess/users/loggedUsers.txt","r");
    int client;  char* userLogat; char line[100];
    while(!feof(logg)){
        fscanf(logg,"%d",&client);
        fseek(logg,-1,SEEK_CUR);
        fgets(line,100,logg);
        userLogat=strtok(line," ");
        userLogat=strtok(NULL,"\n");
        printf("Fisier..%s, %i\n",userLogat,client);
        if(cl==client){
            printf("nume..%s\n",userLogat);
            break;
        }
    }
    fclose(logg);
    printf("nume user.. %s\n",userLogat);

    char info[100]; 
    strcpy(info,"/home/catalina/rc/mess/users/");
    strcat(info,nume);
    strcat(info,"-");
    strcat(info,userLogat);
    strcat(info,".txt");

    char info1[100]; 
    strcpy(info1,"/home/catalina/rc/mess/users/");
    strcat(info1,userLogat);
    strcat(info1,"-");
    strcat(info1,nume);
    strcat(info1,".txt");


    FILE* history;
    if((history=fopen(info,"r+")) ==NULL){
        if((history=fopen(info1,"r+"))==NULL){
            strcpy(raspuns,"Nu aveti conversatii cu aceasta persoana\n");
            return 0;
        }
    }

    char linie[50]; raspuns[0]='\0';
    while(fgets(linie,50,history)!=NULL){
        strcat(raspuns, linie);
        printf("conv...%s\n",linie);
    }    

    fclose(history);
    return 0;
}

int mess(char* buf, char * raspuns, int lungime, int client){  //mesaj user mesaj
    if(isLogged(client)==0){
        strcpy(raspuns, "Nu sunteti logati\n");
        return 0;
    }

    //client
    char *nume=strtok(buf," ");
    nume=strtok(NULL," ");
    nume=strtok(NULL," ");

    //mesaj
    char* mesaj=strtok(NULL,"\n");
    printf("mesajul... %s\n",mesaj);

    //verificare user existent
    if(userExist(nume)==0){
        strcpy(raspuns,"Acest user este inexistent\n");
        return 0;
    }  

    FILE* logg=fopen("/home/catalina/rc/mess/users/loggedUsers.txt","r");
    int cl;  char* userLogat; char line[100];
    while(!feof(logg)){
        fscanf(logg,"%d",&cl);
        fseek(logg,-1,SEEK_CUR);
        fgets(line,100,logg);
        userLogat=strtok(line," ");
        userLogat=strtok(NULL,"\n");
        printf("Fisier..%s, %i\n",userLogat,cl);
        if(cl==client){
            printf("nume..%s\n",userLogat);
            break;
        }
    }
    fclose(logg);
    printf("nume user.. %s\n",userLogat);

    //deschidere fisiere
    printf("Deschid fisier\n");

    char info[100]; 
    strcpy(info,"/home/catalina/rc/mess/users/");
    strcat(info,nume);
    strcat(info,"-");
    strcat(info,userLogat);
    strcat(info,".txt");
    printf("am scris link.. %s\n",info);

    char info1[100]; 
    strcpy(info1,"/home/catalina/rc/mess/users/");
    strcat(info1,userLogat);
    strcat(info1,"-");
    strcat(info1,nume);
    strcat(info1,".txt");


    FILE* text; int ok=0;
    if((text=fopen(info,"r+")) ==NULL){
        if((text=fopen(info1,"r+"))==NULL){
            if((text=fopen(info,"w"))==NULL){
                perror("deschidere fisier\n");
            }else{
                ok=1;
            }
        }
    }

    char info2[100];
    strcpy(info2,"/home/catalina/rc/mess/users/");
    strcat(info2,nume);
    strcat(info2,".txt");
    printf("continut file %s\n",info2);
    FILE* text2 = fopen(info2,"r+");
    if(text2 == NULL){
        if((text2=fopen(info2,"w"))==NULL){
            perror("Eroare\n");
        }
    }

    printf("Am deschis fisiere\n");

    //scriere in fisiere
    char mesajComplet[100];
    strcpy(mesajComplet,userLogat);
    strcat(mesajComplet," : ");
    strcat(mesajComplet,mesaj);

    int nr=0;
    if(ok==0){
        while(!feof(text)){
            fscanf(text,"%d",&nr);
            fgets(line,100,text);
        }
        nr++;
    }else{
        nr=1;
    }

    fseek(text,0,SEEK_END);
    fseek(text2,0,SEEK_END);
    
    if(fprintf(text,"%d | %s\n",nr,mesajComplet)<0){
        perror("Eroare fisier\n");
    }
    if(fprintf(text2,"%d | %s\n",nr,mesajComplet)<0){
        perror("Eroare fisier\n");
    }  

    printf("Scriem clientului\n");

    strcpy(raspuns,"Mesajul a fost trimis cu succes!\n");     

    fclose(text);
    fclose(text2);

    return 0;    
}

int notif(char* buf, char * raspuns, int lungime, int client){
    if(isLogged(client)==0){
        strcpy(raspuns, "Nu sunteti logati\n");
        return 0;
    }

    //cautare nume client
    FILE* logg=fopen("/home/catalina/rc/mess/users/loggedUsers.txt","r");
    int cl;  char* userLogat; char line[20];
    while(!feof(logg)){
        fscanf(logg,"%d",&cl);
        fseek(logg,-1,SEEK_CUR);
        fgets(line,20,logg);
        userLogat=strtok(line," ");
        userLogat=strtok(NULL,"\n");
        printf("Fisier..%s, %i\n",userLogat,cl);
        if(cl==client){
            printf("nume..%s\n",userLogat);
            break;
        }
    }

    fclose(logg);
    printf("nume user.. %s\n",userLogat);

    // deschidere fisier cu notificari user
    char link[100];
    strcpy(link,"/home/catalina/rc/mess/users/");
    strcat(link,userLogat);
    strcat(link,".txt");
    printf("link...%s\n",link);
    FILE* text = fopen(link,"r");
    if(text==NULL){
        perror("Nu aveti notificari\n");
        return 0;
    }

    printf("scriem fisier\n");
    //trimitere mesaje
    char linie[50]; int i=0;
    while(fgets(linie,50,text)!=NULL){
        if(i==0 && linie[0]=='\0'){
            strcat(raspuns, "Nu\n");
        }else if(i==0 && linie[0]!='\0'){
            strcat(raspuns,"Aveti urmatoarele notificari noi:\n");
        }
        i++;
        strcat(raspuns, linie);
        printf("notif...%s\n",linie);
    }
    printf("redenumim fisierul\n");

    fclose(text);    
    FILE* new =fopen(link,"w");
    fclose(new);

    return 0;
}

int reply(char* buf, char * raspuns, int lungime, int client){
    if(isLogged(client)==0){
        strcpy(raspuns, "Nu sunteti logati\n");
        return 0;
    }

    //client
    char *nume=strtok(buf," ");
    nume=strtok(NULL," ");

    //nr mesaj
    char* nrMesaj=strtok(NULL," ");

    //mesaj
    char* mesaj=strtok(NULL,"\n");
    printf("nume...%s, nr... %s,, mesaj.. %s\n",nume,nrMesaj,mesaj);//48 57

    //verificare user existent
    if(userExist(nume)==0){
        strcpy(raspuns,"User inexistent\n");
        return 0;
    }

    FILE* logg=fopen("/home/catalina/rc/mess/users/loggedUsers.txt","r");
    int cl;  char* userLogat; char line[100];
    while(!feof(logg)){
        fscanf(logg,"%d",&cl);
        fseek(logg,-1,SEEK_CUR);
        fgets(line,100,logg);
        userLogat=strtok(line," ");
        userLogat=strtok(NULL,"\n");
        printf("Fisier..%s, %i\n",userLogat,cl);
        if(cl==client){
            printf("nume..%s\n",userLogat);
            break;
        }
    }
    fclose(logg);
    printf("nume user.. %s\n",userLogat);

    printf("Deschid fisier\n");
    int nr=0;

    char info[100]; 
    strcpy(info,"/home/catalina/rc/mess/users/");
    strcat(info,nume);
    strcat(info,"-");
    strcat(info,userLogat);
    strcat(info,".txt");
    printf("am scris link.. %s\n",info);

    char info1[100]; 
    strcpy(info1,"/home/catalina/rc/mess/users/");
    strcat(info1,userLogat);
    strcat(info1,"-");
    strcat(info1,nume);
    strcat(info1,".txt");

    FILE* text;
    if((text=fopen(info,"r+")) ==NULL){
        if((text=fopen(info1,"r+"))==NULL){
            strcpy(raspuns,"Nu exista conversatii cu aceasta persoana");
            return 0;
        }
    }

    char info2[100];
    strcpy(info2,"/home/catalina/rc/mess/users/");
    strcat(info2,nume);
    strcat(info2,".txt");
    printf("continut file %s\n",info2);
    FILE* text2 = fopen(info2,"r+");
    if(text2 == NULL){
        if((text2=fopen(info2,"w"))==NULL){
            perror("Eroare\n");
        }
    }

    printf("Am deschis fisiere\n");
    
    int bun=0; char linie[100];
    while(!feof(text)){
        fscanf(text,"%d",&nr);
        fgets(linie,100,text);
        if(nr==atoi(nrMesaj)){
            bun=1;
        }
    }
    if(bun==0){
        strcpy(raspuns,"Nu exista mesaj cu acest numar\n");
        return 0;
    }
    nr++;
    
    fseek(text,0,SEEK_END);
    fseek(text2,0,SEEK_END);

    char mesajComplet[100];
    strcpy(mesajComplet,"[reply ");
    strcat(mesajComplet,nrMesaj);
    strcat(mesajComplet,"]");
    strcat(mesajComplet,userLogat);
    strcat(mesajComplet," : ");
    strcat(mesajComplet,mesaj);
    
    if(fprintf(text,"%d | %s\n",nr,mesajComplet)<0){
        perror("Eroare fisier\n");
    }
    if(fprintf(text2,"%s\n",mesajComplet)<0){
        perror("Eroare fisier\n");
    }  

    printf("Scriem clientului\n");
    strcpy(raspuns,"Mesajul a fost trimis cu succes!\n");     

    fclose(text);
    fclose(text2);

    return 0;
}

int logout(char* buf, char * raspuns, int lungime, int cl){
    if(isLogged(cl)==0){
        strcpy(raspuns, "Nu sunteti logati\n");
        return 0;
    }
     
    FILE* logg=fopen("/home/catalina/rc/mess/users/loggedUsers.txt","r");
    FILE* temp=fopen("/home/catalina/rc/mess/users/logged.txt","w");

    char line[50];

    int nr;
    while(!feof(logg)){
        fscanf(logg,"%d",&nr);
        fseek(logg,-1,SEEK_CUR);
        fgets(line,50,logg);
        printf("nr..%d, nume..%s",nr,line);
        if(line[1]=='\0') break;

        if(nr!=cl){
            fprintf(temp,"%s",line);
        }
    }
    strcpy(raspuns,"Logout a fost realizata cu succes\n");

    fclose(logg);
    fclose(temp);

    remove("/home/catalina/rc/mess/users/loggedUsers.txt");
    rename("/home/catalina/rc/mess/users/logged.txt","/home/catalina/rc/mess/users/loggedUsers.txt");
    return 0;
}

int quit(char* buf, char * raspuns, int lungime, int cl){
    if(isLogged(cl)!=0){
        strcpy(raspuns,"Nu v-ati delogat!\n");
        return 0;
    }
    strcpy(raspuns,"QUIT");
    return 1;
}

int procesareCerere(char* buf, char * rasp, int lungime, int cl){
    printf("procesam cererea\n");
    if(lungime==0){
        strcpy(rasp,"Nu am primit cerere\n");
    }
    else if(strstr(buf,"inregistrare")!=NULL){
        regist(buf, rasp, lungime,  cl);
    }
    else  if(strstr(buf,"login")!=NULL){
        login(buf, rasp, lungime,  cl);
    }
    else if(strstr(buf,"history")!=NULL){
        history(buf, rasp, lungime,  cl);
    }
    else if(strstr(buf,"text to")!=NULL){
        mess(buf, rasp, lungime,  cl);
    }
    else if(strstr(buf,"reply")!=NULL){
        reply(buf, rasp, lungime,  cl);
    }
    else if(strstr(buf,"notificari")!=NULL){
        notif(buf,rasp,lungime,cl);
    }
    else if(strstr(buf,"logout")!=NULL){
        logout(buf, rasp, lungime,  cl);
    }
    else if(strstr(buf, "quit")!=NULL){
        if(quit(buf, rasp, lungime,  cl)==1){
            return 1;
        }else{
            return 0;
        }
    }
    else{
        strcat(rasp,"Comanda este invalida\n");
    }
    printf("Am procesat!\n");
    return 0;

}


int main(){
    struct sockaddr_in server;
    struct sockaddr_in client;
    char buff[BUFFSIZE];
    char buffcl[BUFFCL];

    int sd;

    if((sd=socket(AF_INET,SOCK_STREAM,0))==-1){
        perror("Eroare socket\n");
        return 1;
    }

    bzero(&server,sizeof(server));
    bzero(&client,sizeof(client));

    server.sin_family=AF_INET;
    server.sin_addr.s_addr=htonl(INADDR_ANY);
    server.sin_port=htons(PORT);

    if(bind(sd,(struct sockaddr *)&server, sizeof(struct sockaddr))==-1){
        perror("Eroare bind\n");
        return 2;
    }

    if(listen(sd,2)==-1){
        perror("Eroare liste\n");
        return 3;
    }

    while(1){
        int cl;
        int lenght=sizeof(client);

        printf("Asteptam la portul %i\n",PORT);
        fflush(stdout);

        if((cl=accept(sd,(struct sockaddr *) &client,&lenght))==-1){
            perror("Eroare la accept\n");
            continue;
        }

        printf("client...%i\n",cl);
        int check=0;

        int pid;
        if((pid=fork())==-1){
            perror("Eroare fork\n");
            return 5;
        }else if(pid==0){            
            while(1){

                bzero(buff,BUFFSIZE);
                printf("Asteptam mesajul...\n");
                fflush(stdout);
                if(read(cl,buff,BUFFSIZE)<=0){
                    perror("Eroare la citire\n");
                    close(cl);
                    return 1;
                }

                printf("Mesajul a fost receptionat...%s\n",buff);

                bzero(buffcl,BUFFCL);
                check= procesareCerere(buff,buffcl,sizeof(buff),cl);

                printf("Raspunsul e ...%s\n",buffcl);

                if(write(cl,buffcl,BUFFCL)<=0){
                    perror("Eroare la write catre client\n");
                    return 1;
                }else{
                    printf("Mesajul a fost trimis cu succes\n");
                }

                if(strstr(buff,"quit")!=NULL && check==1){
                    printf("Clientul %i a fost deconectat\n",cl);
                    close(cl);
                    return 0;
                }
            }
        }else{
            continue;
        }
    }
}
