#include "headers.h"

char** home_path;
int home_path_length;
char** current_path;
int current_path_length;

int base_path_holds();
void move_to(char*);
void move_back();
void go_home();
char* get_path();
char* get_home();
char* processed_path(char*);
void print_complete_path();

char* old_path = NULL;

void initialize_path() {
    home_path = malloc(128 * sizeof(char*));
    current_path = malloc(128 * sizeof(char*));

    char* path = getcwd(NULL, 0);
    char* token = strtok(path, "/");
    while (token != NULL) {
        home_path[home_path_length++] = token;
        current_path[current_path_length++] = token;
        
        token = strtok(NULL, "/");
    }
    chdir(get_path());
}
void print_path() {
    if (current_path_length < home_path_length) {
        for (int i=0;i<current_path_length;i++) {
            printf("/%s", current_path[i]);
        }
    }
    else {
        if (base_path_holds()) {
            printf("~");
            if (current_path_length > home_path_length) {
                for (int i=home_path_length;i<current_path_length;i++) {
                    printf("/%s", current_path[i]);
                }
            }
        }
        else {
            for (int i=0;i<current_path_length;i++) {
               printf("/%s", current_path[i]);
            }    
        }
    }
    if (current_path_length == 0) {
        printf("/");
    }
}

int base_path_holds() {
    if (current_path_length < home_path_length) {
        return 0;
    }
    for (int i=0;i<home_path_length;i++) {
        if (strcmp(current_path[i], home_path[i])){
            return 0;
        }
    }
    return 1;
}

void move_to(char* path) {
    // Path is absolute
    char* final_path;
    if (path[0] == '/'){
        final_path = path;
    }

    // Path is relative
    else {
        final_path = processed_path(path);
    }

    struct stat info;
    int response = stat(final_path, &info);
    if (!response && S_ISDIR(info.st_mode)) {
        old_path = get_path();
        chdir(final_path);
        current_path_length = 0;
        char* token = strtok(final_path, "/");
        while (token != NULL) {
            current_path[current_path_length++] = token;
            token = strtok(NULL, "/");
        }
    }
    else {
        printf("Directory not found\n");
    }
}

void move_back() {
    if (current_path_length == 0) {
        printf("Cannot go any deeper\n");
    }
    else {
        char* final_path = get_path();
        old_path = get_path();
        strrchr(final_path, '/')[0] = '\0';
        chdir(final_path);
        current_path_length--;
    }
}

void go_home() {
    old_path = get_path();
    for (int i=0;i<home_path_length;i++) {
        current_path[i] = home_path[i];
    }
    current_path_length = home_path_length;
    chdir(get_home());
}

void print_complete_path() {
    for (int i=0;i<current_path_length;i++) {
        printf("/%s", current_path[i]);
    }
    if (current_path_length == 0){
        printf("/");
    }
}

char* get_path() {
    int total_length = 0;
    for (int i=0;i<current_path_length;i++) {
        total_length += strlen(current_path[i]);
    }
    char* path = malloc((total_length + current_path_length + 1) * sizeof(char));
    path[0] = '\0';
    for (int i=0;i<current_path_length;i++) {
        strcat(path, "/");
        strcat(path, current_path[i]);    
    }
    if (strlen(path) == 0) {
        return "/";
    }
    return path;
}

char* get_home() {
    int total_length = 0;
    for (int i=0;i<home_path_length;i++) {
        total_length += strlen(home_path[i]);
    }
    char* path = malloc((total_length + home_path_length + 1) * sizeof(char));
    path[0] = '\0';
    for (int i=0;i<home_path_length;i++) {
        strcat(path, "/");
        strcat(path, home_path[i]);    
    }
    if (strlen(path) == 0) {
        return "/";
    }
    return path;
}

// Returns path with flags ., .. and ~ replaced with there respective values
char* processed_path(char* path) {
    char* processed = malloc(256 * sizeof(char));
    processed[0] = '\0';
    char* token = strtok(path, "/");
        if (!strcmp(token, ".")) {
            strcat(processed, get_path());
        }
        else if(!strcmp(token, "..")) {
            char* current_path = get_path();
            strrchr(current_path, '/')[0] = '\0';
            strcat(processed, current_path);
        }
        else if (!strcmp(token, "~")) {
            strcat(processed, get_home());
        }
        else {
            strcat(processed, get_path());
            strcat(processed, "/");
            strcat(processed, token);
        }
        token = strtok(NULL, "/");
        while (token != NULL) {
            strcat(processed, "/");
            strcat(processed, token);
            token = strtok(NULL, "/");
        }
    return processed;   
}

void past_pwd() {
    if (old_path == NULL) {
        print_complete_path();
        printf("\n");
    }
    else {
        printf("%s\n", old_path);
        move_to(old_path);
    }
}