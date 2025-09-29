#include <stdio.h> 
#include <string.h>
#include <inttypes.h>
#include <stdbool.h>
#include <time.h>
#include <stdlib.h>
#include "libs/dynarray.h"
#include "libs/ansi_colors.h"

// TODO: Add 'cancel operation' option

void clearScreen() 
{
    printf("\e[1;1H\e[2J");
}

void removeNewLine(char *string) 
{
    size_t length = strlen(string);
    if ((length > 0) && (string[length - 1] == '\n')) 
    {
        string[length - 1] = '\0';
    }
}

char *generateTimestamp() 
{
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
        
    printf("Generating Timestamp\n");

    char *str = malloc(26);

    if (t->tm_mday < 10) 
    {
        snprintf(str, 26, " | Created At: 0%d.%d.%d", t->tm_mday, t->tm_mon + 1, t->tm_year + 1900);    
    } 
    else if (t->tm_mon + 1 < 10)
    {
        snprintf(str, 26, " | Created At: %d.0%d.%d", t->tm_mday, t->tm_mon + 1, t->tm_year + 1900);
    } 
    else if (t->tm_mday < 10 && t->tm_mon + 1 < 10) 
    {
        snprintf(str, 26, " | Created At: 0%d.0%d.%d", t->tm_mday, t->tm_mon + 1, t->tm_year + 1900);
    } 
    else 
    {
        snprintf(str, 26, " | Created At: %d.%d.%d", t->tm_mday, t->tm_mon + 1, t->tm_year + 1900);
    }

    if (str == NULL) 
    {
        fprintf(stderr, "Error: Unable to return timestamp!\n");
        exit(EXIT_FAILURE);
    }

    return str;
}

char *extract_last_word(char *string) 
{
    int len = strlen(string);
    
    int i = len - 1;
    while (i >= 0 && string[i] != ' ') {
        i--;
    }

    int word_len = len - i - 1;
    char *result = malloc(word_len + 1);
    
    strcpy(result, string + i + 1);
    
    string[i + 1] = '\0';

    return result;
}

void printMenu() 
{
    clearScreen();
    printf("To-Do List\n");
    printf("1. Add Task\n");
    printf("2. List Tasks\n");
    printf("3. Change Status\n");
    printf("4. Remove Task\n");
    printf("5. Save\n");
    printf("6. Load\n");
    printf("7. Exit\n");
}

void addTask(DynArr *a) 
{
    clearScreen();
    printf("Enter Task: ");
    char task_buffer[256];
    fgets(task_buffer, sizeof(task_buffer), stdin);

    char *new_task_str = strdup(task_buffer); 
    removeNewLine(new_task_str);
    
    // Add Timestamp
    char *timestamp = generateTimestamp();
    strcat(new_task_str, timestamp);
    
    // Add Status
    strcat(new_task_str, " | Status: Open\n"); 
    if (new_task_str == NULL) 
    {
        // Handle memory allocation failure
        fprintf(stderr, "Failed to allocate memory for task.\n");
        return;
    }

    insertArray(a, &new_task_str);
    return;
}

void listTasks(DynArr a) 
{
    clearScreen();
    printf("Task List:\n");
    
    char **tasks = (char**)a.arr;

    for (size_t i = 0; i < a.used; i++) 
    {
        if (tasks[i] != NULL) 
        {
            char *msg = tasks[i];
            char *status = extract_last_word(msg);

            // Print with colors
            if (strcmp(status, "Done") == 0)
                printf("%zu. %s:" BGRN "%s" reset "\n", i + 1, msg, status);
            else
                printf("%zu. %s:" BWHT "%s" reset "\n", i + 1, msg, status);
        }
    }
}

void changeStatus(DynArr a) 
{
    clearScreen();
    listTasks(a);

    int32_t index = 0;
    printf("Select Task: ");
    scanf("%d", &index);
    
    if (index < 0 || index >= a.size) 
    {
        printf("Invalid index.\n");
        return;
    }
    
    char **tasks = (char**)a.arr;
    char *task = tasks[index - 1];
    
    if (task == NULL) 
    {
        return; // Handle null pointers
    }
    
    char *lastSpace = strrchr(task, ' ');
    char *targetStart = (lastSpace != NULL) ? lastSpace + 1 : task;
    
    const char *newWord = "Done\n";
    size_t newWordLen = strlen(newWord);
    size_t available = strlen(task) - (targetStart - task);

    if (newWordLen > available)
    {
        printf("Error: New word too long, need to reallocate.\n");
        return;
    }

    strcpy(targetStart, newWord);
}

void removeTask(DynArr *a) 
{
    clearScreen();
    listTasks(*a);

    int32_t index = 0;
    printf("Select Task: ");
    scanf("%d", &index);

    removeItem(a, index - 1);
}

void saveTasks(DynArr a) 
{
    clearScreen();
    printf("Enter Path (Use // for '\'): ");
    char path_buffer[256];
    fgets(path_buffer, sizeof(path_buffer), stdin);

    path_buffer[strcspn(path_buffer, "\n")] = 0;

    FILE *fptr;
    fptr = fopen(path_buffer, "w");

    char **tasks = (char**)a.arr;
    for (size_t i = 0; i < a.used; i++) 
    {
        // Check for a null pointer
        if (tasks[i] != NULL) 
        { 
            fprintf(fptr, tasks[i]);
        }
    }

    fclose(fptr);
    return;
}

void loadTasks(DynArr *a) 
{
    clearScreen();
    printf("Enter Path (Use // for '\'): ");
    char path_buffer[256];
    fgets(path_buffer, sizeof(path_buffer), stdin);

    path_buffer[strcspn(path_buffer, "\n")] = 0;

    FILE *fptr;
    fptr = fopen(path_buffer, "r");

    char data[256];
    while(fgets(data, 256, fptr)) {
        char *task_str = strdup(data); 
        insertArray(a, &task_str);
    }

    fclose(fptr);
    return;
}

int main() 
{
    DynArr tasks;
    initArray(&tasks, 2, sizeof(char*));
    
    bool exit = false;

    while (!exit) 
    {
        printMenu();
        
        int32_t action;
        printf("Choice: ");
        scanf("%d", &action);
        
        int32_t c;
        while ((c = getchar()) != '\n' && c != EOF) {}

        switch (action)
        {
            case 1:
                addTask(&tasks);
                break;
            
            case 2:
                listTasks(tasks);
                printf("Press Enter to return to menu");   
                getchar();
                break;

            case 3:
                changeStatus(tasks);
                printf("Press Enter to return to menu");   
                getchar();
                break;

            case 4:
                removeTask(&tasks);
                printf("Press Enter to return to menu");   
                getchar();
                break;

            case 5:
                saveTasks(tasks);
                break;

            case 6:
                loadTasks(&tasks);
                break;

            case 7:
                clearScreen();
                freeArray(&tasks);
                exit = true;
                break;

            default:
                printf("Invalid Input!\n");
                printf("Press Enter to return to menu"); 
                getchar();
                break;
        }
    }

    return 0;
}