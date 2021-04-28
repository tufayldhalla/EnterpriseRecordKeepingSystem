# Enterprise Record Keeping System

In this assignment a client-server model was utilized for inter process communication. Message queues were used to implement the communication between the client process and server process. The client server model is used to model a record keeper, which stores records of employees in a data structure. The server provides client with 7 services: Insert a new record, check name of employee, check department of employee, check employee number of employee, check salary of employee, get employee numbers of all employees in specified department and delete and employee record.

Requirements:
- Version of Fedora Linux OS.


How to run:
1. Download the zipped folder and unzip the contents.
2. Run the following Makefiles by entering:
	$ make -f Makefile_Administrator
	$ make -f Makefile_RecordKeeper
3. Then finally to run the program use the following the command:
	$ ./administrator
	$ ./recordKeeper
4. Enjoy

# Authors:
- Tufayl Dhalla
- Sahil Sharma
