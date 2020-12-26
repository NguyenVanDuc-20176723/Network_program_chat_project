#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "../symtab/object_data.h"
#include "../symtab/error_invalid.h"
// #include "../symtab/database.h"
#include "jsonapi.h"
#define QUEUE_MAX 10
//test
typedef struct {
    char username[30];
    int sockfd;
}Elementtype;
typedef struct Node_ {
    Elementtype element;  
    struct Node_ *next;
}Node;


Node *new_node(Elementtype e){
    Node *new = (Node*)malloc(sizeof(Node));
    new->element = e;
    new->next = NULL;
    return new;
}

Node *insert_at_end(Node *root, Elementtype element){
    Node *new = new_node(element);
    if(root == NULL)
        return new;
    Node *cur = root;
    while(cur->next != NULL)
        cur = cur->next;
    cur->next = new;

    return root;
}

void printList(Node *root){
    if(root == NULL){
        printf("ds rong\n");
        return;
    }
    Node *cur = root;
    while(cur != NULL){
        printf("username: %s\tsockID: %d\n",cur->element.username,cur->element.sockfd);
        cur = cur->next;
    }
    

}
//
int checkNumber(char* str);

int main(int argc, char **argv){

    int sockfd, connfd, port, recvBytes;
    struct sockaddr_in servaddr, clieaddr;
    socklen_t len_serv, len_clie;
    int selectfd, maxfd, maxi = -1;
    int i = 0;
    fd_set readfds, masterfds;
    int client[QUEUE_MAX];

    //test
    Node *root = NULL;
    // List_DB *root_db = NULL;
    //
    if(argc != 2){
        printf("Error: sai cu phap\n");
        printf("Cu phap: ./server <port-number>\n");
        return 0;
    }
    
    if(checkNumber(argv[1]) == 0){
        printf("Error: parameter1 (port) phai la so\n");
        return 0;
    }

    port = atoi(argv[1]);
    for(; i < QUEUE_MAX; i++){
        client[i] = -1;
    }
    //create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0){
        perror("Error: create socket !");
        return 0;
    }
    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");//htonl(INADDR_ANY);

    len_serv = sizeof(servaddr);

    if(bind(sockfd, (struct sockaddr*)&servaddr,len_serv) < 0){
        perror("Error: bind!");
        return 0;
    }
    if(listen(sockfd,QUEUE_MAX) < 0){
        perror("Error: listen!");
        return 0;
    }

    FD_ZERO(&masterfds);
    FD_ZERO(&readfds);
    FD_SET(sockfd,&masterfds);
    maxfd = sockfd;

    puts("Server running...");

    do{
        memcpy(&readfds,&masterfds,sizeof(masterfds));
        if((selectfd = select(maxfd +1,&readfds,NULL,NULL,NULL)) < 0){
            perror("select");
            return 0;
        }else if(selectfd == 0){
            printf("timeout\n");
            sleep(3);
        }else{
            if(FD_ISSET(sockfd,&readfds)){
                int index_clie;             
                len_clie = sizeof(clieaddr);
                if((connfd = accept(sockfd,(struct sockaddr*)&clieaddr,&len_clie)) < 0){
                    perror("Error: accept! ");
                    return 0;
                }
                Elementtype *element = (Elementtype*)malloc(sizeof(Elementtype));
                element->sockfd = connfd;
                printf("sock %d\n",element->sockfd);
                root = insert_at_end(root,*element);
                printList(root);
                printf("\n");
                printf("New connect with client %d\n",connfd);              
                index_clie = connfd - sockfd - 1;
                if(index_clie == QUEUE_MAX){
                    printf("Server day!\n");   
                }else{
                    if(client[index_clie] == -1){
                        client[index_clie] = connfd;
                    }
                }
                FD_SET(connfd,&masterfds);
                if(connfd > maxfd){
                        maxfd = connfd;
                }
                if(index_clie > maxi) 
                        maxi = index_clie;
            }
            for(i = 0; i<= maxi;i++){
                int sock_cl;
                if((sock_cl = client[i]) < 0) continue;
                if(FD_ISSET(sock_cl,&readfds)){                  
                    Object *obj = (Object*)malloc(sizeof(Object));                   
                    if((recvBytes = recv(sock_cl,obj,sizeof(Object),0)) < 0){
                        printf("ERROR! In socket %d\n",sock_cl);
                        perror("recv - message");
                        client[i] = -1;
                        FD_CLR(sock_cl,&masterfds);
                        close(sock_cl);
                        continue;
                    }                 
                    switch(obj->signal){
                        case SIGNAL_LOGIN:
                        
                            printf("------------LOGIN-------------\n");
                            {
                                Node *cur_login = root;
                                while(cur_login != NULL){
                                    if(cur_login->element.sockfd == sock_cl){
                                        strcpy(cur_login->element.username, obj->login.username);
                                        break;
                                    }
                                    cur_login = cur_login->next;
                                }
                            }
                            int check_login = -1;
                             Error err_login;
                                user_db userdb;
                                char msg_err_login[100];
                            printf("Account: %s\n",obj->login.username);
                            printf("Password: %s\n\n",obj->login.password);  
                            //check loi
                             
                                check_login = check_user(obj->login.username); 
            
                                printf("%d", check_login);
                              
                                if(check_login == 1){
                                    userdb = getUser(obj->login.username);
                                    if(strcmp(userdb.password, obj->login.password) != 0){ // sai pass
                                        err_login = ERR_CAN_NOT_PASSWORD;
                                    }else{ //dang nhap thanh cong
                                        if(userdb.login_status == 0){
                                            err_login = ERR_NONE;
                                            userdb.login_status = 1;
                                        }else{
                                            err_login = ERR_USERNAME_LOGIN;
                                        }
                                    }

                                }else{ //sai username
                                    err_login = ERR_NOT_USERNAME;
                                }
                                error_to_string(err_login,msg_err_login);
                                if(send(sock_cl,msg_err_login,strlen(msg_err_login),0) < 0){
                                    printf("ERROR! In socket %d\n",sock_cl);
                                    perror("send - msg_err login");
                                    client[i] = -1;
                                    FD_CLR(sock_cl,&masterfds);
                                    close(sock_cl);
                                    continue;
                                }
                                             
                            break;
                        case SIGNAL_SIGNUP:                          
                            // printf("------------SIGNUP-----------\n");
                            // printf("Name: %s\n",obj->signup.name);
                            // printf("Account: %s\n",obj->signup.username);
                            // printf("Password: %s\n",obj->signup.password);
                            // printf("Confirm Password: %s\n\n",obj->signup.re_password);
                     
                            // //Data_base *db = (Data_base*)malloc(sizeof(Data_base));
                            // Error err = ERR_NONE;
                            // char msg_err[100];
                       
                            // strcpy(db->user.name,obj->signup.name);
                            // strcpy(db->user.username,obj->signup.username);
                            // strcpy(db->user.password,obj->signup.password);
                            // db->user.login_status = false;
                            // db->user.is_admin = false;
                            // if(root_db == NULL)
                            //     root_db = insert_at_end_db(root_db,db);
                            // else{
                            //     // List_DB *cur_db = root_db;
                            //     bool check = false;
                            //     while(cur_db != NULL){
                            //         if(strcmp(obj->signup.username,cur_db->DB->user.username) == 0){
                            //             check = true;
                            //             break;
                            //         }
                            //         cur_db = cur_db->next;
                            //     }
                            //     if(check == true){
                            //         err = ERR_HAS_USERNAME;
                            //     }else{
                            //         root_db = insert_at_end_db(root_db,db);
                            //     }
                            // }
                            // error_to_string(err,msg_err);
                            // if(send(sock_cl,msg_err,strlen(msg_err),0) < 0){
                            //     printf("ERROR! In socket %d\n",sock_cl);
                            //     perror("send - err");
                            //     client[i] = -1;
                            //     FD_CLR(sock_cl,&masterfds);
                            //     close(sock_cl);
                            //     continue;
                            // }
                            
                            // //obj->signals = SIGNAL_NONE;
                            break;
                        case SIGNAL_CHAT_PRIVATE:
                            
                            // printf("%s : %s\n",obj->chat_private.from_username,obj->chat_private.message);
                            // //luu db

                            // Node *cur_chat = root;
                            // Object *obj_chat_private;
                            // while(cur_chat != NULL){
                            //     if(strcmp(cur_chat->element.username,obj->chat_private.from_username) != 0){
                            //         //char message_[250];
                            //         //sprintf(message_,"[%s]:\n%s",obj->chat_private.from_username,obj->chat_private.message);
                            //         obj_chat_private = duplicate_object(obj);
                            //         if(send(cur_chat->element.sockfd,obj_chat_private, 
                            //             sizeof(Object),0)<0){
                            //             printf("ERROR! In socket %d\n",sock_cl);
                            //             perror("recv - message");
                            //             client[i] = -1;
                            //             FD_CLR(sock_cl,&masterfds);
                            //             close(sock_cl);
                            //             return 0;
                            //         }
                            //     }
                            //     cur_chat = cur_chat->next;
                            // }
                            
                            break;
                        case SIGNAL_CHANGE_PASSWORD:
                            // printf("------Change password-----------\n");
                            // printf("Username: %s\n",obj->change_password.username);
                            // printf("New Password: %s\n",obj->change_password.new_password);
                            // // List_DB *db_change_pass = root_db;
                            
                            // while(db_change_pass != NULL){
                            //     if(strcmp(obj->change_password.username,db_change_pass->DB->user.username) == 0){
                            //         strcpy(db_change_pass->DB->user.password,obj->change_password.new_password);
                            //         break;
                            //     }
                            //     db_change_pass = db_change_pass->next;
                            // }
                            break;
                        case SIGNAL_ADD_FRIEND:
                            
                            break;
                        case SIGNAL_NONE:
                            break;
                    }
                    free_object(obj);
                  
                }
            }
            
        }
    }while(1);
    puts("server closed!\n");
    //free_list_db(root_db);
    close(sockfd);
    return 0;
}

int checkNumber(char* str){
    int i;
    if(strlen(str) == 0) return 0;
    for(i = 0; i< strlen(str); i++){
        if(!(str[i] >= '0' && str[i] <= '9'))
            return 0;
    }
    return 1;
}