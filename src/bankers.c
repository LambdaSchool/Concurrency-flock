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
	// vvvvvvvvvvvvvvvvvv
	// !!!! IMPLEMENT ME:
	int randNum = rand() % 999;
	return randNum;
	// Return a random number between 0 and 999 inclusive using rand()

	// ^^^^^^^^^^^^^^^^^^
}

int get_small_rand(void)
{
	int randNum = rand() % 3;
	return randNum;
}

/**
 * Main
 */
int main(int argc, char **argv)
{
	if (argc != 2) {
		fprintf(stderr, "usage: bankers numprocesses\n");
        exit(1);
	}
	int user_input = atoi(argv[1]);
	int num_processes = user_input;
	if (num_processes <= 0) {
		fprintf(stderr, "bankers: num processes must be greater than 0\n%s is invalid.\n", argv[1]);
		exit(1);
	}
	printf("Creating %d processes...\n", num_processes);
	
	int fd = open_balance_file(BALANCE_FILE);
	write_balance(fd, 10000);
	close_balance_file(fd);

	for (int i = 0; i < num_processes; i++) {
		if (fork() == 0) {
			srand(getpid());

			int amount = get_random_amount();

			int balance;
			fd = open_balance_file(BALANCE_FILE);
			while (flock(fd, LOCK_EX) == -1);
			int new_balance, case_num;
			read_balance(fd, &balance);
			case_num = get_small_rand(); // determine deposit, withdraw, or check balance
			switch(case_num) {
				case 0: // Deposit
					new_balance = balance + amount;
					write_balance(fd, new_balance);
			
					printf("Deposited $%d, new balance $%d.\n", amount, new_balance);

					flock(fd, LOCK_UN);
					close_balance_file(fd);	

					exit(0);
				break;
				case 1: // Withdraw
					new_balance = balance - amount;
					if (new_balance < 0) {
						printf("Only have $%d, can't withdraw $%d.\n", balance, amount);
						flock(fd, LOCK_UN);
						close_balance_file(fd);
						exit(0);
					}
			
					write_balance(fd, new_balance);
			
					printf("Withdrew $%d, new balance $%d.\n", amount, new_balance);

					flock(fd, LOCK_UN);
					close_balance_file(fd);	

					exit(0);
				break;
				case 2: // Check balance
					printf("Balance is $%d\n", balance);

					flock(fd, LOCK_UN);
					close_balance_file(fd);	

					exit(0);
				break;
			}
		}
	}

	// Parent process: wait for all forked processes to complete
	for (int i = 0; i < num_processes; i++) {
		wait(NULL);
	}

	return 0;
}
