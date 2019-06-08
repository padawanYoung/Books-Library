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
Using this console program you able to create your own books library, and save it into file. You just have to run thie program, and select “Create new file” option in main menu. Then, you have to add couple of information about your favorite book: Book’s author, Book’s title, publisthing date (dd/mm/yyy), pages quantiy.  You can add book one by one, find book by title, and even delete it. Each book you added listed in hash table. This table, stores in RAM memory, untill you exit the program. When you close the program, library writes into file. When you want to look at your library, you can run the programm and choose “Open existing library”. The program will show all books that you added. When you run this program, it makes memory leask test before executing the main loop. The test is to create 100 books, write them into table and delete it in random order. Then you will able to check program on possible memory leaks.
