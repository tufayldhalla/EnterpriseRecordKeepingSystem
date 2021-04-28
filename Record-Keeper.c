#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <sys/msg.h>

//Data Packet to send/receive to/from server
typedef struct{
    int userChoice; //1 - insert record, 2 - check name, 3 - check department, 4 - check salary, 5 - check employee number, 6 - list all employee numbers in certain department, 7 - delete record
    //pid of client
    pid_t pid;
    char name[13];
    char departmentName[13];
    long long int empNum;
    long long int empNums[13];
    int numOfEmps;
    long long int salary;
    //if check/insertion/deletion was a success
    bool success;
}packet;

//Message to send/receive to/from  server
typedef struct {
    long int key;
    packet finalPacket;
}myMSG;

//Database record
typedef struct  {
	char name[13];
	char departmentName[13];
	long long int empNum;
	long long int empNums[13];
	long long int salary;
}record;

//Linked List node
struct node{
    record data;
    struct node *next;
};

//Node type
typedef struct node *node_t;

//Start of linked list
node_t head;

//Function declarations
void insert(packet receivedPacket);
void checkName(packet receivedPacket);
void checkDepartment(packet receivedPacket);
void checkSalary(packet receivedPacket);
void checkEmployeeNumber(packet receivedPacket);
void checkEmployeesInDepartment(packet receivedPacket);
void delete(packet receivedPacket);

static int Rmsgid; //receive (server) message queue
static int Smsgid; //send (client) message queue
static int running = 1;

int main()
{
	//Server Message Queue Initialization
	Rmsgid = msgget((key_t)1235, 0666 | IPC_CREAT);
	if (Rmsgid == -1) {
		fprintf(stderr, "msgget failed with error: %d\n", errno);
		exit(EXIT_FAILURE);
	}

	//Client Message Queue Initialization
	Smsgid = msgget((key_t)1234, 0666 | IPC_CREAT);
	if (Smsgid == -1) {
		fprintf(stderr, "msgget failed with error: %d\n", errno);
		exit(EXIT_FAILURE);
	}
	
    //data packet initilization
	packet receivedPacket;
	
	while(running) {
		//Receiving message from client
		myMSG msgReceived;
		if (msgrcv(Rmsgid, (void *)&msgReceived, sizeof(receivedPacket), 0, 0) == -1) {
			fprintf(stderr, "msgrcv failed with error: %d\n", errno);
			exit(EXIT_FAILURE);
		}
		receivedPacket = msgReceived.finalPacket;
        //decrypt packet and see what option the user chose
		if (receivedPacket.userChoice == 1) { // INSERT RECORD
		    insert(receivedPacket);
		}else if (receivedPacket.userChoice  == 2) { // CHECK NAME
		    checkName(receivedPacket);
		}else if (receivedPacket.userChoice  == 3) { //CHECK DEPARTMENT
		    checkDepartment(receivedPacket);
		}else if (receivedPacket.userChoice  == 4) { //CHECK SALARY
		    checkSalary(receivedPacket);
		}else if (receivedPacket.userChoice  == 5) { //CHECK EMPLOYEE NUMBER
		    checkEmployeeNumber(receivedPacket);
		}else if (receivedPacket.userChoice  == 6) { //CHECK EMPLOYEES IN DEPARTMENT
		    checkEmployeesInDepartment(receivedPacket);
		}else if (receivedPacket.userChoice  == 7) { //DELETE RECORD
		    delete(receivedPacket);
		}
	}

	if (msgctl(Smsgid, IPC_RMID, 0) == -1) {
		fprintf(stderr, "1.msgctl(IPC_RMID) failed\n");
		exit(EXIT_FAILURE);
	}
    if (msgctl(Rmsgid, IPC_RMID, 0) == -1) {
        fprintf(stderr, "2.msgctl(IPC_RMID) failed\n");
        exit(EXIT_FAILURE);
    }
	exit(EXIT_SUCCESS);
}

//Create node class
node_t createNode(){
    node_t temp;
    temp = (node_t)malloc(sizeof(struct node));
    temp->next = NULL;
    return temp;
}

//Insert a node into the database (linked list)
void insert(packet receivedPacket) {
    //case if empty list (no records in database)
	if (head == NULL) {
        //create node
		node_t node1 = createNode();
        //make head point to front of list
		head = node1;
        //enter data for that record
		strcpy(node1->data.name, receivedPacket.name);
		strcpy(node1->data.departmentName, receivedPacket.departmentName);
		node1->data.empNum = receivedPacket.empNum;
		node1->data.salary = receivedPacket.salary;
	}
    //if list is not empty
	else {
        //traverse to end of list and insert record at end of list
		node_t nodeY = createNode();
		nodeY = head;
		node_t nodeX = createNode();
		while (nodeY->next != NULL) {
			nodeY = nodeY->next;
		}
        //enter data for that record
		strcpy(nodeX->data.name, receivedPacket.name);
		strcpy(nodeX->data.departmentName, receivedPacket.departmentName);
		nodeX->data.empNum = receivedPacket.empNum;
		nodeX->data.salary = receivedPacket.salary;
		nodeY->next = nodeX;
	}
    //insertion successful
	receivedPacket.success = 1;
    //Send message back to client
    myMSG msgSent;
    msgSent.key = receivedPacket.pid;
    msgSent.finalPacket = receivedPacket;
    if (msgsnd(Smsgid, (void *)&msgSent, sizeof(receivedPacket), 0) == -1) {
		fprintf(stderr, "msgsnd failed\n");
		exit(EXIT_FAILURE);
	}
}

//Check certain name of employee in database given employee number
void checkName(packet receivedPacket) {
	bool correct = 0;
    //case if empty list
    if (head == NULL) {
    	receivedPacket.success = 0;
	}
    //case if not empty list
	else {
        //traverse through list to try to find a match
		node_t nodeN = createNode();
		nodeN = head;
		while (nodeN != NULL) {
			if (nodeN->data.empNum == receivedPacket.empNum) {
				correct = 1;
				break;
			}
			nodeN = nodeN->next; 
		}
        //if match find tell client successful and send name of employee back
		if (correct) {
			strcpy(receivedPacket.name,nodeN->data.name);
			receivedPacket.success = 1;
		}
        //else tell client unsuccessful
		else {
			receivedPacket.success = 0;
		}
	}
    //Send message to client
    myMSG msgSent;
    msgSent.key = receivedPacket.pid;
    msgSent.finalPacket = receivedPacket;
    if (msgsnd(Smsgid, (void *)&msgSent, sizeof(receivedPacket), 0) == -1) {
		fprintf(stderr, "msgsnd failed\n");
		exit(EXIT_FAILURE);
	}
}

//Check certain department name of employee in database given employee number
void checkDepartment(packet receivedPacket) {
	bool correct = 0;
    //case if empty list
    if (head == NULL) {
    	receivedPacket.success = 0;
	}
    //case if not empty list
	else {
        //traverse through list to try to find a match
		node_t nodeN = createNode();
		nodeN = head;
		while (nodeN != NULL) {
			if (nodeN->data.empNum == receivedPacket.empNum) {
				correct = 1;
				break;
			}
			nodeN = nodeN->next; 
		}
        //if match find tell client successful and send name of department back
		if (correct) {
			strcpy(receivedPacket.departmentName, nodeN->data.departmentName);
			receivedPacket.success = 1;
		}
        //else tell client unsuccessful
		else {
			receivedPacket.success = 0;
		}
	}
    //Send message to client
    myMSG msgSent;
    msgSent.key = receivedPacket.pid;
    msgSent.finalPacket = receivedPacket;
    if (msgsnd(Smsgid, (void *)&msgSent, sizeof(receivedPacket), 0) == -1) {
		fprintf(stderr, "msgsnd failed\n");
		exit(EXIT_FAILURE);
	}
}

//Check certain salary of employee in database given employee number
void checkSalary(packet receivedPacket) {
	bool correct = 0;
    //case if empty list
    if (head == NULL) {
    	receivedPacket.success = 0;
	}
    //case if not empty list
	else {
        //traverse through list to find a match
		node_t nodeN = createNode();
		nodeN = head;
		while (nodeN != NULL) {
			if (nodeN->data.empNum == receivedPacket.empNum) {
				correct = 1;
				break;
			}
			nodeN = nodeN->next; 
		}
        //if match found add salary of employee in packet
		if (correct) {
			receivedPacket.salary = nodeN->data.salary;
			receivedPacket.success = 1;
		}
		else {
			receivedPacket.success = 0;
		}
	}
    //send message to client
    myMSG msgSent;
    msgSent.key = receivedPacket.pid;
    msgSent.finalPacket = receivedPacket;
    if (msgsnd(Smsgid, (void *)&msgSent, sizeof(receivedPacket), 0) == -1) {
		fprintf(stderr, "msgsnd failed\n");
		exit(EXIT_FAILURE);
	}
}

//Check certain number of employee in database given employee name
void checkEmployeeNumber(packet receivedPacket) {
	bool correct = 0;
    //case if empty list
    if (head == NULL) {
    	receivedPacket.success = 0;
	}
    //case if not empty list
	else {
        //traverse through list to try to find a match
		node_t nodeN = createNode();
		nodeN = head;
		while (nodeN != NULL) {
			if (strcmp(nodeN->data.name, receivedPacket.name) == 0) {
				correct = 1;
				break;
			}
			nodeN = nodeN->next; 
		}
        //if match found then store employee number in packet
		if (correct) {
			receivedPacket.empNum = nodeN->data.empNum;
			receivedPacket.success = 1;
		}
        //if not then unsuccessfull
		else {
			receivedPacket.success = 0;
		}
	}
    //send message to client
    myMSG msgSent;
    msgSent.key = receivedPacket.pid;
    msgSent.finalPacket = receivedPacket;
    if (msgsnd(Smsgid, (void *)&msgSent, sizeof(receivedPacket), 0) == -1) {
		fprintf(stderr, "msgsnd failed\n");
		exit(EXIT_FAILURE);
	}
}

//Check list of employee numbers from database given department name
void checkEmployeesInDepartment(packet receivedPacket) {
	bool correct = 0;
    //count of how many employee numbers
	int i = 0;
    //if list is empty
    if (head == NULL) {
    	receivedPacket.success = 0;
	}
    //if list is not empty
	else {
        //traverse through list to find all matches and store in array of employee numbers
		node_t nodeN = createNode();
		nodeN = head;
		while (nodeN != NULL) {
			if (strcmp(nodeN->data.departmentName, receivedPacket.departmentName) == 0) {
				receivedPacket.empNums[i] = nodeN->data.empNum;
				correct = 1;
				i++;
			}
			nodeN = nodeN->next; 
		}
        //store number of employee numbers
		if (correct) {
			receivedPacket.numOfEmps = i;
			receivedPacket.success = 1;
		}
		else {
			receivedPacket.success = 0;
		}
	}
    //send message to client 
    myMSG msgSent;
    msgSent.key = receivedPacket.pid;
    msgSent.finalPacket = receivedPacket;
    if (msgsnd(Smsgid, (void *)&msgSent, sizeof(receivedPacket), 0) == -1) {
		fprintf(stderr, "msgsnd failed\n");
		exit(EXIT_FAILURE);
	}
	
}

//Delete a node from the database (linked list)
void delete(packet receivedPacket) {
	node_t nodeX = createNode();
    receivedPacket.success = 0;
    //case if empty list
	if (head == NULL) {
    	receivedPacket.success = 0;
	}
	//case if deleting head
	else if (head->data.empNum == receivedPacket.empNum) {
		nodeX = head;
		head = head->next;
		free(nodeX);
		receivedPacket.success = 1;
	}
	//case if deleting within list
	else {
        //traverse through list to find a match
		node_t nodeY = createNode();
		nodeX = head;
		nodeY = nodeX->next;
		while (nodeY != NULL) {
			if (nodeY->data.empNum == receivedPacket.empNum) {
				nodeX->next = nodeY->next;
				free(nodeY);
				receivedPacket.success = 1;
				break;
			}
			else {
				nodeX = nodeY;
				nodeY = nodeY->next;
			}
		}
	}
    //send message to client if successful or unsuccessful deletion occured
    myMSG msgSent;
    msgSent.key = receivedPacket.pid;
    msgSent.finalPacket = receivedPacket;
    if (msgsnd(Smsgid, (void *)&msgSent, sizeof(receivedPacket), 0) == -1) {
		fprintf(stderr, "msgsnd failed\n");
		exit(EXIT_FAILURE);
	}
	
}
