/* COP 3502C Assignment 2 THis program is written by: Rayyan Vorajee */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_NAME_SIZE 51
#define MAX_QUEUES 12

typedef struct Customer {
    char name[MAX_NAME_SIZE];
    int numTickets;
    int lineNum;
    int arrivalTime;
} Customer;

typedef struct Node {
    Customer* customerPtr;
    struct Node* next;
} Node;

typedef struct Queue {
    Node* front;
    Node* rear;
    int nodeCount;
} Queue;

// Queue related Functions.
void initializeQueue(Queue* queuePtr);
void enqueue(Queue* queuePtr, Customer* cust);
int isQueueEmpty(Queue* queuePtr);
Customer* peek(Queue* queuePtr);
Customer* dequeue(Queue* queuePtr);
Customer* createCustomer(char* name, int arrivalTime, int numTickets);
//void freeNode(Node* nodePtr);

// Helper functions for processing the customers.
int getNextQueue(Queue queues[], int from, int to);
int calculateCheckoutTime(Customer* customerPtr);
int getQueueMin(Queue queues[]);

// Receive a queuePointer & return the # of nodes in the queue,
int getQueueCount(Queue* queuePtr) {
    return queuePtr->nodeCount;
}
// gets a list of queues & return the queue # with the lowest number of customeer
int getQueueMin(Queue queues[]) {
    int minCount = 500001;
    int minIndex = 0; // 0 is the default if all are empty
    for (int i = 0; i < MAX_QUEUES; i++) {
        if (!isQueueEmpty(&queues[i]) && getQueueCount(&queues[i]) < minCount) {
            minIndex = i;
            minCount = getQueueCount(&queues[i]);
        }
    }
    return minIndex + 1;
}
// Load queues with customers & data from inputs.
void loadQueues(Queue queues[], int numCustomers) {
    // Preload each line!
    for (int i = 0; i < numCustomers; i++) {
        char name[MAX_NAME_SIZE + 1];
        int arrivalTime, numTickets;
        scanf("%s%d%d", name, &numTickets, &arrivalTime);
        Customer* customerPtr = createCustomer(name, arrivalTime, numTickets);
        int lineNum = ((name[0]) - 'A') % 13;
        if (lineNum == 0) {
            lineNum = getQueueMin(queues);
        }
        customerPtr->lineNum = lineNum;

        enqueue(&queues[lineNum - 1], customerPtr);
    }
}
int calculateCheckoutTime(Customer* customerPtr) {
    return 30 + customerPtr->numTickets * 5;
}
void initializeQueue(Queue* queuePtr) {
    queuePtr->front = NULL;
    queuePtr->rear = NULL;
    queuePtr->nodeCount = 0;
}
// Enqueue the customer into queue pointed to by queuePtr.
void enqueue(Queue* queuePtr, Customer* customerPtr) {
    Node* temp = malloc(sizeof(Node));
    temp->customerPtr = customerPtr;
    temp->next = NULL;

    if (isQueueEmpty(queuePtr)) {
        queuePtr->front = temp;
        queuePtr->rear = temp;
    } else {
        queuePtr->rear->next = temp;
        queuePtr->rear = temp;
    }
    (queuePtr->nodeCount)++;
}
// Returns 1 iff the queue pointed to by queuePtr is empty.
int isQueueEmpty(Queue* queuePtr) {
    return queuePtr->front == NULL;
}
// Returns a pointer to the front customer pointed to by the queue pointed
// to by queuePtr. Returns NULL if there is no customer.
Customer* peek(Queue* queuePtr) {
    if (isQueueEmpty(queuePtr))
        return NULL;

    return queuePtr->front->customerPtr;
}
// Dequeue function
Customer* dequeue(Queue* queuePtr) {
    if (isQueueEmpty(queuePtr))
        return NULL;

    Node* temp = queuePtr->front;
    queuePtr->front = queuePtr->front->next;

    if (queuePtr->front == NULL)
        queuePtr->rear = NULL;

    (queuePtr->nodeCount)--;
    Customer* customerPtr = temp->customerPtr;
    free(temp);
    return customerPtr;
}
void freeNode(Node* nodePtr) {
    free(nodePtr->customerPtr);
    free(nodePtr);
}

Customer* createCustomer(char* name, int arrivalTime, int numTickets) {
    Customer* tmp = malloc(sizeof(Customer));
    strcpy(tmp->name, name);
    tmp->arrivalTime = arrivalTime;
    tmp->numTickets = numTickets;
    return tmp;
}
// Receives all the queues & the range of queues to find who arrived first
int getNextQueue(Queue queues[], int from, int to) {
    int res = -1, minArrivalTime = 1000000001;
    // Get the queue for the booth with the least arrival time.
    for (int i = from; i <= to; i++) {
        if (isQueueEmpty(&queues[i]))
            continue;
      
        Customer* tmp = peek(&queues[i]);
        // If this person arrived earlier, update minArrivalTime and res
        if (tmp->arrivalTime < minArrivalTime) {
            res = i;
            minArrivalTime = tmp->arrivalTime;
        }
    }
    return res;
}
int main(void) {
    int numCustomers, numBooths;
    scanf("%d %d", &numCustomers, &numBooths);
    // Create an array for the queues.
    Queue queues[MAX_QUEUES];
    for (int i = 0; i < MAX_QUEUES; i++)
        initializeQueue(&queues[i]);
     loadQueues(queues, numCustomers);
     int nonEmptyQueues[MAX_QUEUES], nonEmptyTracker = 0;
     for (int i = 0; i < MAX_QUEUES; i++) {
        if (!isQueueEmpty(&queues[i]))
            nonEmptyQueues[nonEmptyTracker++] = i;
    }
    // Calculate `d` that represents how many queues each booth will process.
    int d = nonEmptyTracker / numBooths;
    int m = nonEmptyTracker % numBooths; // shows how many booths will get an extra queue.

    int *boothQueueCount = calloc(numBooths, sizeof(int)); //Contains the number of queues to be processed by a booth.
    for (int i = 0; i < numBooths; i++) {
        boothQueueCount[i] += d;
        if (i < m)
            boothQueueCount[i] += 1;
    }

    int lineCountHelper = 0;
    for (int booth = 1; booth <= numBooths; booth++) {
        printf("Booth %d\n", booth);

        // Count the # of customers this booth will process.
        int numCustomersBooth = 0;
        for (int i = 0; i < boothQueueCount[booth - 1]; i++) {
            numCustomersBooth += getQueueCount(&queues[nonEmptyQueues[lineCountHelper + i]]);
        }
        int currentTime = 0;
        for (int i = 0; i < numCustomersBooth; i++) {
            int fromQueue = nonEmptyQueues[lineCountHelper];
            // Get the last queue # from nonEmptyQueues for this booth.
            int toQueue = nonEmptyQueues[lineCountHelper + boothQueueCount[booth - 1] - 1];
            int whichQueue = getNextQueue(queues, fromQueue, toQueue);
            // Dequeue this item from the queue.
            Customer* customerPtr = dequeue(&queues[whichQueue]);

            // Updates starting time, if necessary.
            if (currentTime < customerPtr->arrivalTime)
                currentTime = customerPtr->arrivalTime;
          
            currentTime += calculateCheckoutTime(customerPtr);
            printf("%s from line %d checks out at time %d.\n", customerPtr->name, customerPtr->lineNum, currentTime);
            free(customerPtr);
        }
        printf("\n");
        lineCountHelper += boothQueueCount[booth - 1];
    }
    free(boothQueueCount);
    return 0;
}