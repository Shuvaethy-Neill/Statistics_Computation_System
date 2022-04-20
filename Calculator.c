/* Shuvaethy Neill 101143478 */

//This is the server

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/msg.h>

#include "shared_mem.h"

#define MICRO_SEC_IN_SEC 1000000

/* Declaring functions */
struct my_msg_st doCommand(int *arr, int *freePosition, int num, char data[], long int *totalTime, int *totalCommands);
void insert(int *arr, int number, int freePosition);
int delete(int *arr, int N, int freePosition);
int sum(int *arr, int freePosition);
float average(int *arr, int freePosition);
int min(int *arr, int freePosition);
void median(int *arr, int freePosition, struct my_msg_st *outcome);
void increasingSort(int *copy, int freePosition);
void printArray (int *arr, int freePosition);

int main()
{
    int arr[ARR_SIZE] = {0}; //array to hold list of numbers user inputs
    int running = 1;    //used to enter while loop and if 0 then exit
    int msgid_to_user;  //id of output queue (to client)
    int msgid_from_user;    //id of input queue (from client)
    int freePosition = 0;   //first index in arr that is not occupied by a user inputted number
    struct my_msg_st message, response; //data to be sent and received
    long int totalTime = 0; //to keep track of avg time
    int totalCommands = 0; //how mnay commands performed

    /* Set up message queue delivered to client */
    msgid_to_user = msgget((key_t)KEY1, 0666 | IPC_CREAT);
    if (msgid_to_user == -1) {
        fprintf(stderr, "msgget failed with error: %d\n", errno);
        exit(EXIT_FAILURE);
    }

    /* Set up message queue delivered from client */
    msgid_from_user = msgget((key_t)KEY2, 0666 | IPC_CREAT);
    if (msgid_from_user == -1) {
        fprintf(stderr, "msgget failed with error: %d\n", errno);
        exit(EXIT_FAILURE);
    }

    printf("\nWaiting to receive...\n");

    while(running) {

        //Read incoming message
        if (msgrcv(msgid_from_user, &message, sizeof(message), 0, 0) == -1) {
            fprintf(stderr, "msgrcv failed with error: %d\n", errno);
            exit(EXIT_FAILURE);
        }

        //Print user inputs
        printf("\nThe command you wrote: %s\n", message.data);
        if(strcmp(message.data, "insert") == 0 || strcmp(message.data, "delete") == 0){
            printf("The number you wrote: %d\n", message.num);
        }

        //Do command based on user input
        response = doCommand(arr, &freePosition, message.num, message.data, &totalTime, &totalCommands);
        response.my_msg_type = 1;

        //Print average time
        printf("The average time to perform a command on the server is %.3f micro sec\n", (totalTime/(totalCommands * 1.0)));

        //Print list after every iteration
        printArray(arr, freePosition);

        //Send message to user (client)
        if (msgsnd(msgid_to_user, &response, sizeof(response),  0) == -1) {
            fprintf(stderr, "msgsnd failed\n");
            perror("msgsnd error");
            exit(EXIT_FAILURE);
        }

        //User wants to exit, exit while loop and delete queues
        if (strcmp(message.data, "exit") == 0){
            running = 0;
        }
    }

    /* Delete the two message queues when user is done */
    if (msgctl(msgid_to_user, IPC_RMID, 0) == -1) {
        fprintf(stderr, "msgctl(IPC_RMID) failed\n");
        exit(EXIT_FAILURE);
    }
    if (msgctl(msgid_from_user, IPC_RMID, 0) == -1) {
        fprintf(stderr, "msgctl(IPC_RMID) failed\n");
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}

/* Function to check which command user has chosen and call the appropriate command function */
struct my_msg_st doCommand(int *arr, int *freePosition, int num, char data[], long int *totalTime, int *totalCommands){

    struct timeval start, end;
    //Used to return a response
    struct my_msg_st outcome;

    //If list is empty, do not allow delete, sum, min, average, or median commands
    if((*freePosition == 0) && (!(strcmp(data, "insert") == 0)) && (!(strcmp(data, "exit") == 0))){
        strcpy(outcome.data, "Empty list! Insert first.");
    }
    //Insert command
    else if(strcmp(data, "insert") == 0){
        gettimeofday(&start, NULL);
        insert(arr, num, *freePosition);
        gettimeofday(&end, NULL);
        (*totalTime) = (*totalTime) + ((end.tv_sec * MICRO_SEC_IN_SEC + end.tv_usec) - (start.tv_sec * MICRO_SEC_IN_SEC + start.tv_usec));
        //Increment freePosition
        (*freePosition) = (*freePosition) + 1;
        strcpy(outcome.data, "Number added.");
    }
    //Delete command
    else if(strcmp(data, "delete") == 0){
        gettimeofday(&start, NULL);
        int temp = delete(arr, num, *freePosition);
        gettimeofday(&end, NULL);
        (*totalTime) = (*totalTime) + ((end.tv_sec * MICRO_SEC_IN_SEC + end.tv_usec) - (start.tv_sec * MICRO_SEC_IN_SEC + start.tv_usec));
        //Argument num is not in list of numbers
        if(temp == 0){
            strcpy(outcome.data, "Not in list! Cannot delete.");
        }
        //Successfully deleted
        else{
            (*freePosition) = (*freePosition) - temp;
            strcpy(outcome.data, "Number deleted.");
        }
    }
    //Sum command
    else if(strcmp(data, "sum") == 0){
        gettimeofday(&start, NULL);
        sprintf(outcome.data, "The sum of all the numbers is %d", sum(arr, *freePosition));
        gettimeofday(&end, NULL);
        (*totalTime) = (*totalTime) + ((end.tv_sec * MICRO_SEC_IN_SEC + end.tv_usec) - (start.tv_sec * MICRO_SEC_IN_SEC + start.tv_usec));
    }
    //Average command
    else if(strcmp(data, "average") == 0){
        gettimeofday(&start, NULL);
        sprintf(outcome.data, "The average is %.2f", average(arr, *freePosition));
        gettimeofday(&end, NULL);
        (*totalTime) = (*totalTime) + ((end.tv_sec * MICRO_SEC_IN_SEC + end.tv_usec) - (start.tv_sec * MICRO_SEC_IN_SEC + start.tv_usec));
    }
    //Min command
    else if(strcmp(data, "min") == 0){
        gettimeofday(&start, NULL);
        sprintf(outcome.data, "The min is %d", min(arr, *freePosition));
        gettimeofday(&end, NULL);
        (*totalTime) = (*totalTime) + ((end.tv_sec * MICRO_SEC_IN_SEC + end.tv_usec) - (start.tv_sec * MICRO_SEC_IN_SEC + start.tv_usec));
    }
    //Median command
    else if(strcmp(data, "median") == 0){
        gettimeofday(&start, NULL);
        median(arr, *freePosition, &outcome);
        gettimeofday(&end, NULL);
        (*totalTime) = (*totalTime) + ((end.tv_sec * MICRO_SEC_IN_SEC + end.tv_usec) - (start.tv_sec * MICRO_SEC_IN_SEC + start.tv_usec));
    }
    //Exit command
    else if(strcmp(data, "exit") == 0){
        strcpy(outcome.data, "Goodbye!");
    }
    //Command is not one of the listed commands
    else{
        strcpy(outcome.data, "That is not a valid");
    }
    (*totalCommands) = (*totalCommands) + 1;
    //Return response
    return outcome;
}

/* Function to insert number in array */
void insert(int *arr, int number, int freePosition){
    arr[freePosition] = number;
}

/* Function to delete every instance of specific number in array */
int delete(int *arr, int N, int freePosition){
    int count = 0;  //how many numbers deleted
    int i;
    int j;

    for(i = 0; i < freePosition; i++){
        if(arr[i] !=  N){
            arr[j] = arr[i];
            j++;
        }
        else{
            count++;
        }
    }
    return count;
}

/* Function to find sum of all numbers in array */
int sum(int *arr, int freePosition){
    int sum = 0;

    for(int i = 0; i < freePosition; i++){
        sum += arr[i];
    }
    return sum;
}

/* Function to find average of numbers in array */
float average(int *arr, int freePosition){
    //Call sum function
    int total_sum = sum(arr, freePosition);
    return (float)total_sum/(freePosition);
}

/* Function to find the min number in array */
int min(int arr[], int freePosition){
    int min = arr[0];
    for(int i = 0; i < freePosition; i++){
        if(min > arr[i]){
            min = arr[i];
        }
    }
    return min;
}

/* Function to find the median of the nnumbers in array */
void median(int *arr, int freePosition, struct my_msg_st *outcome){
    int first = 0;
    int second = 0;

    //Copy array so original is not modified
    int copy[freePosition];
    for(int i = 0; i < freePosition; i++) {
        copy[i] = arr[i];
    }

    //Sort copy array in increasing order
    increasingSort(copy, freePosition);

    //If number of elements are even
    if(freePosition % 2 == 0){
        first = copy[(freePosition-1)/2];
        second = copy[freePosition/2];
        sprintf(outcome->data, "Medians are %d,%d", first, second);
    }
    //If number of elements are odd
    else{
        first = copy[freePosition/2];
        sprintf(outcome->data, "Median is %d", first);
    }

}

/* Function to sort array in ascending order */
void increasingSort(int *copy, int freePosition){
    int temp = 0;
    //Sort copy array in increasing order
    for (int j = 0; j < freePosition; j++) {
        for (int k = j + 1; k < freePosition; k++) {
           if(copy[j] > copy[k]) {
               temp = copy[j];
               copy[j] = copy[k];
               copy[k] = temp;
           }
        }
    }
}

/* Function to sort array in ascending order */
void printArray (int *arr, int freePosition){
    printf("The number set: ");

    for(int i = 0; i < freePosition; i++){
        printf("%d ", arr[i]);
    }
    printf("\n");
}
