//Authors: Tufayl Dhalla and Sahil Sharma
//imports
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include "GUI.h"

//getOption
//Promp user for input
int getUserOption(){
		int finished = 0; 
		int Number =0;
		do{
			bool correct = 1;
			char input[50];
			printf("Please enter an integer to choose a command from the following\n"); 
			printf("Enter 1 to insert a record. \n"); 
			printf("Enter 2 to check name \n"); 
			printf("Enter 3 to check department \n"); 
			printf("Enter 4 to check salary \n"); 
			printf("Enter 5 to check employee number \n"); 
			printf("Enter 6 to check all employees in department \n"); 
			printf("Enter 7 to delete record \n");
			scanf(" %s", input);
            //Error checking
            //Making sure inputted response is a valid character and between [1,7]
			if(!isdigit(input[0])){
				printf("Invalid Input \n"); 
				correct=0; 
			}else{
				Number = atoi(input); 
			}	
			if(Number >= 1 && Number<=7 && correct){
					finished =1; 
			}else{
				printf("Not an option \n");
			}
		}while(finished == 0); 
		
		return Number; 
}

//prompt user to input name of employee
void getName(char *name){
	int finished = 0; 
		do{
			bool correct = 1;
			char input[50];
			printf("Enter Employee First Name: "); 
			scanf(" %s", input); 
			//Error checking
			for(int i=0; input[i] != '\0';i++){
				if(i>=BUF_SIZE){
					correct=0; 
					break; 
				}		
			}
			if(correct){ 
				strcpy(name,input); 
				finished=1;
			}else{
				printf("Invalid Input"); 
			}
		
		}while(finished == 0); 
}

//prompt user to input name of department
void getDepartmentName(char *name){
	int finished = 0; 
		do{
			bool correct = 1;
			char input[50];
			printf("Enter Department Name: "); 
			scanf(" %s", input); 
			//Error checking
			for(int i=0; input[i] != '\0';i++){
				if(i>=BUF_SIZE){
					correct=0; 
					break; 
				}		
			}
			if(correct){ 
				strcpy(name,input); 
				finished=1; 
			}else{
				printf("Invalid Input"); 
			}
		
		}while(finished == 0); 
}

//prompt user to input employee number
long long int getEmployeeNumber(){
		int finished = 1; 
		long long int num; 
		do{
			bool correct = 1;
			char input[25]; 
			printf("Enter an Employee number: "); 
			scanf("%s", input);
            //Error checking
			for(int q=0; input[q] != '\0'; q++){
				if(!isdigit(input[q])|| q>=BUF_SIZE){
					correct = 0;
					break;
				}
			}
			if (correct) {
				num=atoi(input); 
				finished=0; 
			}
		}while(finished); 
		return num; 
}

//prompt user to input salary of employee
long long int getSalary(){
		int finished = 1; 
		long long int num; 
		do{
			bool correct = 1;
			char input[25]; 
			printf("Enter employee's salary: "); 
			scanf("%s", input);
            //Error checking
			for(int q=0; input[q] != '\0'; q++){
				if(!isdigit(input[q])|| q>=BUF_SIZE){
					correct = 0;
					break;
				}
			}
			if (correct) {
				num=atoi(input); 
				finished=0; 
			}
		}while(finished); 
		return num; 
}
