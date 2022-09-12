
# Angrave's 2020 Acme CS 241 Exam Prep

## A.K.A. Preparing for the Final Exam & Beyond CS 241...

  

Some of the questions require research (wikibook; websearch; wikipedia).

It is accepted to work together and discuss answers, but please make an honest attempt first!

Be ready to discuss and clear confusions & misconceptions in your last discussion section.

The final will also include pthreads, fork-exec-wait questions and virtual memory address translation.

Be awesome. Angrave.

  

## 1. C

  
  

1. What are the differences between a library call and a system call? Include an example of each.

  
  

2. What is the `*` operator in C? What is the `&` operator? Give an example of each.

`*` operator is to dereference the parameters. int* c; *c = 1\

`&` operator is to access the address. int a = 1; int* t = &a;\

  

3. When is `strlen(s)` != `1+strlen(s+1)` ?
When s is NULL.

  

4. How are C strings represented in memory? What is the wrong with `malloc(strlen(s))` when copying strings?
C strings are represents as an array of characters in memory.
  
  

5. Implement a truncation function `void trunc(char*s,size_t max)` to ensure strings are not too long with the following edge cases.

```

if (length < max)

strcmp(trunc(s, max), s) == 0

else if (s is NULL)

trunc(s, max) == NULL

else

strlen(trunc(s, max)) <= max

// i.e. char s[]="abcdefgh; trunc(s,3); s == "abc".

```
void trunc(char*s,size_t max) {
  if (strlen(s) < max) return s;
  else if (s == NULL) return NULL;
  else s[max-1] = '\0'; return s;
}
  
  

6. Complete the following function to create a deep-copy on the heap of the argv array. Set the result pointer to point to your array. The only library calls you may use are malloc and memcpy. You may not use strdup.

  

`void duplicate(char **argv, char ***result);`

  

7. Write a program that reads a series of lines from `stdin` and prints them to `stdout` using `fgets` or `getline`. Your program should stop if a read error or end of file occurs. The last text line may not have a newline char.

  

## 2. Memory

  

1. Explain how a virtual address is converted into a physical address using a multi-level page table. You may use a concrete example e.g. a 64bit machine with 4KB pages.

  

2. Explain Knuth's and the Buddy allocation scheme. Discuss internal & external Fragmentation.
Knuth: As the blocks are contiguous, the end of one blocks sits right next to the start of the next block. So the current block (apart from the first one) can look a few bytes further back to lookup the size of the previous block. With this information you can now jump backwards
Buddy: A segregated allocator is one that divides the heap into different areas that are handled by different sub-allocators dependent on the size of the allocation request. Sizes are grouped into powers of two and each size is handled by a different sub-allocator and each size maintains its own free list.
internal & external fragmentation:Internal fragmentation is the wasted space within each allocated block because of rounding up from the actual requested allocation to the allocation granularity. External fragmentation is the various free spaced holes that are generated in either your memory or disk space.
  

3. What is the difference between the MMU and TLB? What is the purpose of each?
The Memory Management Unit (MMU) performs the address translation. If the translation succeeds, the page get pulled from RAM – conceptually the entire page isn’t loaded up. The result is cached in the TLB.
  

4. Assuming 4KB page tables what is the page number and offset for virtual address 0x12345678 ?

  

5. What is a page fault? When is it an error? When is it not an error?
A page fault is a type of exception raised by computer hardware when a running program accesses a memory page that is not currently mapped by the memory management unit (MMU) into the virtual address space of a process.

  

6. What is Spatial and Temporal Locality? Swapping? Swap file? Demand Paging?

  

## 3. Processes and Threads

  

1. What resources are shared between threads in the same process?
Resources like code, heap, data, and files can be shared among all threads within a process.
  

2. Explain the operating system actions required to perform a process context switch

  

3. Explain the actions required to perform a thread context switch to a thread in the same process

  

4. How can a process be orphaned? What does the process do about it?
Parent exit earlier than child process. Assign them to the init - the first process
  

5. How do you create a process zombie?
If parent don’t wait on your children, Zombies occur when a child terminates and then take up a spot in the kernel process table for your process.
  

6. Under what conditions will a multi-threaded process exit? (List at least 4)
Returning from the thread function
Calling pthread_exit
Cancelling the thread with pthread_cancel
Terminating the process through a signal
  

## 4. Scheduling

1. Define arrival time, pre-emption, turnaround time, waiting time and response time in the context of scheduling algorithms. What is starvation? Which scheduling policies have the possibility of resulting in starvation?

  

2. Which scheduling algorithm results the smallest average wait time?
Shortest-Job-First algoritm results the smallest average wait time.
  

3. What scheduling algorithm has the longest average response time? shortest total wait time?

  

4. Describe Round-Robin scheduling and its performance advantages and disadvantages.
Round Robin is a CPU scheduling algorithm where each process is assigned a fixed time slot in a cyclic way. The advantages are that every process gets equal share of CPU, the newly created process is added to end of read queue and each process gets a chance to reschedule after a particular quantum time in this scheduling. And the disadvatages ares that there is a lot of context switches, a larger waiting time and response time and low throughput.
  

5. Describe the First Come First Serve (FCFS) scheduling algorithm. Explain how it leads to the convoy effect.

  

6. Describe the Pre-emptive and Non-preemptive SJF scheduling algorithms.

  

7. How does the length of the time quantum affect Round-Robin scheduling? What is the problem if the quantum is too small? In the limit of large time slices Round Robin is identical to _____?

  

8. What reasons might cause a scheduler switch a process from the running to the ready state?

  

## 5. Synchronization and Deadlock

  

1. Define circular wait, mutual exclusion, hold and wait, and no-preemption. How are these related to deadlock?
Circular wait: each process must be waiting for a resource which is being held by another process.
Mutual exclusion: At least one process must be held in a non-shareable mode. Only one process can use the resource at the given instant of time.
Hold and wait: a process is currently holding at least one resource and requesting additional resources which are being held by other processes.
No-preemption: a resource can be released only voluntarily by the process holding it.
Satisfying these four conditions indicates the possibility of deadlock on systems.
  

2. What problem does the Banker's Algorithm solve?
anker's Algorithm is used majorly in the banking system to avoid deadlock. It helps you to identify whether a loan will be given or not. This algorithm is used to test for safely simulating the allocation for determining the maximum amount available for all resources. q
  

3. What is the difference between Deadlock Prevention, Deadlock Detection and Deadlock Avoidance?

  

4. Sketch how to use condition-variable based barrier to ensure your main game loop does not start until the audio and graphic threads have initialized the hardware and are ready.

  

5. Implement a producer-consumer fixed sized array using condition variables and mutex lock.

  

6. Create an incorrect solution to the CSP for 2 processes that breaks: i) Mutual exclusion. ii) Bounded wait.

  

7. Create a reader-writer implementation that suffers from a subtle problem. Explain your subtle bug.

  

## 6. IPC and signals

  

1. Write brief code to redirect future standard output to a file.
```c
int fd = open(filename, "w+");
dup2(fd, stdout);
```

2. Write a brief code example that uses dup2 and fork to redirect a child process output to a pipe
```c
int p[2];
pipe(p);
pid_t pid = fork();
if (pid == 0) {
    dup2(p[1], stdout);
    close(p[0])
} else {
    close(p[1]);
}
``` 

3. Give an example of kernel generated signal. List 2 calls that can a process can use to generate a SIGUSR1.

  

4. What signals can be caught or ignored?
SIGINT, SIGQUIT, SIGABRT, SUGPIPE, SIGTERM can be caught or ignored. SIGKILL and SIGSTOP cannot be caught and cannot be ignored.
  

5. What signals cannot be caught? What is signal disposition?

  

6. Write code that uses sigaction and a signal set to create a SIGALRM handler.

  

7. Why is it unsafe to call printf, and malloc inside a signal handler?

  

## 7. Networking

  

1. Explain the purpose of `socket`, `bind`, `listen`, and `accept` functions
```socket``` call is used to generate a brand new network socket for usage; ```bind``` call is used to bind the unnamed socket to a certain port so that the socket can receive/send information on the Internet; ```listen``` call is usef mostly on server's side, it specifies how many pending incoming connections a server desires to handle; ```accept``` call should be used after a listen, accept simply accepts a connection from socket and creates a new file descriptor specifically for that new connection, this newly created socket will be used to send network information. 
  

2. Write brief (single-threaded) code using `getaddrinfo` to create a UDP IPv4 server. Your server should print the contents of the packet or stream to standard out until an exclamation point "!" is read.

  

3. Write brief (single-threaded) code using `getaddrinfo` to create a TCP IPv4 server. Your server should print the contents of the packet or stream to standard out until an exclamation point "!" is read.

  

4. Explain the main differences between using `select` and `epoll`. What are edge- and level-triggered epoll modes?

  

5. Describe the services provided by TCP but not UDP.
TCP has a handshake feature that guarantee and maintain a relatively stable connection comparing to UDP. TCP can send more data in one go than UDP does, and if TCP package is lost during transmission, it will always be resent to the receiver, while UDP doesn't care about losing data. 
  

6. How does TCP connection establishment work? And how does it affect latency in HTTP1.0 vs HTTP1.1?

  

7. Wrap a version of read in a loop to read up to 16KB into a buffer from a pipe or socket. Handle restarts (`EINTR`), and socket-closed events (return 0).

  

8. How is Domain Name System (DNS) related to IP and UDP? When does host resolution not cause traffic?

  

9. What is NAT and where and why is it used?

  

## 8. Files

  

1. Write code that uses `fseek`, `ftell`, `read` and `write` to copy the second half of the contents of a file to a `pipe`.
```c
void write_to_pipe(char *filename, int pipe) {
    FILE *fp = fopen(filename, "r");
    struct stat info;
    stat(filename, & info);
    size_t half_size = info.st_size / 2;
    fseek(fp, half_size, SEEK_SET);
    char *box = calloc(1, half_size);
    fread(box, half_size, 1, fp);
    int fnum = fileno(fp);
    write(fnum,box, half_size);
    return;
}
```

2. Write code that uses `open`, `fstat`, `mmap` to print in reverse the contents of a file to `stderr`.

  

3. Write brief code to create a symbolic link and hard link to the file /etc/password

  

4. "Creating a symlink in my home directory to the file /secret.txt succeeds but creating a hard link fails" Why?

  

5. Briefly explain permission bits (including sticky and setuid bits) for files and directories.

  

6. Write brief code to create a function that returns true (1) only if a path is a directory.
```c
int is_directory(char *path) {
    struct stat info;
    if (stat(path, & info) != 0) {
        return -1;
    } else {
        retrun S_ISDIR(info.st_mode);
    }
}
```
  

7. Write brief code to recursive search user's home directory and sub-directories (use `getenv`) for a file named "xkcd-functional.png' If the file is found, print the full path to stdout.

  

8. The file 'installmeplz' can't be run (it's owned by root and is not executable). Explain how to use sudo, chown and chmod shell commands, to change the ownership to you and ensure that it is executable.

  

## 9. File system

Assume 10 direct blocks, a pointer to an indirect block, double-indirect, and triple indirect block, and block size 4KB.

  

1. A file uses 10 direct blocks, a completely full indirect block and one double-indirect block. The latter has just one entry to a half-full indirect block. How many disk blocks does the file use, including its content, and all indirect, double-indirect blocks, but not the inode itself? A sketch would be useful.

  

2. How many i-node reads are required to fetch the file access time at /var/log/dmesg ? Assume the inode of (/) is cached in memory. Would your answer change if the file was created as a symbolic link? Hard link?

  

3. What information is stored in an i-node? What file system information is not?
i-node is a representation of files in a filesystem. It contains the information about a specific file, including file name, file size, modified time, permissions etc. 
  

4. Using a version of stat, write code to determine a file's size and return -1 if the file does not exist, return -2 if the file is a directory or -3 if it is a symbolic link.

  

5. If an i-node based file uses 10 direct and n single-indirect blocks (1 <= n <= 1024), what is the smallest and largest that the file contents can be in bytes? You can leave your answer as an expression.

  

6. When would `fstat(open(path,O_RDONLY),&s)` return different information in s than `lstat(path,&s)`?
if the path of ```lstat``` is a symbolic link to a file, then ```fstat``` would return the information of the file pointed to by symbolic link, but lstat would return the information about the symbolic link. 
  

## 10. "I know the answer to one exam question because I helped write it"

  

Create a hard but fair 'spot the lie/mistake' multiple choice or short-answer question. Ideally, 50% can get it correct.
```c
01  int count=5;
02  pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
03  pthread_cond_t cv = PTHREAD_COND_INITIALIZER;
04
05  void barrier() {
06    pthread_mutex_lock(&m);
07    count--;
08    if(count >0) {
09        while(count>0) pthread_cond_wait(&cv,&m);
10    } else {
11         pthread_cond_signal(&cv);
12    }
13    pthread_mutex_unlock(&m);
14  }
```
Which one of the following best describes the code above?

(a) When the 5th  thread calls  barrier, 5 threads will continue executing code after the  barrier  call

(b) One thread can continue executing code after the barrier call  _before_  the 5th  thread calls  barrier

(c) The given code correctly implements a single-use barrier

(d) When the 5th  thread calls  barrier, 2 threads will continue executing code after the  barrier  call

(e) When the 5th  thread calls  barrier, no threads will continue executing code after the  barrier  call