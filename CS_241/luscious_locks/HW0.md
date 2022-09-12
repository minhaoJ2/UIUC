


# Welcome to Homework 0!

For these questions you'll need the mini course and virtual machine (Linux-In-TheBrowser) at -

http://cs-education.github.io/sys/

Let's take a look at some C code (with apologies to a well known song)-
```C
// An array to hold the following bytes. "q" will hold the address of where those bytes are.
// The [] mean set aside some space and copy these bytes into teh array array
char q[] = "Do you wanna build a C99 program?";

// This will be fun if our code has the word 'or' in later...
#define or "go debugging with gdb?"

// sizeof is not the same as strlen. You need to know how to use these correctly, including why you probably want strlen+1

static unsigned int i = sizeof(or) != strlen(or);

// Reading backwards, ptr is a pointer to a character. (It holds the address of the first byte of that string constant)
char* ptr = "lathe"; 

// Print something out
size_t come = fprintf(stdout,"%s door", ptr+2);

// Challenge: Why is the value of away zero?
int away = ! (int) * "";


// Some system programming - ask for some virtual memory

int* shared = mmap(NULL, sizeof(int*), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
munmap(shared,sizeof(int*));

// Now clone our process and run other programs
if(!fork()) { execlp("man","man","-3","ftell", (char*)0); perror("failed"); }
if(!fork()) { execlp("make","make", "snowman", (char*)0); execlp("make","make", (char*)0)); }

// Let's get out of it?
exit(0);
```

## So you want to master System Programming? And get a better grade than B?
```C
int main(int argc, char** argv) {
	puts("Great! We have plenty of useful resources for you, but it's up to you to");
	puts(" be an active learner and learn how to solve problems and debug code.");
	puts("Bring your near-completed answers the problems below");
	puts(" to the first lab to show that you've been working on this.");
	printf("A few \"don't knows\" or \"unsure\" is fine for lab 1.\n"); 
	puts("Warning: you and your peers will work hard in this class.");
	puts("This is not CS225; you will be pushed much harder to");
	puts(" work things out on your own.");
	fprintf(stdout,"This homework is a stepping stone to all future assignments.\n");
	char p[] = "So, you will want to clear up any confusions or misconceptions.\n";
	write(1, p, strlen(p) );
	char buffer[1024];
	sprintf(buffer,"For grading purposes, this homework 0 will be graded as part of your lab %d work.\n", 1);
	write(1, buffer, strlen(buffer));
	printf("Press Return to continue\n");
	read(0, buffer, sizeof(buffer));
	return 0;
}
```
## Watch the videos and write up your answers to the following questions

**Important!**

The virtual machine-in-your-browser and the videos you need for HW0 are here:

http://cs-education.github.io/sys/

The coursebook:

http://cs241.cs.illinois.edu/coursebook/index.html

Questions? Comments? Use Piazza:
https://piazza.com/illinois/fall2020/cs241

The in-browser virtual machine runs entirely in Javascript and is fastest in Chrome. Note the VM and any code you write is reset when you reload the page, *so copy your code to a separate document.* The post-video challenges (e.g. Haiku poem) are not part of homework 0 but you learn the most by doing (rather than just passively watching) - so we suggest you have some fun with each end-of-video challenge.

HW0 questions are below. Copy your answers into a text document because you'll need to submit them later in the course. More information will be in the first lab.

## Chapter 1

In which our intrepid hero battles standard out, standard error, file descriptors and writing to files.

### Hello, World! (system call style)
1. Write a program that uses `write()` to print out "Hi! My name is `<Your Name>`".
	```c
		#include <unistd.h>
		int main() {
			write(STDOUT_FILENO, "Hi! My name is Michael Jiang", 28);
		}
	```
### Hello, Standard Error Stream!
2. Write a function to print out a triangle of height `n` to standard error.
   - Your function should have the signature `void write_triangle(int n)` and should use `write()`.
   - The triangle should look like this, for n = 3:
   ```c
   *
   **
   ***
   ```
   ```c
   #include <unistd.h>
	void write_triangle(int n) {
		int i = 0;
		for(; i < n; i++) {
			write(STDERR_FILENO, "*", 1);
		}
		write(STDERR_FILENO, "\n", 1);
	}
	int main(int n) {
		int i = 0;
		for(; i < n; i++) {
			write_triangle(n);
		}
		return 0;
	}
   ```
### Writing to files
3. Take your program from "Hello, World!" modify it write to a file called `hello_world.txt`.
   - Make sure to to use correct flags and a correct mode for `open()` (`man 2 open` is your friend).
```c
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
int main() {
	mode_t mode = S_IRUSR | S_IWUSR;
	int fildes = open("hello_world.txt", O_CREAT | O_TRUNC | O_RDWR, mode);
	write(fildes, "Hello, World!", 13);
	close(fildes);
	return 0;
}
```
### Not everything is a system call
4. Take your program from "Writing to files" and replace `write()` with `printf()`.
   - Make sure to print to the file instead of standard out!
```c
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
int main() {
	mode_t mode = S_IRUSR | S_IWUSR;
	int fildes = open("hello_world.txt", O_CREAT | O_TRUNC | O_RDWR, mode);
	dprintf(3, "Hello World!");
	close(fildes);
	return 0;
}
```
5. What are some differences between `write()` and `printf()`?
```c
write() is a system call which happens inside the kernel instead of being part of the application. And kernel is a software layer that provides some basic functionalities that abstract hardware to you.
Apart from this, write() is designed to only write a sequence of bytes and printf() is a function that converts the data into a formatted sequence of bytes and that calls write() to write those bytes to the output.
```

## Chapter 2

Sizing up C types and their limits, `int` and `char` arrays, and incrementing pointers

### Not all bytes are 8 bits?
1. How many bits are there in a byte?
```c
8
```
2. How many bytes are there in a `char`?
```c
1
```
3. How many bytes the following are on your machine?
   - `int`, `double`, `float`, `long`, and `long long`
```c
int: 4, double: 8, float: 4, long: 8, long long: 8 
```
### Follow the int pointer
4. On a machine with 8 byte integers:
```C
int main(){
    int data[8];
}
```
If the address of data is `0x7fbd9d40`, then what is the address of `data+2`?
```c
0x7fbd9d50
```
5. What is `data[3]` equivalent to in C?
   - Hint: what does C convert `data[3]` to before dereferencing the address?
```c
data + 3
```

### `sizeof` character arrays, incrementing pointers
  
Remember, the type of a string constant `"abc"` is an array.

6. Why does this segfault?
```C
char *ptr = "hello";
*ptr = 'J';
```
```c
The string literals refer to an anonymous array object of type char[N] with static storage duration, where N is the length of the string plus one. Any attempt to modify it has undefined behavior.
```
7. What does `sizeof("Hello\0World")` return?
```c
6
```
8. What does `strlen("Hello\0World")` return?
```c
5
```
9. Give an example of X such that `sizeof(X)` is 3.
```c
X = "ab", sizeof("ab") = 3
```
10. Give an example of Y such that `sizeof(Y)` might be 4 or 8 depending on the machine.
```c
Y = int*, sizeof(Y) = 4 or 8 depending on the machine.
```

## Chapter 3

Program arguments, environment variables, and working with character arrays (strings)

### Program arguments, `argc`, `argv`
1. What are two ways to find the length of `argv`?
```c
	1. printf("%d\n", argc);
	2. 
	int len = 0;
	while (argv[len] != NULL) {
		len++;
	}
	printf("%d\n", len);
```
2. What does `argv[0]` represent?
```c
The name of the program if the name of the program exists.
```
### Environment Variables
3. Where are the pointers to environment variables stored (on the stack, the heap, somewhere else)?
```c
On the stack.
```
### String searching (strings are just char arrays)
4. On a machine where pointers are 8 bytes, and with the following code:
```C
char *ptr = "Hello";
char array[] = "Hello";
```
What are the values of `sizeof(ptr)` and `sizeof(array)`? Why?
```c
sizeof(ptr) = 8; Since ptr is a pointer and its size is always the same.
sizeof(array) = 6; equal to the length of the string plus one '\0'.
```
### Lifetime of automatic variables

5. What data structure manages the lifetime of automatic variables?
```c
Stack
```
## Chapter 4

Heap and stack memory, and working with structs

### Memory allocation using `malloc`, the heap, and time
1. If I want to use data after the lifetime of the function it was created in ends, where should I put it? How do I put it there?
```c
We should put it in the heap memory.
i) Declare the data as static to put it in the data segment of memory.
ii) Use malloc() to allocate it on the heap.
```
2. What are the differences between heap and stack memory?
```c
	1. In stack memory, the allocation and deallocation of memory is automatically done. And in heap memory, it needs to be done manually.
	2. Stack is not flexible. The memory size alotted on stack cannot be changed whereas Heap is flexible.
	3. The access of memory on heap is costlier than the access on stack.
```
3. Are there other kinds of memory in a process?
```c
Command line arguments and environment variables segment.
Uninitialized data segment.
Initialized data segment.
Text segment.
```
4. Fill in the blank: "In a good C program, for every malloc, there is a ___".
```c
free()
```
### Heap allocation gotchas
5. What is one reason `malloc` can fail?
```c
When there is no enough memory to be allocated.
```
6. What are some differences between `time()` and `ctime()`?
```c
time() is a system call function to get the current system time. And ctime() is a function to transfer the result of time() to human readable string line.
```
7. What is wrong with this code snippet?
```C
free(ptr);
free(ptr);
```
```c
The same ptr is freed twice. The second free would fail since ptr is pointing to an invalid address after being freed.
```
8. What is wrong with this code snippet?
```C
free(ptr);
printf("%s\n", ptr);
```
```c
The printf function used the pointer pointing to an invalid address.
```
9. How can one avoid the previous two mistakes? 
```c
After freeing a pointer, set this pointer to NULL.
``` 
### `struct`, `typedef`s, and a linked list
10. Create a `struct` that represents a `Person`. Then make a `typedef`, so that `struct Person` can be replaced with a single word. A person should contain the following information: their name (a string), their age (an integer), and a list of their friends (stored as a pointer to an array of pointers to `Person`s).
```c
struct Person {
	char* name;
	int age;
	struct Person** friends;
}
typedef struct Person person_t;
```
11. Now, make two persons on the heap, "Agent Smith" and "Sonny Moore", who are 128 and 256 years old respectively and are friends with each other.
```c
int main() {
	person_t* person1 = (person_t*) malloc(sizeof(person_t));
	person_t* person2 = (person_t*) malloc(sizeof(person_t));
	person1 -> name = "Agent Smith";
	person1 -> age = 128;
	person2 -> name = "Sonny Moore";
	person2 -> age = 256;
	person1 -> friends[0] = person2;
	person2 -> friends[0] = person1;
	free(person1);
	free(person2);
	return 0;
}
```
### Duplicating strings, memory allocation and deallocation of structures
Create functions to create and destroy a Person (Person's and their names should live on the heap).
12. `create()` should take a name and age. The name should be copied onto the heap. Use malloc to reserve sufficient memory for everyone having up to ten friends. Be sure initialize all fields (why?).
```c
person_t* create(char* setName, int setAge) {
	person_t* person = (person_t*) malloc(sizeof(person_t));
	person -> name = malloc(sizeof(*setName));
	person -> name = strdup(setName);
	person -> age = setAge;
	person -> friends = malloc(sizeof(person_t*) * 10);
	int i = 0;
	for (; i < 10; i++) {
		person -> friends[i] = NULL;
	}
	return person;
}
```
13. `destroy()` should free up not only the memory of the person struct, but also free all of its attributes that are stored on the heap. Destroying one person should not destroy any others.
```c
void destroy(person_t* person) {
	free(person -> name);
	person -> name = NULL;
	free(person -> friends);
	person -> friends = NULL;
	memset(person, 0, sizeof(person_t));
	free(person);
	person = NULL;
	return
}
```
## Chapter 5 

Text input and output and parsing using `getchar`, `gets`, and `getline`.

### Reading characters, trouble with gets
1. What functions can be used for getting characters from `stdin` and writing them to `stdout`?

>- char() is used to get from stdin and putchar() is used to write to stdout.

2. Name one issue with `gets()`.
>- Overflow of the input (size larger than declared) will change the variable without warning.

### Introducing `sscanf` and friends
3. Write code that parses the string "Hello 5 World" and initializes 3 variables to "Hello", 5, and "World".
```c
#include <stdio.h>
int main() {
	char* data = "Hello 5 World";
	char buffer1[10];
	char buffer2[10];
	int score = -1;
	int result = sscanf(data, "%s %d %s", buffer1, &score, buffer2);
	return EXIT_SUCCESS;
}
```
### `getline` is useful
4. What does one need to define before including `getline()`?
```c
#define _GNU_SOURCE
#include <stdio.h>
int main() {
	FILE* filestream = fopen("filename.txt", "r");
	char* buffer = NULL;
	size_t capacity = 0;
	ssize_t result;
}
```
5. Write a C program to print out the content of a file line-by-line using `getline()`.
```c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
int main() {
	FILE* filestream = fopen("filename.txt", "r");
	char* buffer = NULL;
	size_t capacity = 0;
	ssize_t  result;
	while ((result = getline(&buffer, &capacity, filestream)) != -1) {
		printf("%s\n", buffer);
	}
}
```
## C Development

These are general tips for compiling and developing using a compiler and git. Some web searches will be useful here

1. What compiler flag is used to generate a debug build?
>- gcc -g
2. You modify the Makefile to generate debug builds and type `make` again. Explain why this is insufficient to generate a new build.
>- We need to "make clean" first to clean all previous .o files to generate a new build.
3. Are tabs or spaces used to indent the commands after the rule in a Makefile?
>- Spaces.
4. What does `git commit` do? What's a `sha` in the context of git?
>- The "git commmit" command would save all staged changes, along with a brief description from the user, in a commit to the local repository. And every commit would create a unique ID ("SHA", i.e., simple hashing algorithm, or hash value) to keep record of the changes and the user who made the changes.
5. What does `git log` show you?
>- `git log` allow you to review and read a history of everything that happens to a repository.
6. What does `git status` tell you and how would the contents of `.gitignore` change its output?
>- The `git status` command displays the state of the working directory and the staging area. It lets you see which changes have been staged, which haven't, and which files aren't being tracked by Git.  The `.gitignore` file specifies intentionally untracked files that Git should ignore.
7. What does `git push` do? Why is it not just sufficient to commit with `git commit -m 'fixed all bugs' `?
>- `git push` uploads local repository content to a remote repository. And `git commit -m 'fixed all bugs' would simply save all staged changes to the local repository. And the remote repository would not be affected.`
8. What does a non-fast-forward error `git push` reject mean? What is the most common way of dealing with this?
>- Git cannot make changes on the remote without losing commits, so it would refuse the pushes. And this usually happens when some other user is pushing to the same branch. We can resolve this error by fetching and merging the remote branch, or using pull to perform both at once.
## Optional (Just for fun)
- Convert your a song lyrics into System Programming and C code and share on Piazza.
- Find, in your opinion, the best and worst C code on the web and post the link to Piazza.
- Write a short C program with a deliberate subtle C bug and post it on Piazza to see if others can spot your bug.
- Do you have any cool/disastrous system programming bugs you've heard about? Feel free to share with your peers and the course staff on piazza.
