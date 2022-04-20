/* Shuvaethy Neill */

//This is the client

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/msg.h>
#include <math.h>

#include "shared_mem.h"

/*Declaring functions*/
void command_prompt(char *command);
int getNumParam();
int isdigit(int command);

int main()
{
    int running = 1;    //used to enter while loop and if 0 then exit
    int msgid_to_calc;  //id of output queue (to server)
    int msgid_from_calc;    //id of input queue (from server)
    struct my_msg_st message, response;     //data to be sent and received
    long int msg_to_receive = 1;    //message received type is 1
    int number;

    /* Set up message queue delivered to server*/
    msgid_to_calc = msgget((key_t)KEY2, 0666 | IPC_CREAT);
    if (msgid_to_calc == -1) {
        fprintf(stderr, "msgget failed with error: %d\n", errno);
        exit(EXIT_FAILURE);
    }

    /* Set up message queue delivered from server*/
    msgid_from_calc = msgget((key_t)KEY1, 0666 | IPC_CREAT);
    if (msgid_from_calc == -1) {
        fprintf(stderr, "msgget failed with error: %d\n", errno);
        exit(EXIT_FAILURE);
    }

    while(running) {
        //Get command from user
        command_prompt(message.data);

        //Check if num argument is needed
        if(strcmp(message.data, "insert") == 0 || strcmp(message.data, "delete") == 0){
            number = getNumParam();
            message.num = number;
        }

        message.my_msg_type = 1;

        //Send message to calculator (server)
        if (msgsnd(msgid_to_calc, &message, sizeof(message), 0) == -1) {
            fprintf(stderr, "msgsnd failed\n");
            exit(EXIT_FAILURE);
        }

        //Read response from calculator (server)
        if (msgrcv(msgid_from_calc, &response, sizeof(response), 0, 0) == -1) {
            fprintf(stderr, "msgrcv failed\n", errno);
            exit(EXIT_FAILURE);
        }

        //Process return message from calculator (server)
        printf ("Message received from calculator: %s\n", response.data);

        //Check if user wants to exit
        if(strcmp(message.data, "exit") == 0){
            //Exit out of while loop to delete queues
            running = 0;
        }
    }

    exit(EXIT_SUCCESS);
}

/*Function that prompts user for command*/
void command_prompt(char *command){
    printf("\nCOMMANDS:\n");
    printf("insert\n");
    printf("delete\n");
    printf("sum\n");
    printf("average\n");
    printf("min\n");
    printf("median\n");
    printf("exit\n");
    printf("\nPlease enter the command you would like to use: ");
    scanf("%s", command);

    //Make sure the argument passed is no more than 35 characters in length (re-prompt)
    while (strlen(command) > 36){
        printf("Please enter a command that is less than 35  characters: \n");
        scanf("%s", command);
    }
}

/*Function that prompts user for command*/
int getNumParam(){
    int num;
    printf("Please enter a number: ");
    scanf("%d", &num);
    return num;
}
