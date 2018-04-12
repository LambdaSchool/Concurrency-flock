## Answers

1.  **Short answer**: How can things go wrong if two processes attempt the
    above plan at the same time? Is there more than one way things can go
    wrong?

    Two processes writing to one file may result in undefined behavior.
    Basically anything can happen..

1.  **Short answer**: what do each of the arguments to `open()` mean?

    `open(FILE_NAME_PATH, OPTIONS_FOR_READ_OR_WRITE_ETC, ... , LAST_OPTION)`

1.  **Short answer**: What happens? Do things go as planned and look
    sensible? What do you speculate is happening?

    Sometimes the balance is deducted as expected (by chance),
    but most of the time it just does unexpected stuff.
    Sometimes it looks like each `fork()` just writes to the file as if
    it were the only process and prints out the expected behavior.

1.  **Short answer**: Why is it working? How has adding locks fixed the
    problems you noted in question 1? How is overall performance of the
    application affected?

    It is working after implementing flock() because calling flock()
    will wait until flock != -1 (unless there is an explicit if for it,
    or set flock with unblocking process),
    or until the file is "unlocked" by `flock(fd, LOCK_UN)`, in which case
    `flock()` will equal zero. Because of this, only one process has access
    to `balance.txt` at any given moment in time.

    The overall performance will be decreased because each `fork`ed process
    must wait until `balance.txt` is unlocked
    (which makes the `write` calls _serial_). Basically it is a little faster
    than using a forloop and a single process (each process is "ready" to go).
    But the overhead of having many waiting processes may affect system perf
    and/or memory alloc.
