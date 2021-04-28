# Enterprise Record Keeping System

In this assignment, a client-server model was utilized for inter-process communication. Message queues were used to implement the communication between the client process and server process. The client-server model is used to model a record keeper, which stores records of employees in a data structure. The server provides a client with 7 services: insert a new record, check the name of an employee, check the department of an employee, check an employee number, check the salary of an employee, get the employee numbers of all employees in a specified department, and delete an employee record.

Requirements:
- Version of Fedora Linux OS.


How to run:
1. Download the zipped folder and unzip the contents.
2. Run the following Makefiles by entering:
	- $ make -f Makefile_Administrator
	- $ make -f Makefile_RecordKeeper
3. Then finally to run the program use the following the command:
	- $ ./administrator
	- $ ./recordKeeper
4. Enjoy

# Authors:
- Tufayl Dhalla
- Sahil Sharma
