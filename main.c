#include <stdio.h>
#include <stdlib.h>

#define LENGTH 20
#define TABLE_SIZE 16

enum finite_Machine_states {
    EXIT = 0,
    state1 = 1,
    state2 = 2,
    state3 = 3,
    state4 = 4,
    state5 = 5,
    state9 = 9,
    idle_state1 = -1,
    idle_state2 = -2
};
enum permissions {
    yes = 'y', no = 'n', z_state = 'z'
};

//typedef unsigned char HashType;
typedef unsigned int HashType;

typedef struct datesPattern {
    unsigned int year;
    unsigned int day;
    unsigned int month;
} publishingDate;
typedef struct booksPattern {
    HashType hash;
    char author[LENGTH];
    char title[LENGTH];
    struct datesPattern *date;
    unsigned int pagesQuantity;
    struct booksPattern *next;
    struct booksPattern *prev;
} Book;

Book *TABLE[TABLE_SIZE];
FILE *fptr;
int currentState;
char permission;
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

void delete_book_from_list();

float power(float value, int argument);

HashType Hash(char *str);

HashType HashIndex(HashType hash);

void clearTable();

int main() {
    void (*Action[6])();
    Action[state1] = act_create_library;
    Action[state2] = act_read_from_library;
    Action[state3] = act_book_add;
    Action[state4] = act_book_del;
    Action[state5] = act_book_find;
    Action[state9] = act_mainMenu;

    currentState = idle_state1;
    while (1) {
        if (currentState == idle_state1) {
            act_mainMenu();
            scanf("%d", &currentState);
            (*Action[currentState])();
        }
        if (currentState == idle_state2) {
            currentState = idle_state1;
            act_subMenu();
            scanf("%d", &currentState);
            (*Action[currentState])();
        }
        if (currentState == EXIT) break;
    }
    printf("\nProgram was closed\n");
    clearTable();
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
    printf("-{3 ADD}--{4 DEL}--{5 FIND}--{9 MAIN MENU}--{0 EXIT}-  \n");
    printf("====================================================\n");
}

void act_create_library() {
    fptr = fopen("BooksLibrary.txt", "w");
    printf("\nFile BooksLibrary.txt was created\n"
                   "Would you like to add few books to new library? y/n\n");
    scanf("%s", &permission);
    if (permission == yes) {
        permission = z_state;
        clearTable();
        act_book_add();
        currentState = idle_state2;
    }
    if (permission == no) {
        permission = z_state;
        currentState = idle_state1;
    }
}

void act_read_from_library() {
//    printf("act_showLibrary -> 1st state\n");
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
            currentState = idle_state1;
        }
    } else {
        readListFromLibrary();
        displayList();
        currentState = idle_state2;
    }
}

void displayList() {
    Book *swap = NULL;
    for (int i = 0; i < TABLE_SIZE; ++i) {
        swap = TABLE[i];
        if (swap == NULL) continue;
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
        free(swap);
    }
}

void writeBooksList2File() {
    fptr = fopen("BooksLibrary.txt", "a");
    Book *temp = NULL;
    for (int i = 0; i < TABLE_SIZE; ++i) {
        temp = TABLE[i];
        if (temp == NULL) continue;
        while (temp != NULL) {
            fprintf(fptr, "%u\t", temp->hash);
            fprintf(fptr, "%s\t", temp->author);
            fprintf(fptr, "%s\t", temp->title);
            fprintf(fptr, "%u\t", temp->pagesQuantity);
            fprintf(fptr, "%u\t%u\t%u\n", temp->date->day, temp->date->month, temp->date->year);
            temp = temp->next;
        }
        free(temp);
    }
    fclose(fptr);
}

void readListFromLibrary() {
    Book *temp = NULL;
    Book *p = NULL;
    Book *prevBook = NULL;
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

    fclose(fptr);
    free(p);
    free(temp);
    free(prevBook);
}

void createBooksList() {
    int i = 1;
    Book *prevBook = NULL;
    Book *temp = NULL;
    Book *p = NULL;
    permission = yes;
    while (permission == yes) {
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
        scanf("%s", &permission);
        if (permission == no)break;
    }
    free(p);
    free(temp);
    free(prevBook);
}

void delete_book_from_list() {
    /*
     * According function act_book_find() we get to structure which we want to delete.
     * So we need to redefine next and revious link in previous and next strucrures, and we should exeptions,
     * when this structure is first or last possition in this TABLE[hashIndex] list
     * */
    Book *nextBook;
    Book *prevBook;
    if (TABLE[hashIndex]->next == NULL){
        prevBook=TABLE[hashIndex]->prev;
        prevBook->next=NULL;
        free(TABLE[hashIndex]);
    }
    if (TABLE[hashIndex]->prev == NULL){
        nextBook=TABLE[hashIndex]->next;
        nextBook->prev=NULL;
        free(TABLE[hashIndex]);
    }
    else {
        prevBook = TABLE[hashIndex]->prev;
        nextBook = TABLE[hashIndex]->next;
        prevBook->next = nextBook;
        nextBook->prev = prevBook;
        free(TABLE[hashIndex]);
    }
//    free(nextBook);
//    free(prevBook);
    currentState = idle_state1;
}

void act_book_add() {
    createBooksList();
    writeBooksList2File();
    currentState = idle_state1;
}

void act_book_del() {
    act_book_find();
    printf("Are you sure that you want to delete this book from library? y/n\n");
    if (scanf("%c",&permission)==yes){
        currentState = idle_state1;
        delete_book_from_list();
        printf("Book was deleted\n");
    }
    if (scanf("%c",&permission)==no){
        currentState = idle_state1;
        hashIndex = 0;
    }
}

void act_book_find() {
    currentState = idle_state1;
    permission = z_state;

    hashIndex = 0;
    char title[LENGTH];

    readListFromLibrary();

    printf("\nPlease, enter book's title: ");
    scanf("%s", title);

    hash = Hash(title);
    hashIndex = HashIndex(hash);

    if (TABLE[hashIndex] == NULL)
        printf("Book wasn't found\n");

    while (TABLE[hashIndex] != NULL) {
        if (TABLE[hashIndex]->hash == hash) {
            printf("\n The book with title \'%s\' was found\n"
                           "Do you want to look all information about it? y/n\n", title);
            if (scanf("%c", &permission) == yes) {
                displayList();
                break;
            }
        }
        TABLE[hashIndex] = TABLE[hashIndex]->next;
    }
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
        free(TABLE[i]);
    }
}