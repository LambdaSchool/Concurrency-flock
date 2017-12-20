1. **Short answer**: How can things go wrong if two processes attempt the
   above plan at the same time? Is there more than one way things can go
   wrong?

   * If 2 withdrawals occur at the same time (i.e. before the balance is updated(written) from first withdrawal) then it's possible that the true balance gets "lost in the shuffle". This is probably why the balance sometimes gets larger, even though deposits are not being made.

2. Study and understand the skeleton code in the `src/` directory.

   **Short answer**: what do each of the arguments to `open()` mean?

   * open(filename, O_CREAT|O_RDWR, 0644)

   * filename - name of file to open

   * O_CREAT - create the file if it does not exist

   * O_RDWR - sets permission for reading and writing

   * 0644 - (06) owner of file can read and write, (4) users of group can read, (4) everyone else can read

   * open returns a file descriptor

3. Take the skeleton code in the `src/` directory and implement the
   pieces marked. Run it.

   **Short answer**: What happens? Do things go as planned and look
   sensible? What do you speculate is happening?

   * The balance is not accurate (sometimes it goes up). Some processes are making withdrawals before other/previous processes are updating the balance.

4. Add calls to [`flock()`](https://linux.die.net/man/2/flock) to
   capture and release an exclusive lock on the file before reading and
   changing the data.

   The results of the run should now make sense.

5. **Short answer**: Why is it working? How has adding locks fixed the
   problems you noted in question 1? How is overall performance of the
   application affected?

   * The system allows the current process to complete, by "locking" the other processes. A subsequent process can't try to read the balance until it has been written by the previous process. This ensures that the balance is always correct (i.e. doesn't go higher at times).
