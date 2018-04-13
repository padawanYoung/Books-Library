#include <stdio.h>
#include <stdlib.h>
#include <afxres.h>

#define TABLE_SIZE 16

#define BOOKS_QUANTITY 100
#define TEST_ITERATIONS 10000

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
    char *author;
    char *title;
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

HashType titlesHash[BOOKS_QUANTITY];

void act_mainMenu();

void act_subMenu();

void act_create_library();

void act_read_from_library();

void act_book_add();

void act_book_find();

void act_book_del();

void createBooksList();

void displayList();

void writeBooksList2File();

void delete_book_from_list(Book **book, HashType hash);

void displaySelectedBook(Book *SelectedBook);

Book *readListFromLibrary(Book *table[]);

Book *findBook(Book *table[], HashType hash, HashType hashIndex);

float power(float value, int argument);

HashType Hash(char *str);

HashType HashIndex(HashType hash);

char *ReadStringFromInputStream(FILE *pointer, char SeparateSymbol, int fflushFlag);

void printString(char *string);

void clearTable();

void createRandBooksList();

unsigned int randRange(unsigned int min, unsigned int max);

char *randomCharField(int length);

void bookDelInRandOrder();

void wait(int seconds);

int main() {
    setbuf(stdout, 0);
    printf("Delay 10 sec, then test will started\n");
    wait(10);
    for (int j = 0; j < TABLE_SIZE; ++j) TABLE[j] = NULL;

    for (int j = 0; j < TABLE_SIZE; ++j) {
        TABLE[j] = NULL;
    }
    for (int i = 0; i <TEST_ITERATIONS ; ++i) {
        createRandBooksList();
        bookDelInRandOrder();
        printf("%d\n",i+1);
    }

    void (*Action[6])();
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
        if (currentState == EXIT) {
            break;
        }
    }
    clearTable();
    printf("Program was closed\n");
    return 0;
}

void act_mainMenu() {
    printf("1 - Create a new file\n");
    printf("2 - Open exist book's library\n");
    printf("0 - Close the program\n");
    printf("------------------------------------------------------\n");
}

void act_subMenu() {
    printf("-{3 ADD}--{4 DEL}--{5 FIND}--{6 MAIN MENU}--{0 EXIT}-  \n");
    printf("====================================================\n");
}

void act_create_library() {
    fptr = fopen("BooksLibrary.txt", "w+");
    fclose(fptr);
    fptr = NULL;
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
        clearTable();
        *TABLE = readListFromLibrary(TABLE);
        displayList();
        currentState = SubMenu;
    }
    fclose(fptr);


}

void displayList() {
    Book *temp = NULL;
    for (int i = 0; i < TABLE_SIZE; i++) {
        temp = TABLE[i];
        while (temp != NULL) {
            printf("Book #%d\n", i);
            printf("Author: ");
            printString(temp->author);
            printf("Title: ");
            printString(temp->title);
            printf("HASH: %u\n", temp->hash);
            printf("Pages: %u\n", temp->pagesQuantity);
            printf("Date: %u/%u/%u\n", temp->date->day, temp->date->month, temp->date->year);
            printf("====================================================\n");
            temp = temp->next;
        }
    }
}

void writeBooksList2File() {
    Book *temp = NULL;
    for (int i = 0; i < TABLE_SIZE; ++i) {
        temp = TABLE[i];
        while (temp != NULL) {
            fprintf(fptr, "%u{", temp->hash);
            fprintf(fptr, "%s;%s;", temp->author, temp->title);
            fprintf(fptr, "%u;", temp->pagesQuantity);
            fprintf(fptr, "%u/%u/%u}\n", temp->date->day, temp->date->month, temp->date->year);
            temp = temp->next;
        }
    }
}

Book *readListFromLibrary(Book *table[]) {
    Book *temp = NULL;
    Book *p = NULL;
    temp = (Book *) malloc(sizeof(Book));
    temp->date = (publishingDate *) malloc(sizeof(publishingDate));

    while (fscanf(fptr, "%u{", &temp->hash) != EOF) {

        hashIndex = HashIndex(temp->hash);
        temp->author = ReadStringFromInputStream(fptr, ';', 0);
        temp->title = ReadStringFromInputStream(fptr, ';', 0);
        fscanf(fptr, "%u;%u/%u/%u}\n",
               &temp->pagesQuantity,
               &temp->date->day, &temp->date->month, &temp->date->year);
        temp->next = NULL;
        temp->prev = NULL;

        if (table[hashIndex] == NULL) {
            table[hashIndex] = temp;
        } else {
            p = table[hashIndex];
            while (p->next != NULL) {
                p = p->next;
            }
            p->next = temp;
            p->next->prev = p;
        }
        temp = (Book *) malloc(sizeof(Book));
        temp->date = (publishingDate *) malloc(sizeof(publishingDate));
    }
    return *table;
}

void createBooksList() {
    int i = 1;
    Book *temp = NULL;
    Book *p = NULL;
    permission = yes;

    while (permission == yes) {
        permission = z_state;
        temp = (Book *) malloc(sizeof(Book));
        temp->date = (publishingDate *) malloc(sizeof(publishingDate));
        printf("Enter the data for Book's number: %d\n"
                       "\tAuthor: ", i++);
        temp->author = ReadStringFromInputStream(stdin, '\n', 1);
        printf("\tTitle: ");
        temp->title = ReadStringFromInputStream(stdin, '\n', 1);
        printf("\tPages' quantity: ");
        scanf("%u", &(temp->pagesQuantity));
        printf("\n Date in format -> dd/mm/yyy: ");
        scanf("%u%*c%u%*c%u", &(temp->date->month), &(temp->date->day), &(temp->date->year));

        temp->hash = Hash(temp->title);
        hashIndex = HashIndex(temp->hash);

        temp->next = NULL;
        temp->prev = NULL;

        if (TABLE[hashIndex] == NULL) {
            TABLE[hashIndex] = temp;
        } else {
            p = TABLE[hashIndex];
            while (p->next != NULL) {
                p = p->next;
            }
            p->next = temp;
            p->next->prev = p;
        }
        printf("\nWould you like to add one more book? y/n\n");
        if (scanf("%s", &permission) == no)break;
    }
    permission = z_state;
}

void delete_book_from_list(Book **book, HashType hash) {
    Book *prevNode = NULL;
    Book *selectedNode = *book;

    if (selectedNode != NULL && selectedNode->hash == hash) {
        *book = selectedNode->next;

        free(selectedNode->author);
        free(selectedNode->title);
        free(selectedNode->date);
        free(selectedNode);
        selectedNode = NULL;
        return;
    }

    while (selectedNode != NULL && selectedNode->hash != hash) {
        prevNode = selectedNode;
        selectedNode = selectedNode->next;
    }
    if (selectedNode == NULL) {
        printf("Doesn't exist\n");
        return;
    }
    prevNode->next=selectedNode->next;
    prevNode->prev=selectedNode->prev;

    free(selectedNode->author);
    free(selectedNode->title);
    free(selectedNode->date);
    free(selectedNode);
    selectedNode = NULL;
}

void act_book_add() {
    fptr = fopen("BooksLibrary.txt", "w+");
    createBooksList();
    displayList();
    writeBooksList2File();
    fclose(fptr);
    currentState = idle_state;
}

void displaySelectedBook(Book *SelectedBook) {
    printf("Author: ");
    printString(SelectedBook->author);
    printf("Title: ");
    printString(SelectedBook->title);
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
    while (SelectedBook != NULL) {
        if (SelectedBook->hash == hash) {
            return SelectedBook;
        }
        SelectedBook = SelectedBook->next;
    }
    return SelectedBook;
}

void act_book_del() {
    permission = z_state;
    hashIndex = 0;
    char *title;
    Book *SelectedBook = NULL;

    printf("Please, enter book's title: \n");
    title = ReadStringFromInputStream(stdin, '\n', 1);

    hash = Hash(title);
    hashIndex = HashIndex(hash);

    SelectedBook = findBook(TABLE, hash, hashIndex);
    displaySelectedBook(SelectedBook);

    printf("Are you sure that you want to delete this book from library? y/n\n");
    scanf("%s", &permission);
    if (permission == yes) {
        permission = z_state;
        delete_book_from_list(&TABLE[hashIndex], hash);
        printf("Book was deleted\n");

        fptr = fopen("BooksLibrary.txt", "w+");
        writeBooksList2File();
        fclose(fptr);

        printf("Lirary was updated\n");
    }
    currentState = idle_state;
}

void act_book_find() {
    hashIndex = 0;
    char *title;
    Book *SelectedBook = NULL;
    printf("Please, enter book's title: \n");
    title = ReadStringFromInputStream(stdin, '\n', 1);
    hash = Hash(title);
    hashIndex = HashIndex(hash);
    SelectedBook = findBook(TABLE, hash, hashIndex);
    if (SelectedBook != NULL)
        displaySelectedBook(SelectedBook);
    else printf("Book with title %s wasn't found\n", title);
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
        free(TABLE[i]);
        TABLE[i] = NULL;
    }
}

char *ReadStringFromInputStream(FILE *pointer, char SeparateSymbol, int fflushFlag) {
    char *string;
    int i = 0;
    int j = 1;
    int temp = 0;
    if (fflushFlag == 1) {
        fflush(pointer);
    }
    string = (char *) malloc(sizeof(char));
    while ((char) temp != SeparateSymbol) {
        temp = getc(pointer);
        string = (char *) realloc(string, j++ * sizeof(char));
        string[i] = (char) temp;
        i++;
    }
    string[i - 1] = '\0';
    return string;
}

void printString(char *string) {
    while (*string) {
        printf("%c", *string++);
    }
    printf("\n");
}
unsigned int randRange(unsigned int min, unsigned int max) {
    double scaled = (double) rand() / RAND_MAX;

    return (unsigned int) ((max - min + 1) * scaled + min);
}
char *randomCharField(int length) {
    char *string = NULL;
    int temp;
    int i = 0;
    int j = 1;
    string = (char *) malloc(length+1 *sizeof(char));
    while (j <= length) {
        temp = randRange('a', 'z');
        string[i] = (char) temp;
        i++;
        j++;
    }
    string[i++] = '\0';
    return string;
}
void createRandBooksList() {
    int i = 1;
    int j = 0;
    Book *temp = NULL;
    Book *p = NULL;
//    printf ("Creating Library was started!\n");
    while (i <= BOOKS_QUANTITY) {
        temp = (Book *) malloc(sizeof(Book));
        temp->date = (publishingDate *) malloc(sizeof(publishingDate));
        temp->author = randomCharField(randRange(5, 12));
        temp->title = randomCharField(randRange(5, 12));
        temp->pagesQuantity = randRange(80, 999);
        temp->date->month = randRange(1, 12);
        temp->date->day = randRange(1, 31);
        temp->date->year = randRange(1850, 2017);

        temp->hash = Hash(temp->title);
        titlesHash[j] = temp->hash;

        hashIndex = HashIndex(temp->hash);

        temp->next = NULL;
        temp->prev = NULL;

        if (TABLE[hashIndex] == NULL) {
            TABLE[hashIndex] = temp;
        } else {
            p = TABLE[hashIndex];
            while (p->next != NULL) {
                p = p->next;
            }
            p->next = temp;
            p->next->prev = p;
        }
        i++;
        j++;
    }
}

void bookDelInRandOrder() {
    hashIndex = 0;
    int Index = 0;
    int prevIndex = 0;
    int Done = 0;
//    printf ("Delleting was started!\n");
/*    for(int i = 0; i<100;i++){
        if (i%2 == 1){
            hash = titlesHash[i];
            hashIndex = HashIndex(hash);
            delete_book_from_list(&TABLE[hashIndex], hash);
            titlesHash[i] = 0;
        }
    }
    for(int j = 0; j<100;j++){
        if (j%2 == 0){
            hash = titlesHash[j];
            hashIndex = HashIndex(hash);
            delete_book_from_list(&TABLE[hashIndex], hash);
            titlesHash[j] = 0;
        }
    }*/
    for (int j = 0; j < BOOKS_QUANTITY ; ++j) {
        Done = 0;
        Index = randRange(0, BOOKS_QUANTITY - 1);
        prevIndex = Index;
        while (Done == 0) {
            while (titlesHash[Index] == 0) {
                if (Index++ == BOOKS_QUANTITY) {
                    for (int i = 0; i <= prevIndex; ++i) {
                        if (i == prevIndex) {
                            Done = 1;
                        }
                        if (titlesHash[i] != 0) {
                            Index = i;
                            break;
                        }
                    }
                }
                if (Done == 1) break;
            }
            if (Done == 0) {
                hash = titlesHash[Index];
                hashIndex = HashIndex(hash);
                delete_book_from_list(&TABLE[hashIndex], hash);
                titlesHash[Index] = 0;
                Done = 1;
            }
        }
    }
}

void wait(int seconds) {   // Pretty crossplatform, both ALL POSIX compliant systems AND Windows
#ifdef _WIN32
    Sleep(1000 * seconds);
#else
    sleep( seconds );
#endif
}
