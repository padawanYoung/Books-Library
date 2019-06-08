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

# Finite-state machine



```cpp
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
```
