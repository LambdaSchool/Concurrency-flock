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
int close_balance_file(int fd)
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
	char buffer[1024];
	int size = sprintf(buffer, "%d", balance);

	// We want to replace the balance in the file with a new balance. To
	// do that, we first truncate the file to 0 bytes size, then move
	// the current read/write position to the start of the file.
	ftruncate(fd, 0);
	lseek(fd, 0, SEEK_SET);

	// Now we write the new balance
	int bytes_written = write(fd, buffer, size);

	// Make sure nothing went wrong
	if (bytes_written < 0) {
		// What does perror do? man 3 perror
		// perror prints a message to stderr describing the last error that occured
		// in a call to a system or library function.
		perror("write");
	}
}

/**
 * Read the balance from a file
 */
void read_balance(int fd, int *balance)
{
	char buffer[1024];

	// Seek to the beginning of the file, just in case we're not there
	// already:
	lseek(fd, 0, SEEK_SET);

	// Read the balance into a buffer
	int bytes_read = read(fd, buffer, sizeof buffer);
	buffer[bytes_read] = '\0';

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
int get_random_amount(void)
{
	return rand() % 1000;
}

/**
 * Main
 */
int main(int argc, char **argv)
{
	// Parse the command line

	if(argc < 2) {
		fprintf(stderr, "usage: bankers numprocesses\n");
		exit(1);
	}

	// How many processes to fork at once
	int num_processes = atoi(argv[1]);

	if(num_processes <= 0) {
		fprintf(stderr, "bankers: num processes must be greater than 0\n");
		exit(2);
	}

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
			int processPicker = rand() % 3;

			int balance;

			fd = open_balance_file(BALANCE_FILE);

			if(processPicker == 0) {
				flock(fd, LOCK_EX);
				read_balance(fd, &balance);
				// sleep(1);
				if(balance >= amount) {
					balance = balance - amount;
					printf("PID: %d Withdrew $%d, new balance $%d\n", getpid(), amount, balance);
					write_balance(fd, balance);
				} else {
					printf("PID: %d Only have $%d, can't withdraw $%d\n", getpid(), balance, amount);
				}
				flock(fd, LOCK_UN);
			} else if(processPicker == 1) {
				flock(fd, LOCK_EX);
				read_balance(fd, &balance);
				// sleep(1);
				balance += amount;
				printf("PID: %d Deposited $%d, new balance $%d\n", getpid(), amount, balance);
				write_balance(fd, balance);
				flock(fd, LOCK_UN);
			} else {
				// WSL weirdness? All balance checks happen at the same time when using LOCK_SH
				// with the calls to sleep() in place. Using LOCK_EX or removing the calls to sleep()
				// stops that.
				flock(fd, LOCK_SH);
				read_balance(fd, &balance);
				// sleep(1);
				printf("PID: %d Your balance is $%d\n", getpid(), balance);
				flock(fd, LOCK_UN);
			}

			close_balance_file(fd);
			
			exit(0);
		}
	}

	// Parent process: wait for all forked processes to complete
	for (int i = 0; i < num_processes; i++) {
		wait(NULL);
	}

	return 0;
}
