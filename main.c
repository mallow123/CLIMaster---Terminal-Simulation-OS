#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME_LENGTH 50
#define MAX_FILES 10
#define MAX_DIRS 10
#define MAX_CONTENT_SIZE 1000

// Structure to represent a file
typedef struct File {
    char name[MAX_NAME_LENGTH];
    char content[MAX_CONTENT_SIZE];  // Store file content
} File;

// Structure to represent a directory
typedef struct Directory {
    char name[MAX_NAME_LENGTH];
    struct Directory *parent;         // Parent directory
    struct Directory *subdirs[MAX_DIRS];  // Subdirectories inside this directory
    File *files[MAX_FILES];               // Files inside this directory
    int num_files;
    int num_subdirs;
} Directory;

// Current working directory pointer
Directory *current_dir;

// Root directory (initial directory)
Directory root;

// Initialize the file system
void initFileSystem() {
    strcpy(root.name, "root");
    root.parent = NULL;
    root.num_files = 0;
    root.num_subdirs = 0;
    current_dir = &root;

    // Predefine some files for demonstration (built-in files)
    File *aboutFile = (File *)malloc(sizeof(File));
    strcpy(aboutFile->name, "about.txt");
    strcpy(aboutFile->content, "This is CLIMaster - a terminal-based simulation project.");
    root.files[root.num_files++] = aboutFile;

    File *projectsFile = (File *)malloc(sizeof(File));
    strcpy(projectsFile->name, "projects.txt");
    strcpy(projectsFile->content, "Other Projects:\n1. Digital Mining Engine\n2. Self-destructing File System");
    root.files[root.num_files++] = projectsFile;

    // Create some subdirectories for demonstration
    Directory *dir1 = (Directory *)malloc(sizeof(Directory));
    strcpy(dir1->name, "projects");
    dir1->parent = &root;
    dir1->num_files = 0;
    dir1->num_subdirs = 0;
    root.subdirs[root.num_subdirs++] = dir1;

    Directory *dir2 = (Directory *)malloc(sizeof(Directory));
    strcpy(dir2->name, "files");
    dir2->parent = &root;
    dir2->num_files = 0;
    dir2->num_subdirs = 0;
    root.subdirs[root.num_subdirs++] = dir2;
}

// Create a new directory
void mkdirCommand(const char *dir_name) {
    if (current_dir->num_subdirs < MAX_DIRS) {
        Directory *new_dir = (Directory *)malloc(sizeof(Directory));
        strcpy(new_dir->name, dir_name);
        new_dir->parent = current_dir;
        new_dir->num_files = 0;
        new_dir->num_subdirs = 0;
        current_dir->subdirs[current_dir->num_subdirs++] = new_dir;
        printf("Directory '%s' created.\n", dir_name);
    } else {
        printf("Cannot create directory: Maximum directories reached.\n");
    }
}

// Change directory (supports both relative and absolute paths)
void cdCommand(const char *dir_name) {
    // If path starts with '/', it's absolute, otherwise relative
    if (dir_name[0] == '/') {
        current_dir = &root;
    }

    char *dir = strtok((char *)dir_name, "/");
    while (dir != NULL) {
        if (strcmp(dir, "..") == 0 && current_dir->parent != NULL) {
            current_dir = current_dir->parent;
            printf("Moved up to '%s'.\n", current_dir->name);
        } else {
            int found = 0;
            for (int i = 0; i < current_dir->num_subdirs; i++) {
                if (strcmp(current_dir->subdirs[i]->name, dir) == 0) {
                    current_dir = current_dir->subdirs[i];
                    printf("Changed directory to '%s'.\n", dir);
                    found = 1;
                    break;
                }
            }
            if (!found) {
                printf("Directory '%s' not found.\n", dir);
                break;
            }
        }
        dir = strtok(NULL, "/");
    }
}

// List files and directories in the current directory
void lsCommand() {
    printf("Files:\n");
    for (int i = 0; i < current_dir->num_files; i++) {
        printf("- %s\n", current_dir->files[i]->name);
    }
    printf("Directories:\n");
    for (int i = 0; i < current_dir->num_subdirs; i++) {
        printf("- %s\n", current_dir->subdirs[i]->name);
    }
}

// Create a new file
void createfileCommand(const char *file_name) {
    if (current_dir->num_files < MAX_FILES) {
        File *new_file = (File *)malloc(sizeof(File));
        strcpy(new_file->name, file_name);
        new_file->content[0] = '\0';  // Empty file content
        current_dir->files[current_dir->num_files++] = new_file;
        printf("File '%s' created.\n", file_name);
    } else {
        printf("Cannot create file: Maximum files reached.\n");
    }
}

// Display file content
void catCommand(const char *file_name) {
    for (int i = 0; i < current_dir->num_files; i++) {
        if (strcmp(current_dir->files[i]->name, file_name) == 0) {
            printf("Content of '%s':\n", file_name);
            printf("%s\n", current_dir->files[i]->content);  // Print the content of the file
            return;
        }
    }
    printf("File '%s' not found.\n", file_name);  // File not found
}

// Write content to a file
void echoCommand(const char *text, const char *file_name) {
    for (int i = 0; i < current_dir->num_files; i++) {
        if (strcmp(current_dir->files[i]->name, file_name) == 0) {
            strcat(current_dir->files[i]->content, text);
            printf("Text written to '%s'.\n", file_name);
            return;
        }
    }
    printf("File '%s' not found.\n", file_name);
}

// Remove a file
void removeFileCommand(const char *file_name) {
    for (int i = 0; i < current_dir->num_files; i++) {
        if (strcmp(current_dir->files[i]->name, file_name) == 0) {
            free(current_dir->files[i]);
            for (int j = i; j < current_dir->num_files - 1; j++) {
                current_dir->files[j] = current_dir->files[j + 1];
            }
            current_dir->num_files--;
            printf("File '%s' removed.\n", file_name);
            return;
        }
    }
    printf("File '%s' not found.\n", file_name);
}

// Display help for available commands
void helpCommand() {
    printf("Available commands:\n");
    printf("1. ls               - List files and directories in the current directory\n");
    printf("2. mkdir <dir>      - Create a new directory\n");
    printf("3. cd <dir>         - Change to the specified directory\n");
    printf("4. createfile <file> - Create a new empty file\n");
    printf("5. cat <file>       - Display the content of a file\n");
    printf("6. echo <text> <file> - Append text to a file\n");
    printf("7. remove <file>    - Remove a file\n");
    printf("8. help             - Show this help message\n");
    printf("9. exit             - Exit the terminal simulation\n");
}

// Main command loop
void commandLoop() {
    char command[100];
    char arg1[MAX_NAME_LENGTH], arg2[MAX_NAME_LENGTH];
    int running = 1;

    while (running) {
        printf("\n%s$ ", current_dir->name);  // Show current directory name
        fgets(command, sizeof(command), stdin);
        command[strcspn(command, "\n")] = 0;  // Remove trailing newline

        // Match and execute the commands
        if (strcmp(command, "ls") == 0) {
            lsCommand();
        } else if (sscanf(command, "mkdir %s", arg1) == 1) {
            mkdirCommand(arg1);
        } else if (sscanf(command, "cd %s", arg1) == 1) {
            cdCommand(arg1);
        } else if (sscanf(command, "createfile %s", arg1) == 1) {
            createfileCommand(arg1);
        } else if (sscanf(command, "cat %s", arg1) == 1) {
            catCommand(arg1);
        } else if (sscanf(command, "echo %s %s", arg1, arg2) == 2) {
            echoCommand(arg1, arg2);
        } else if (sscanf(command, "remove %s", arg1) == 1) {
            removeFileCommand(arg1);
        } else if (strcmp(command, "help") == 0) {
            helpCommand();
        } else if (strcmp(command, "exit") == 0) {
            running = 0;
        } else {
            printf("Invalid command. Type 'help' for a list of commands.\n");
        }
    }
}

// Main function
int main() {
    initFileSystem();
    printf("Welcome to CLIMaster terminal!\n");
    commandLoop();
    printf("Exiting CLIMaster terminal...\n");
    return 0;
}
