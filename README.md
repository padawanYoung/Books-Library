# BooksLibrary

This is an educational project that includes the following programming topics:
* Basiks of C language:
  * Type of data;
  * Arrays;
  * Conditions, loops;
  * functions;
  * Working with files (stdin, writing, reading);
  * Enumerators;
  * Structures;
  * Pointers, pointers to functions;
  * Dynamic allocation memory;
  * Memory free;
* Algorithms:
  * [Finite-state machine](https://en.wikipedia.org/wiki/Finite-state_machine);
  * [Linked list](https://en.wikipedia.org/wiki/Linked_list);
  * Storing data in [Hash-table](https://en.wikipedia.org/wiki/Hash_table);
  
# Summary
Using this console program you able to create your own books library, and save it into file. You just have to run thie program, and select “Create new file” option in main menu. Then, you have to add couple of information about your favorite book: Book’s author, Book’s title, publisthing date (dd/mm/yyy), pages quantiy.  You can add book one by one, find book by title, and even delete it. Each book you added listed in hash table. This table, stores in RAM memory, untill you add books to library. When you finish adding books, library writes into file. When you want to look at your library, you can run the programm and choose “Open exist book's library”. The program will show all books you added. When you run this program, it makes memory leask test before executing the main loop. The test is to create 100 books, write them into table and delete it in random order.This test repeats 1000 times. Then you will able to check program on possible memory leaks.

# Library's architecture

## Book's abstraction

A book, as object, has properties that distinguishes it from others (Author's name, Title, publishing date, pages' quantity). In this project book appears as structure: 

```cpp
typedef unsigned int HashType;

typedef struct {
    unsigned int year;
    unsigned int day;
    unsigned int month;
} publishingDate;

typedef struct booksPattern {
    HashType hash;
    char *author;
    char *title;
    publishingDate *date;
    unsigned int pagesQuantity;
    struct booksPattern *next;
    struct booksPattern *prev;
} Book;
```
## Hash table

All books are stored in the HASH table for quick search and removal.

In the program, the HASH table looks like this:

```cpp
Book *TABLE[TABLE_SIZE];
```
The hash is calculated from the "title" field as follows:

```cpp
HashType Hash(char *str) {
    HashType h = 0;
    int argument = 0;
    float polynomialKoefficient = 1.1;
    while (*str) h += *str++ * power(polynomialKoefficient, argument++);
    return h;
}
```

The function is used to determine the HASH index of the table. Of course, there is a possibility of a collision, then the books will be written in the same table cell, and presented as a linked list.

```cpp
HashType HashIndex(HashType hash) {
    return hash % TABLE_SIZE;
}
```

# Finite-state machine abstraction

Using enumerations defines all possible states.

```cpp
// Defining Machine's state
typedef enum {
    EXIT = 0,
    createNew = 1,
    read = 2,
    add = 3,
    del = 4,
    find = 5,
    Main = 6,
    SubMenu = -2,
    idle_state = -1
} finite_Machine_states;

// Creating variable which stores current state
finite_Machine_states currentState;
```
## Initialization and execution:

In this program, the finite state machine is represented as an array of pointers to functions. The dimension of the array depends on the number of possible states.

```cpp
int main(){
/* ..... */
 void (*Action[6])();
    Action[createNew] = act_create_library;  // Prompt to add books -> fill a table -> edit the table -> writting to FILE
    Action[read] = act_read_from_library;    // Reading from FILE  -> fill a table -> edit the table -> writing to FILE
    Action[add] = act_book_add;              // Add a book to the table
    Action[del] = act_book_del;              // Delete a book on the HASH key
    Action[find] = act_book_find;            // Find a book on the HASH key
    Action[Main] = act_mainMenu;             // Show main menu

    currentState = idle_state;               // Stetting current machine state
   
    while (1) {                              // Execution the programm
        if (currentState == idle_state) {
            act_mainMenu();
            scanf("%d", &currentState);
            (*Action[currentState])();
        }
        if (currentState == SubMenu) {
            act_subMenu();
            scanf("%d", &currentState);
            (*Action[currentState])();
        }
        if (currentState == EXIT) {
            break;
        }
    }
    /* ..... */
    retutn 0;
}
```

