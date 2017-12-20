#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <fcntl.h>
#include <time.h>

// This is the file where we store the balance
#define BALANCE_FILE "balance.txt" 
// macro defines a constant

/**
 * Open the file containing the balance
 */
int open_balance_file(char *filename)
{
	// This line returns an open "file descriptor" (a number, how Unix
	// tracks open files) for both reading and writing. If the file does
	// not exist, it is created with 0644 permissions.
	return open(filename, O_CREAT|O_RDWR, 0644);
}

/**
 * Close the file containing the balance
 */
int close_balance_file(int fd) //(Find out why file descriptor is fd)
{
	// Close the file descriptor
	return close(fd);
}

/**
 * Write the balance to the file
 */
void write_balance(int fd, int balance)
{
	// Print the balance into a buffer
	// temp data storage (Find out why needed and how to know when to use)
	char buffer[1024];
	int size = sprintf(buffer, "%d", balance);

	// We want to replace the balance in the file with a new balance. To
	// do that, we first truncate the file to 0 bytes size, then move
	// the current read/write position to the start of the file.
	ftruncate(fd, 0);
	lseek(fd, 0, SEEK_SET); 
	// system call to change location of read/write pointer of a 
	// file descriptor

	// Now we write the new balance
	int bytes_written = write(fd, buffer, size);

	// Make sure nothing went wrong
	if (bytes_written < 0) {
		// What does perror do? man 3 perror
		//outputs last system call error to stderr 
		// "write" is where the error occurs
		perror("write");
	}
}

/**
 * Read the balance from a file
 */
// (Find out why use a pointer - to be able to modify/allow other functions to modify?)
void read_balance(int fd, int *balance)
{
	char buffer[1024]; // (possible to make a macro #define?)

	// Seek to the beginning of the file, just in case we're not there
	// already:
	lseek(fd, 0, SEEK_SET);

	// Read the balance into a buffer
	int bytes_read = read(fd, buffer, sizeof buffer);
	buffer[bytes_read] = '\0';
	// '\0' NUL - reserved character used to signify the end of a string

	// Error check
	if (bytes_read < 0) {
		perror("read");
		return;
	}

	// Convert buffer to integer and store in balance
	*balance = atoi(buffer);
}

/**
 * Returns a random amount between 0 and 999.
 */
int get_random_amount(void) // (Look at use of void here)
{
	// vvvvvvvvvvvvvvvvvv
	// !!!! IMPLEMENT ME:
	//int random_num = rand() % 1000;
     return rand() % 1000;
	// Return a random number between 0 and 999 inclusive using rand()
	//return random_num;
	// ^^^^^^^^^^^^^^^^^^
}

/**
 * Main
 */
int main(int argc, char **argv)
{
	// Parse the command line
	
	// vvvvvvvvvvvvvvvvvv
	// !!!! IMPLEMENT ME:

	// We expect the user to add the number of simulataneous processes
	// after the command name on the command line.
	//
	// For example, to fork 12 processes:
	//
	//  ./bankers 12

	// Check to make sure they've added one paramter to the command line
	// with argc. If they didn't specify anything, print an error
	// message to stderr, and exit with status 1:
	// if (argc == NULL) {
	   if (argc != 2) {  //"Segmentation fault" 
		fprintf(stderr, "Did not specify number of processes\n");
		exit(1);
	}
	// "usage: bankers numprocesses\n"
	
	// Store the number of processes in this variable:

	// How many processes to fork at once******
	int num_processes = atoi(argv[1]); // 

	// Make sure the number of processes the user specified is more than
	// 0 and print an error to stderr if not, then exit with status 2:
	//if (num_processes < 0) {
	if (num_processes < 1) {	
		fprintf(stderr, "Number of processes must be greater than zero\n");
		exit(2);
	}
	// "bankers: num processes must be greater than 0\n"

	// ^^^^^^^^^^^^^^^^^^

	// Start with $10K in the bank. Easy peasy.
	int fd = open_balance_file(BALANCE_FILE);
	write_balance(fd, 10000);
	close_balance_file(fd);

	// Rabbits, rabbits, rabbits!
	for (int i = 0; i < num_processes; i++) {
		if (fork() == 0) {
			// "Seed" the random number generator with the current
			// process ID. This makes sure all processes get different
			// random numbers:
			srand(getpid());

			// Get a random amount of cash to withdraw. YOLO.
			int amount = get_random_amount();

			int balance; //(Is this where *balance points to?)

			// vvvvvvvvvvvvvvvvvvvvvvvv
			// !!!! IMPLEMENT ME
			// Open the balance file (feel free to call the helper
			// functions, above).
			//int fd = open_balance_file("balance.txt");
			int fd = open_balance_file(BALANCE_FILE);
			// macro defined above
			// Read the current balance
			read_balance(fd, &balance); //???Pointer
			sleep(1);
			// Try to withdraw money
			if (balance >= amount) {
				balance = balance - amount;
				printf("PID is %d | Withdrew $%d | new balance $%d\n", (int) getpid(), amount, balance);
				write_balance(fd, balance);
			} else {
				printf("Only have $%d, can't withdraw $%d\n", balance, amount);
			}
			// Sample messages to print:
			// "Withdrew $%d, new balance $%d\n"
			// "Only have $%d, can't withdraw $%d\n"

			// Close the balance file
			//^^^^^^^^^^^^^^^^^^^^^^^^^
			close_balance_file(fd);
			// Child process exits
			exit(0);
		}
	}

	// Parent process: wait for all forked processes to complete
	for (int i = 0; i < num_processes; i++) {
		wait(NULL);
	}

	return 0;
}
