#include <stdio.h>
#include <stdlib.h>

#define LENGTH 20
#define TABLE_SIZE 16

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

typedef enum {
    yes = 'y', no = 'n', z_state = 'z'
} permissions;

typedef unsigned int HashType;

typedef struct {
    unsigned int year;
    unsigned int day;
    unsigned int month;
} publishingDate;

typedef struct booksPattern {
    HashType hash;
    char author[LENGTH];
    char title[LENGTH];
    publishingDate *date;
    unsigned int pagesQuantity;
    struct booksPattern *next;
    struct booksPattern *prev;
} Book;

Book *TABLE[TABLE_SIZE];

FILE *fptr;

finite_Machine_states currentState;
permissions permission;

HashType hashIndex;
HashType hash;

void act_mainMenu();

void act_subMenu();

void act_create_library();

void act_read_from_library();

void act_book_add();

void act_book_find();

void act_book_del();

void readListFromLibrary();

void createBooksList();

void displayList();

void writeBooksList2File();

void delete_book_from_list( Book * selectedBook);

void displaySelectedBook(Book *SelectedBook);

Book *findBook(Book *table[], HashType hash, HashType hashIndex);

float power(float value, int argument);

HashType Hash(char *str);

HashType HashIndex(HashType hash);

void clearTable();

int main() {
    setbuf(stdout, 0);
    for (int j = 0; j < TABLE_SIZE; ++j) TABLE[j] = NULL;

    void (*Action [6])();
    Action[createNew] = act_create_library;
    Action[read] = act_read_from_library;
    Action[add] = act_book_add;
    Action[del] = act_book_del;
    Action[find] = act_book_find;
    Action[Main] = act_mainMenu;

    currentState = idle_state;

    while (1) {
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
        if (currentState == EXIT) break;
    }

    clearTable();
    for (int i = 0; i < 6; ++i) {
        Action[i] = NULL;
    }
    fptr = NULL;
    printf("\nProgram was closed\n");
    return 0;
}

void act_mainMenu() {
    printf("=====================================================\n");
    printf("1 - Create a new file\n");
    printf("2 - Open exist book's library\n");
    printf("3 - Add book to library\n");
    printf("4 - Delete book from library\n");
    printf("5 - Find book from library\n");
    printf("0 - Close the program\n");
    printf("------------------------------------------------------\n");
}

void act_subMenu() {
    printf("-{3 ADD}--{4 DEL}--{5 FIND}--{6 MAIN MENU}--{0 EXIT}-  \n");
    printf("====================================================\n");
}

void act_create_library() {
    fptr = fopen("BooksLibrary.txt", "r+");
    fclose(fptr);
    fptr=NULL;
    printf("\nFile BooksLibrary.txt was created\n"
                   "Would you like to add few books to new library? y/n\n");
    scanf("%s", &permission);
    if (permission == yes) {
        permission = z_state;
        currentState = SubMenu;
        act_book_add();
    }
    if (permission == no) {
        permission = z_state;
        currentState = idle_state;
    }
}

void act_read_from_library() {

    if ((fptr = fopen("BooksLibrary.txt", "r")) == NULL) {
        printf("----------------------------------------------------\n");
        printf("Error! opening file. Perhaps file doesn't exist\n");
        printf("----------------------------------------------------\n");
        printf("Would you like to create new library? y/n\n");
        scanf("%s", &permission);
        if (permission == yes) {
            permission = z_state;
            act_create_library();
        }
        if (permission == no) {
            permission = z_state;
            currentState = idle_state;
        }
    } else {
        readListFromLibrary();
        displayList();
        currentState = SubMenu;
    }
    fclose(fptr);
    fptr = NULL;
}

void displayList() {
    Book *swap = NULL;
    for (int i = 0; i < TABLE_SIZE; i++) {
        swap = TABLE[i];
        while (swap != NULL) {
            printf("Book #%d\n", i);
            printf("Author: %s\n", swap->author);
            printf("Title: %s\n", swap->title);
            printf("HASH: %u\n", swap->hash);
            printf("Pages: %u\n", swap->pagesQuantity);
            printf("Date: %u/%u/%u\n", swap->date->day, swap->date->month, swap->date->year);
            printf("====================================================\n");
            swap = swap->next;
        }
    }
    swap = NULL;
}

void writeBooksList2File() {
//    fptr = fopen("BooksLibrary.txt", "r+");
    Book *temp = NULL;
    for (int i = 0; i < TABLE_SIZE; ++i) {
        temp = TABLE[i];
        while (temp != NULL) {
            fprintf(fptr, "%u\t", temp->hash);
            fprintf(fptr, "%s\t", temp->author);
            fprintf(fptr, "%s\t", temp->title);
            fprintf(fptr, "%u\t", temp->pagesQuantity);
            fprintf(fptr, "%u\t%u\t%u\n", temp->date->day, temp->date->month, temp->date->year);
            temp = temp->next;
        }
    }
    temp = NULL;
//    fclose(fptr);
//    fptr = NULL;
}

void readListFromLibrary() {
    Book *temp = NULL;
    Book *p = NULL;
    Book *prevBook = NULL;
    clearTable();
    int eof = 0;
    do {
        temp = (Book *) malloc(sizeof(Book));
        temp->date = (publishingDate *) malloc(sizeof(publishingDate));
        if (fscanf(fptr, "%u\t%s\t%s\t%u\t%u\t%u\t%u\n",
                   &temp->hash, temp->author, temp->title,
                   &temp->pagesQuantity, &temp->date->day,
                   &temp->date->month, &temp->date->year) == EOF) {
            eof = 1;
        }
        temp->next = NULL;
        temp->prev = prevBook;
        prevBook = temp;
        hashIndex = HashIndex(temp->hash);
        if (TABLE[hashIndex] == NULL) { // if list currently empty as first Book
            TABLE[hashIndex] = temp;
        } else {
            p = TABLE[hashIndex];
            while (p->next != NULL) {
                p = p->next;
            }
            p->next = temp;
        }
    } while (eof == 0);
    prevBook = NULL;
    p = NULL;
    temp = NULL;
}

void createBooksList() {
    int i = 1;
    Book *prevBook = NULL;
    Book *temp = NULL;
    Book *p = NULL;
    permission = yes;
    while (permission == yes) {
        permission = z_state;
        //individual isolated node
        temp = (Book *) malloc(sizeof(Book));
        temp->date = (publishingDate *) malloc(sizeof(publishingDate));
        printf("Enter the data for Book's number: %d\n"
                       "\tAuthor: ", i++);
        scanf("%s", temp->author);
        printf("\tTitle: ");
        scanf("%s", temp->title);
        printf("\tPages' quantity: ");
        scanf("%u", &(temp->pagesQuantity));
        printf("\n Date in format -> dd/mm/yyy: ");
        scanf("%u%*c%u%*c%u", &(temp->date->month), &(temp->date->day), &(temp->date->year));

        temp->hash = Hash(temp->title);
        hashIndex = HashIndex(temp->hash);

        temp->next = NULL;
        temp->prev = prevBook;
        prevBook = temp;

        if (TABLE[hashIndex] == NULL) { // if list currently empty as first Book
            TABLE[hashIndex] = temp;
        } else {
            p = TABLE[hashIndex];
            while (p->next != NULL) {
                p = p->next;
            }
            p->next = temp;
        }
        printf("\nWould you like to add one more book? y/n\n");
        if (scanf("%s", &permission) == no)break;
    }
    permission = z_state;
    prevBook = NULL;
    p = NULL;
    temp = NULL;
}

void delete_book_from_list( Book * selectedBook) {
    Book *nextBook = NULL;
    Book *prevBook = NULL;
//    Book *selectedBook = TABLE[hashIndex];

    if (selectedBook->next == NULL) {
        prevBook = selectedBook->prev;
        prevBook->next = NULL;
        free(selectedBook);
    }
    if (selectedBook->prev == NULL) {
        nextBook = selectedBook->next;
        nextBook->prev = NULL;
        free(selectedBook);
    } else {
        prevBook = selectedBook->prev;
        nextBook = selectedBook->next;
        prevBook->next = nextBook;
        nextBook->prev = prevBook;
        free(selectedBook);
    }
    nextBook = NULL;
    prevBook = NULL;
//    selectedBook = NULL;
    currentState = idle_state;
}

void act_book_add() {
    fptr = fopen("BooksLibrary.txt", "w");
    createBooksList();
    writeBooksList2File();
//    displayList();
    fclose(fptr);
    fptr=NULL;
    currentState = idle_state;
}

void displaySelectedBook(Book *SelectedBook) {
    printf("Author: %s\n", SelectedBook->author);
    printf("Title: %s\n", SelectedBook->title);
    printf("HASH: %u\n", SelectedBook->hash);
    printf("Pages: %u\n", SelectedBook->pagesQuantity);
    printf("Date: %u/%u/%u\n",
           SelectedBook->date->day,
           SelectedBook->date->month,
           SelectedBook->date->year);
    printf("====================================================\n");
}

Book *findBook(Book *table[], HashType hash, HashType hashIndex) {
    Book *SelectedBook = table[hashIndex];
    if (SelectedBook == NULL) return NULL;
    while (SelectedBook != NULL) {
        if (SelectedBook->hash == hash)
            return SelectedBook;
        SelectedBook = SelectedBook->next;
    }
    return SelectedBook;
}

void act_book_del() {
    permission = z_state;
    hashIndex = 0;
    char title[LENGTH];
    Book * SelectedBook=NULL;

    if ((fptr = fopen("BooksLibrary.txt", "r+")) != NULL) {
        readListFromLibrary();
        printf("Please, enter book's title: \n");
        scanf("%s", title);

        hash = Hash(title);
        hashIndex = HashIndex(hash);
        SelectedBook = findBook(TABLE, hash, hashIndex);
        displaySelectedBook(SelectedBook);

        printf("Are you sure that you want to delete this book from library? y/n\n");
        if (scanf("%c", &permission) == yes) {
            permission = z_state;
            delete_book_from_list(SelectedBook);
            printf("Book was deleted\n");
            writeBooksList2File();
            printf("Lirary was updated\n");
        }
    }else printf("Error! opening file. Perhaps file doesn't exist\n");
    fclose(fptr);
    SelectedBook = NULL;
    currentState = idle_state;
}

void act_book_find() {
    hashIndex = 0;
    char title[LENGTH];
    Book * SelectedBook=NULL;
    if ((fptr = fopen("BooksLibrary.txt", "r")) != NULL){
        readListFromLibrary();
        printf("Please, enter book's title: \n");
        scanf("%s", title);

        hash = Hash(title);
        hashIndex = HashIndex(hash);

//        if((SelectedBook = findBook(TABLE, hash, hashIndex))!= NULL)
        SelectedBook = findBook(TABLE, hash, hashIndex);
        if (SelectedBook != NULL)
            displaySelectedBook(SelectedBook);
        else printf("Book with title %s wasn't found\n", title);
    } else printf("Error! opening file. Perhaps file doesn't exist\n");
    fclose(fptr);
    fptr=NULL;
    SelectedBook = NULL;
    currentState = SubMenu;
}

float power(float value, int argument) {
    float result = 1;
    for (int i = 0; i < argument; ++i) {
        result *= value;
    }
    return result;
}

HashType Hash(char *str) {
    HashType h = 0;
    int argument = 0;
    float polynomialKoefficient = 1.1;
    while (*str) h += *str++ * power(polynomialKoefficient, argument++);
    return h;
}

HashType HashIndex(HashType hash) {
    return hash % TABLE_SIZE;
}

void clearTable() {
    for (int i = 0; i < TABLE_SIZE; ++i) {
        TABLE[i] = NULL;
    }
}