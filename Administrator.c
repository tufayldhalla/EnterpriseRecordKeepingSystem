#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>
#include <sys/msg.h>
#include "GUI.h"

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

//function declarations
void insert(char name[], char departmentName[], long long int employeeNum, long long int salary);
void checkName(long long int employeeNum);
void checkDepartment(long long int employeeNum);
void checkSalary(long long int employeeNum);
void checkEmployeeNumber(char name[]);
void checkEmployeesInDepartment(char departmentName[]);
void delete(long long int employeeNum);

static pid_t clientPID;
static int Rmsgid; //receive (client) queue id
static int Smsgid; //send (server) queue id

int main()
{
	int running = 1;
	
    //variables
	int input;
	char name[13];
	char departmentName[13];
	long long int employeeNum;
	long long int salary;
	
	clientPID = getpid();

	//Client Message Queue Initialization
	Rmsgid = msgget((key_t)1234, 0666 | IPC_CREAT);
	if (Rmsgid == -1) {
		fprintf(stderr, "msgget failed with error: %d\n", errno);
		exit(EXIT_FAILURE);
	}

	//Server Message Queue Initialization
	Smsgid = msgget((key_t)1235, 0666 | IPC_CREAT);
	if (Smsgid == -1) {
		fprintf(stderr, "msgget failed with error: %d\n", errno);
		exit(EXIT_FAILURE);
	}

	while(running) {
        //get option from user (1 - insert, etc.)
		input = getUserOption();
		if (input == 1) { // INSERT RECORD
			printf("User seleted insert record\n");
            //get particular variables from user to enter into database
		    getName(name);
		    getDepartmentName(departmentName);
		    employeeNum = getEmployeeNumber();
		    salary = getSalary();
            //call function to send data to server to add to database
		    insert(name, departmentName, employeeNum, salary);
		} else if (input == 2) { // CHECK NAME
			printf("User seleted check name based on employee number\n");
            //get particular variable from user to search  database
		    employeeNum = getEmployeeNumber();
            //call function to send data to server to search database
		    checkName(employeeNum);
		} else if (input == 3) { //CHECK DEPARTMENT
			printf("User seleted check department based on employee number\n");
            //get particular variable from user to search  database
		    employeeNum = getEmployeeNumber();
            //call function to send data to server to search database
		    checkDepartment(employeeNum);
		} else if (input == 4) { //CHECK SALARY
			printf("User seleted check salary based on employee number\n");
            //get particular variable from user to search  database
		    employeeNum = getEmployeeNumber();
            //call function to send data to server to search database
		    checkSalary(employeeNum);
		} else if (input == 5) { //CHECK EMPLOYEE NUMBER
			printf("User seleted check employee number based on employee name\n");
            //get particular variable from user to search  database
		    getName(name);
            //call function to send data to server to search database
		    checkEmployeeNumber(name);
		} else if (input == 6) { //CHECK EMPLOYEES IN DEPARTMENT
			printf("User seleted view all employees in department\n");
            //get particular variable from user to search  database
		    getDepartmentName(departmentName);
            //call function to send data to server to search database
		    checkEmployeesInDepartment(departmentName);
		} else if (input == 7) { //DELETE RECORD
			printf("User seleted delete record\n");
            //get particular variable from user to delete from database
		    employeeNum = getEmployeeNumber();
            //call function to send data to server to delete record from database
		    delete(employeeNum);
		}
	}
	
     if (msgctl(Rmsgid, IPC_RMID, 0) == -1) {
     fprintf(stderr, "msgctl(IPC_RMID) failed\n");
     exit(EXIT_FAILURE);
     }
     if (msgctl(Smsgid, IPC_RMID, 0) == -1) {
     fprintf(stderr, "msgctl(IPC_RMID) failed\n");
     exit(EXIT_FAILURE);
     }
	
	
	exit(EXIT_SUCCESS);
}

//Function to take user input and forward to server to add to database and output if insertion was a success or failure
void insert(char name[], char departmentName[], long long int employeeNum, long long int salary) {
    //create data packet with all data
    packet sendPacket;
    sendPacket.pid = clientPID;
    sendPacket.userChoice = 1;
    strcpy(sendPacket.name, name);
    strcpy(sendPacket.departmentName, departmentName);
    sendPacket.empNum = employeeNum;
    sendPacket.salary = salary;
    //create send message
    myMSG msgSent;
    msgSent.key = sendPacket.pid;
    msgSent.finalPacket = sendPacket;
    //send message to server and output error message if send failed
    if (msgsnd(Smsgid, (void *)&msgSent, sizeof(sendPacket), 0) == -1) {
		fprintf(stderr, "msgsnd failed\n");
		exit(EXIT_FAILURE);
	}
    //create receive message
	packet returnPacket;
	myMSG msgReceived;
    //receive message from server and output error message if receive failed
	if (msgrcv(Rmsgid, (void *)&msgReceived, sizeof(returnPacket), getpid(), 0) == -1) {
		fprintf(stderr, "msgrcv failed with error: %d\n", errno);
		exit(EXIT_FAILURE);
	}
	returnPacket = msgReceived.finalPacket;
    //If insertion was a success, or else failure 
	if (returnPacket.success) {
		printf("Record inserted successfully\n");
	}
	else {
		printf("Could not add record\n");
	}
}

//Function to check database based on employee number and return employee name
void checkName(long long int employeeNum) {
    packet sendPacket;
    sendPacket.pid = clientPID;
    sendPacket.userChoice = 2;
    sendPacket.empNum = employeeNum;
    myMSG msgSent;
    msgSent.key = sendPacket.pid;
    msgSent.finalPacket = sendPacket;
    if (msgsnd(Smsgid, (void *)&msgSent, sizeof(sendPacket), 0) == -1) {
		fprintf(stderr, "msgsnd failed\n");
		exit(EXIT_FAILURE);
	}
	packet returnPacket;
	myMSG msgReceived;
	if (msgrcv(Rmsgid, (void *)&msgReceived, sizeof(returnPacket), getpid(), 0) == -1) {
		fprintf(stderr, "msgrcv failed with error: %d\n", errno);
		exit(EXIT_FAILURE);
	}
	returnPacket = msgReceived.finalPacket;
	if (returnPacket.success) {
		printf("Employee name with employee number %lli is %s\n", returnPacket.empNum, returnPacket.name);
	}
	else {
		printf("Could not find name\n");
	}
}

//Function to check database based on employee number and return department name
void checkDepartment(long long int employeeNum) {
    packet sendPacket;
    sendPacket.pid = clientPID;
    sendPacket.userChoice = 3;
    sendPacket.empNum = employeeNum;
    myMSG msgSent;
    msgSent.key = sendPacket.pid;
    msgSent.finalPacket = sendPacket;
    if (msgsnd(Smsgid, (void *)&msgSent, sizeof(sendPacket), 0) == -1) {
		fprintf(stderr, "msgsnd failed\n");
		exit(EXIT_FAILURE);
	}
	packet returnPacket;
	myMSG msgReceived;
	if (msgrcv(Rmsgid, (void *)&msgReceived, sizeof(returnPacket), getpid(), 0) == -1) {
		fprintf(stderr, "msgrcv failed with error: %d\n", errno);
		exit(EXIT_FAILURE);
	}
	returnPacket = msgReceived.finalPacket;
	if (returnPacket.success) {
		printf("Department name with employee number %lli is %s\n", returnPacket.empNum, returnPacket.departmentName);
	}
	else {
		printf("Could not find department name\n");
	}
}

//Function to check database based on employee number and return salary
void checkSalary(long long int employeeNum) {
    packet sendPacket;
    sendPacket.pid = clientPID;
    sendPacket.userChoice = 4;
    sendPacket.empNum = employeeNum;
    myMSG msgSent;
    msgSent.key = sendPacket.pid;
    msgSent.finalPacket = sendPacket;
    if (msgsnd(Smsgid, (void *)&msgSent, sizeof(sendPacket), 0) == -1) {
		fprintf(stderr, "msgsnd failed\n");
		exit(EXIT_FAILURE);
	}
	packet returnPacket;
	myMSG msgReceived;
	if (msgrcv(Rmsgid, (void *)&msgReceived, sizeof(returnPacket), getpid(), 0) == -1) {
		fprintf(stderr, "msgrcv failed with error: %d\n", errno);
		exit(EXIT_FAILURE);
	}
	returnPacket = msgReceived.finalPacket;
	if (returnPacket.success) {
		printf("Salary of employee with employee number %lli is %lli\n", returnPacket.empNum, returnPacket.salary);
	}
	else {
		printf("Could not find salary\n");
	}
}

//Function to check database based on employee name and return employee number
void checkEmployeeNumber(char name[]) {
    packet sendPacket;
    sendPacket.pid = clientPID;
    sendPacket.userChoice = 5;
    strcpy(sendPacket.name, name);
    myMSG msgSent;
    msgSent.key = sendPacket.pid;
    msgSent.finalPacket = sendPacket;
    if (msgsnd(Smsgid, (void *)&msgSent, sizeof(sendPacket), 0) == -1) {
		fprintf(stderr, "msgsnd failed\n");
		exit(EXIT_FAILURE);
	}
	packet returnPacket;
	myMSG msgReceived;
	if (msgrcv(Rmsgid, (void *)&msgReceived, sizeof(returnPacket), getpid(), 0) == -1) {
		fprintf(stderr, "msgrcv failed with error: %d\n", errno);
		exit(EXIT_FAILURE);
	}
	returnPacket = msgReceived.finalPacket;
	if (returnPacket.success) {
		printf("Employee %s has employee number %lli\n", returnPacket.name, returnPacket.empNum);
	}
	else {
		printf("Could not find employee number\n");
	}
}

//Function to check database based on department name and return list of employee numbers
void checkEmployeesInDepartment(char departmentName[]) {
    packet sendPacket;
    sendPacket.pid = clientPID;
    sendPacket.userChoice = 6;
    strcpy(sendPacket.departmentName, departmentName);
    myMSG msgSent;
    msgSent.key = sendPacket.pid;
    msgSent.finalPacket = sendPacket;
    if (msgsnd(Smsgid, (void *)&msgSent, sizeof(sendPacket), 0) == -1) {
		fprintf(stderr, "msgsnd failed\n");
		exit(EXIT_FAILURE);
	}
	packet returnPacket;
	myMSG msgReceived;
	if (msgrcv(Rmsgid, (void *)&msgReceived, sizeof(returnPacket), getpid(), 0) == -1) {
		fprintf(stderr, "msgrcv failed with error: %d\n", errno);
		exit(EXIT_FAILURE);
	}
	returnPacket = msgReceived.finalPacket;
	if (returnPacket.success) {
		for (int i = 0; i < returnPacket.numOfEmps; i++) {
			printf("%d. Employee number %lli\n", (i+1), returnPacket.empNums[i]);
		}
	}
	else {
		printf("Could not list employees in department record\n");
	}
}

//Function to check database based on employee number and delete record
void delete(long long int employeeNum) {
    packet sendPacket;
    sendPacket.pid = clientPID;
    sendPacket.userChoice = 7;
    sendPacket.empNum = employeeNum;
    myMSG msgSent;
    msgSent.key = sendPacket.pid;
    msgSent.finalPacket = sendPacket;
    if (msgsnd(Smsgid, (void *)&msgSent, sizeof(sendPacket), 0) == -1) {
		fprintf(stderr, "msgsnd failed\n");
		exit(EXIT_FAILURE);
	}
	packet returnPacket;
	myMSG msgReceived;
	if (msgrcv(Rmsgid, (void *)&msgReceived, sizeof(returnPacket), getpid(), 0) == -1) {
		fprintf(stderr, "msgrcv failed with error: %d\n", errno);
		exit(EXIT_FAILURE);
	}
	returnPacket = msgReceived.finalPacket;
	if (returnPacket.success) {
		printf("Record deleted successfully\n");
	}
	else {
		printf("Could not delete record, does not exist\n");
	}
}
