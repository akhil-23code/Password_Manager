#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct {
    char username[50];
    char encryptedPassword[50];
} PasswordEntry;

typedef struct HistoryNode {
    char oldPassword[50];
    struct HistoryNode* next;
} HistoryNode;

typedef struct Stack {
    char action[100];
    struct Stack* next;
} Stack;

#define TABLE_SIZE 100
PasswordEntry* passwordTable[TABLE_SIZE];
char* siteNames[TABLE_SIZE];

Stack* undoStack = NULL;
HistoryNode* passwordHistory = NULL;

unsigned int hash(const char* site) {
    unsigned int hashValue = 0;
    while (*site) {
        hashValue = (hashValue << 5) + *site++;
    }
    return hashValue % TABLE_SIZE;
}

void addPassword(const char* site, const char* username, const char* password) {
    unsigned int index = hash(site);

    PasswordEntry* entry = (PasswordEntry*)malloc(sizeof(PasswordEntry));
    strcpy(entry->username, username);
    strcpy(entry->encryptedPassword, password);

    passwordTable[index] = entry;
    siteNames[index] = strdup(site);

    Stack* newStack = (Stack*)malloc(sizeof(Stack));
    sprintf(newStack->action, "add %s", site);
    newStack->next = undoStack;
    undoStack = newStack;

    printf("Password added for %s\n", site);
}

char* retrievePassword(const char* site) {
    unsigned int index = hash(site);
    if (passwordTable[index]) {
        return passwordTable[index]->encryptedPassword;
    } else {
        return "Password not found";
    }
}

void updatePassword(const char* site, const char* newPassword) {
    unsigned int index = hash(site);
    if (passwordTable[index]) {
        HistoryNode* newHistoryNode = (HistoryNode*)malloc(sizeof(HistoryNode));
        strcpy(newHistoryNode->oldPassword, passwordTable[index]->encryptedPassword);
        newHistoryNode->next = passwordHistory;
        passwordHistory = newHistoryNode;

        strcpy(passwordTable[index]->encryptedPassword, newPassword);

        Stack* newStack = (Stack*)malloc(sizeof(Stack));
        sprintf(newStack->action, "update %s", site);
        newStack->next = undoStack;
        undoStack = newStack;

        printf("Password updated for %s\n", site);
    } else {
        printf("Site not found\n");
    }
}

void deletePassword(const char* site) {
    unsigned int index = hash(site);
    if (passwordTable[index]) {
        HistoryNode* newHistoryNode = (HistoryNode*)malloc(sizeof(HistoryNode));
        strcpy(newHistoryNode->oldPassword, passwordTable[index]->encryptedPassword);
        newHistoryNode->next = passwordHistory;
        passwordHistory = newHistoryNode;

        free(passwordTable[index]);
        passwordTable[index] = NULL;
        free(siteNames[index]);
        siteNames[index] = NULL;

        Stack* newStack = (Stack*)malloc(sizeof(Stack));
        sprintf(newStack->action, "delete %s", site);
        newStack->next = undoStack;
        undoStack = newStack;

        printf("Password deleted for %s\n", site);
    } else {
        printf("Site not found\n");
    }
}

void undo() {
    if (undoStack == NULL) {
        printf("No action to undo\n");
        return;
    }

    Stack* lastAction = undoStack;
    undoStack = undoStack->next;

    if (strncmp(lastAction->action, "add", 3) == 0) {
        char site[50];
        sscanf(lastAction->action, "add %s", site);
        deletePassword(site);
    } else if (strncmp(lastAction->action, "update", 6) == 0) {
        char site[50];
        sscanf(lastAction->action, "update %s", site);
        if (passwordHistory) {
            HistoryNode* temp = passwordHistory;
            passwordHistory = passwordHistory->next;
            updatePassword(site, temp->oldPassword);
            free(temp);
        }
    } else if (strncmp(lastAction->action, "delete", 6) == 0) {
        char site[50];
        sscanf(lastAction->action, "delete %s", site);
        printf("Restoring deleted password for %s (from history)\n", site);
    }

    free(lastAction);
    printf("Undo action complete.\n");
}

char* generatePassword(int length) {
    static char password[50];
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()";
    srand(time(NULL));

    for (int i = 0; i < length; i++) {
        password[i] = charset[rand() % (sizeof(charset) - 1)];
    }
    password[length] = '\0';
    return password;
}

void addPasswordFromUser() {
    char site[50], username[50], password[50];
    int choice;

    printf("Enter site name: ");
    scanf("%s", site);

    printf("Enter username: ");
    scanf("%s", username);

    printf("Do you want to (1) Enter your own password or (2) Generate a random password? ");
    scanf("%d", &choice);

    if (choice == 1) {
        printf("Enter password: ");
        scanf("%s", password);
    } else if (choice == 2) {
        int length;
        printf("Enter the length for the random password: ");
        scanf("%d", &length);
        strcpy(password, generatePassword(length));
        printf("Generated password: %s\n", password);
    } else {
        printf("Invalid choice, using default password option.\n");
        return;
    }

    addPassword(site, username, password);
}

void showPasswordList() {
    int printed = 0;
    printf("\nList of stored sites:\n");

    for (int i = 0; i < TABLE_SIZE; i++) {
        if (siteNames[i] != NULL) {
            printf("- %s\n", siteNames[i]);
            printed = 1;
        }
    }

    if (!printed) {
        printf("No passwords stored.\n");
    }

    printf("Do you want to display passwords? (1 for Yes, 0 for No): ");
    int choice;
    scanf("%d", &choice);

    if (choice == 1) {
        for (int i = 0; i < TABLE_SIZE; i++) {
            if (siteNames[i] != NULL) {
                printf("Site: %s, Password: %s\n", siteNames[i], passwordTable[i]->encryptedPassword);
            }
        }
    }
}

void printMenu() {
    printf("\nPassword Manager\n");
    printf("1. Add password\n");
    printf("2. Retrieve password\n");
    printf("3. Update password\n");
    printf("4. Delete password\n");
    printf("5. Undo last action\n");
    printf("6. Generate random password\n");
    printf("7. Show list of stored site names\n");
    printf("8. Exit\n");
    printf("Choose an option: ");
}

int main() {
    int choice;

    while (1) {
        printMenu();
        scanf("%d", &choice);

        if (choice == 1) {
            addPasswordFromUser();
        } else if (choice == 2) {
            char site[50];
            printf("Enter site name to retrieve password: ");
            scanf("%s", site);
            printf("Password for %s: %s\n", site, retrievePassword(site));
        } else if (choice == 3) {
            char site[50], newPassword[50];
            printf("Enter site name to update password: ");
            scanf("%s", site);
            printf("Enter new password: ");
            scanf("%s", newPassword);
            updatePassword(site, newPassword);
        } else if (choice == 4) {
            char site[50];
            printf("Enter site name to delete password: ");
            scanf("%s", site);
            deletePassword(site);
        } else if (choice == 5) {
            undo();
        } else if (choice == 6) {
            int length;
            printf("Enter length for random password: ");
            scanf("%d", &length);
            printf("Generated password: %s\n", generatePassword(length));
        } else if (choice == 7) {
            showPasswordList();
        } else if (choice == 8) {
            printf("Exiting...\n");
            break;
        } else {
            printf("Invalid option, please try again.\n");
        }
    }

    return 0;
}