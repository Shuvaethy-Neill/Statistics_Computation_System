# Statistics_Computation_System
**Inter-process communication with Linux message queues modelling the client-server paradigm**

--------------------------------------------
DESCRIPTION
--------------------------------------------
This statistics computation system focuses on inter-process communication using **Linux message queues** while modelling the **client-server paradigm**. The system consists of two processes that will run on the same CPU: **User** (client process) and **Calculator** (server process). 

Calculator is responsible for storing and performing operations on numbers. User is responsible for sending the following commands to Calculator to perform on those numbers:

1) **Insert**: A number is sent as an argument for this command. In response, the Calculator appends the number to the existing set of numbers stored on the system.

2) **Delete**: A number is sent as an argument for this command. In response, the Calculator deletes all instances of the number in the existing set of numbers stored on the system.

3) **Sum**: In response, Calculator returns the sum of all the existing numbers in the stored set of numbers.

4) **Average**: In response, Calculator returns the average of all the existing numbers in the stored set of numbers.

5) **Min**: In response, Calculator returns the smallest existing number in the stored set of numbers.

6) **Median**: In response, Calculator returns the median value(s) for the stored set of numbers. When the number of values in the set is even, Calculator returns two values. 

7) **Exit**: Queues will be deleted and programs will end.

Calculator stores the set of numbers in an array. The size of the array is set to 100 but can be changed by modifying the MAX_LENGTH constant in the "shared_mem.h" header file.

**The following four files are included**:  
- Calculator.c
- User.c
- shared_mem.h
- Makefile

INSTALLATION & SETUP
--------------------------------------------
You must have access to a Linux operating system in order to execute this program. 

If you do not have Linux installed on your machine, you can also create and use a virtual machine:
1. Download Oracle VirtualBox and create virtual machine
    - Download link: https://www.virtualbox.org/wiki/Download_Old_Builds_6_0
2. Download Fedora 30 ISO in the newly created virtual machine to be able to run Linux
    - Download link: https://archives.fedoraproject.org/pub/archive/fedora/linux/releases/30/Workstation/x86_64/iso/


USAGE
--------------------------------------------
Navigate to the directory in which these files are stored. You can do this using the cd command:

    cd directory_name
    
Ensure that you have the Calculator.c and User.c source files, supporting shared_mem.h header file, and Makefile stored in the same directory. You can verify this by using the ls command and ensuring that the output displays 'Calculator.c',  'User.c', 'shared_mem.h' and 'Makefile'. 

Before executing the files, you will need to compile them. Since the compile command is in the provided Makefile, you will just need to type the following in your terminal:

    make
    
This will create two executable files 'calculator' and 'user' that you can run (as per the command written in the Makefile).

To run the program after compilation, open two terminals.
In the first terminal enter the following:

    ./calculator

In the second terminal enter the following:

    ./user

Once run, Calculator will output that it is waiting for a command. The Calculator terminal will be updated accordingly when the user interacts with the User terminal.

The User terminal will output the list of commands and ask the user to enter a command. Based on the entered command (insert or delete), the user will be prompted for an additional number argument.

Example of User terminal output for insert command:
```
COMMANDS:
insert
delete
sum
average
min
median
exit

Please enter the command you would like to use: insert
Please enter a number: 345
Message received from calculator: Number added.
```
As seen in the sample input, the calc process has sent a response to notify the User terminal via its message queue that the entered command was successfully completed. 

**Note**: *If the user enters an invalid command (including a command longer than 35 characters), they will be asked to enter a valid command.*

Example of corresponding Calculator terminal output for insert command:
```
Waiting to receive...

The command you wrote: insert
The number you wrote: 345
The average time to perform a command on the server is 1.0000 micro sec
The number set: 345 
```
As seen above, the Calculator terminal has outputted what the User has sent in their message, the average time it took to complete the command, and the current numbers in the stored number set. The list of numbers and the average time will be continually updated as more inputs are sent via the message of User.

**Note**: *The outputs of the two terminals above is an example and will vary depending on what the user inputs.*

