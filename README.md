# Bankers and Concurrency

This is an assignment to experiment with concurrency and some of the
issues that arise with it.

## Why Do I Care?

In all kinds of development, web included, multiple processes often have
access to the same resources. Obscure and hard-to-find (read
"expensive") bugs can occur when multiple processes attempt to access
the same resources at the same time.

It is always a good idea to think to yourself, what happens when two of
these processes run at once? Anything bad?

## Concurrency

What is
[concurrency](https://en.wikipedia.org/wiki/Concurrency_(computer_science))?
In a nutshell, we want to run a number of processes at the same time
with access to _shared resources_. And we want these processes not to
step on each-other's toes.

Imagine that after the Mount Everest expedition, there was an expedition
journal that needed to be filled out with the experiences of all
expedition members. Each member is given a pen, the journal is opened,
and they all begin writing on the same page at the same time.

Everyone scribbles on top of everyone else. It's not working.

So the expedition gives it a think, and decides to only allow one person
to write in the journal at a time. Problem solved. Publishers are
definitely interested. Someone smells movie contract.

Of course, all the other members of the expedition are sitting around
doing nothing while they wait their turn. Maybe there is room for
improvement... This is the kind of problem computer scientists like to
solve.

## Money, Money, Money

We're going to write code that simulates a number of processes trying to
withdraw money from a bank account at the same time.

The process for withdrawing money is simple:

1. Open the file containing the balance.
2. Read the current balance.
3. Try to withdraw some amount of cash.
   * If the amount to withdraw is less than (or equal to) the current
     balance:
       * Subtract that amount from the current balance.
	   * Write the new balance to the file.
   * If the amount to withdraw is greater than the current balance:
       * Print an error and do not modify the balance.
4. Close the file containing the balance.

This works fine for any one processes trying to withdraw money in isolation.


## Assignment

Implement code to safely withdraw money from a bank account. (A
simulated bank account, that is. Don't get your hopes up.)

1. **Short answer**: How can things go wrong if two processes attempt the
   above plan at the same time? Is there more than one way things can go
   wrong?
	 1. There are a couple of ways that things can go wrong. For example,
	 two process can both read the remaining balance as $500 and then both withdraw
	 $400. This will result in a remaining balance of -$300. If only one process could
	 access the bank account at a time, only one process would end up withdrawing the money,
	 leaving the balance at $100. Another thing that could go wrong is that before
	 one process is finished withdrawing money, another process could read the remaining
	 balance. The second process would have the incorrect balance after the first process
	 is finished. This could lead the second process to withdrawing more money than is 
	 in the account because it has the incorrect balance.

2. Study and understand the skeleton code in the `src/` directory.

   **Short answer**: what do each of the arguments to `open()` mean?
	2. The first argument is the filename to open. O_CREAT means
  it will create the file if it does not already exist. O_RDWR means 
	it will open the file for both reading and writing. The third argument, 
	0644, are the permissions the file is created with. 6 means that the user
	who owns the file has read and write permissions. The first 4 means the 
	group that owns the file has read permissions. The second 4 means that 
	other users have read permissions.

3. Take the skeleton code in the `src/` directory and implement the
   pieces marked. Run it.
   
   **Short answer**: What happens? Do things go as planned and look
   sensible? What do you speculate is happening?
	 3. There is some weird behavior. For example, one process prints out
	 that the balance is $4337. A later process prints out that the balance
	 is $8154. However, since there is only withdraw functionality, the balance
	 should only go down. I expect that this is happening because processes are 
	 reading the balance before other processes finish withdrawing. The value 
	 that the processes read are out of date when the other processes are done
	 withdrawing, but they have no way of knowing. 

4. Add calls to [`flock()`](https://linux.die.net/man/2/flock) to
   capture and release an exclusive lock on the file before reading and
   changing the data.

   The results of the run should now make sense.
   
5. **Short answer**: Why is it working? How has adding locks fixed the
   problems you noted in question 1? How is overall performance of the
   application affected?
	 5. Adding locks fixed the problems because only one process can manipulate
	 the file at a time. The other process will wait until the process that is 
	 using the file is finished. Adding locks will slow down the application because
	 processes will have to spend time waiting.  

## Stretch Goals

* Make some processes deposit money while others withdraw money.

* Have some processes simply check the balance without changing it. Do
  you need to lock? If so, do you need to use `LOCK_EX`? If not, what
  are the advantages of your approach?