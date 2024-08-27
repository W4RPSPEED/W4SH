#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <linux/limits.h>
#define TOKEN_BUFSIZE 64
#define TOKEN_DELIM " \t\r\n\a"
#define VERSION 1.0.0-alpha


/*
  Beautiful Built-Ins
*/



int tabsh_cd(char **args);
int tabsh_help(char **args);
int tabsh_exit(char **args);
int tabsh_env( char **args);
char *builtin_str[] = {
  "cd",
  "help",
  "exit",
  "env"
};

int (*builtin_func[]) (char **) = {
  &tabsh_cd,
  &tabsh_help,
  &tabsh_exit,
  &tabsh_env
};

int tabsh_num_builtins() {
  return sizeof(builtin_str) / sizeof(char *);
}

/*
  Builtin function implementations.
*/
int tabsh_cd(char **args)
{
  if (args[1] == NULL) {
    fprintf(stderr, "!! Built-In Error !!: expected argument to \"cd\"\n");
  } else {
    if (chdir(args[1]) != 0) {
      perror("tabsh");
    }
  }
  return 1;
}
// Environment variable management
int tabsh_env(char **args) {
  
  if (args[1] == NULL) {
    fprintf(stderr, "!! Built-In Error !!: expected argument to \"env\"\n");
    return 1;
    
  } else if (strcmp("set", args[1]) == 0) {
      if (args[2] == NULL) {
        fprintf(stderr, "!! Built-In Error !!: expected argument to \"set\"\n");
        return 1;
        
      } else {
        if (args[3] == NULL) {
          fprintf(stderr, "!! Built-In Error !!: you cannot assign an empty value; please try \"env delete [NAME]\" instead.\n");
          return 1;
        } else {
          if (setenv(args[2],args[3],1) !=0) {
            fprintf(stderr, "!! Built-In Error !!: Failed to set env variable.");
            return 1;
          } else {
            printf("Successfully set env variable %s to %s.\n", args[2],args[3]);
            return 1;
          }
        }
          
      }
  } else if (strcmp("del", args[1]) == 0) {
    if (args[2] == NULL ) {
      fprintf(stderr, "!! Built-In Error !!: Cannot delete non-existent variable.\n");
      return 1;
      
    } else {
      unsetenv(args[2]);
      return 1;
      
    }
  } else if (strcmp("print", args[1]) == 0) {
    if (args[2] == NULL) {
      fprintf(stderr, "!! Built-In Error !!: Cannot print non-existent variable.\n");
      return 1;
    } else {
      char* env;
      env = getenv(args[2]);
      
      
      if (env == NULL) {
        fprintf(stderr, "!! Built-In Error !!: Cannot print non-existent variable.\n");
        return 1;
      } else {
        env = strdup(env);
        printf("%s\n", env);
        free(env);
        return 1;
      }
    }
    /* else statement to contain any nonexistent commands, and to handle if a nonexistent command is sent. */
  } else {
    fprintf(stderr, "!! Built-In Error !!: Invalid command.\n");
    return 1;
    
  }
 return 0;         
}
    
  


int tabsh_help(char **args)
{
  int i;
  printf("tabsh, developed by TabbySlimeKing.\n");
  printf("Somewhat based on Stephan Brennan's lsh!\n");
  printf("Type program names and arguments, and hit enter.\n");
  printf("The following are built in:\n");

  for (i = 0; i < tabsh_num_builtins(); i++) {
    printf("  %s\n", builtin_str[i]);
  }

  printf("Use the man command for information on other programs.\n");
  return 1;
}

int tabsh_exit(char **args)
{
  return 0;
}




char *tabsh_rl() {
  char *string = NULL;
  size_t size = 0;
  ssize_t chars_read;


  //Getting some LONG strings safely to prevent buffer overflowing.
  
  if (getline(&string, &size, stdin) == -1){
    if (feof(stdin)) {
      
      exit(EXIT_SUCCESS);
    } else {
      perror("!! Readline Error !!");
      exit(EXIT_FAILURE);
    }
  }

  return string;
  
}



char **tabsh_split_line(char *line) {
  int size = TOKEN_BUFSIZE, position = 0;
  char **tokens = malloc(size * sizeof(char*));
  char *token;

  if (!tokens) {
    fprintf(stderr, "!! Allocation Error !!\n");
    exit(EXIT_FAILURE);
  }

  token = strtok(line, TOKEN_DELIM);
  while (token != NULL) {
    tokens[position] = token;
    position++;


    if (position >= size) {
      size += TOKEN_BUFSIZE;
      tokens = realloc(tokens, size * sizeof(char*));
      if (!tokens) {
        fprintf(stderr, "!! Allocation Error !!\n");
        exit(EXIT_FAILURE);
      }
    }

    token = strtok(NULL, TOKEN_DELIM);
  }

  tokens[position] = NULL;
  return tokens;
}

int tabsh_launch(char **args) {
  pid_t pid, wpid;
  int status;
  


  pid = fork();
  if (pid == 0) {
  //this is the kiddo
    if (execvp(args[0], args ) == -1) {
      perror("!! Run Error !!");
    }
    exit(EXIT_FAILURE);
  } else if (pid < 0) {
    //fork err
    perror("!! Run Error !!");
  } else {
    // parent
    do {
      wpid = waitpid(pid, &status, WUNTRACED);
    
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    
  }
  return 1;
}
int tabsh_execute(char **args)
{
  int i;

  if (args[0] == NULL) {
    // An empty command was entered.
    return 1;
  }

  for (i = 0; i < tabsh_num_builtins(); i++) {
    if (strcmp(args[0], builtin_str[i]) == 0) {
      return (*builtin_func[i])(args);
    }
  }

  return tabsh_launch(args);
}

void tabsh_loop(void)
{
  
  char *line;
  char **args;
  int status;
  ssize_t size= 0;

    

  do {
    char* hn;
    char* un;
    hn = getenv("HOSTNAME");
    un = getenv("USER");
    strdup(un);
    strdup(hn);
    
    
    
    char cwd[PATH_MAX];
    getcwd(cwd, sizeof(cwd));
    printf("[%s@%s]: %s >", un, hn, cwd);
    line = tabsh_rl();
    args = tabsh_split_line(line);
    status = tabsh_execute(args);

    free(line);
    free(args);
    free(un);
    free(hn);
  } while (status);
}
int main() {
  printf("warning: this is early development software!\nPrepare for bugs, missing features, and other nasty things.\n \033[1m \033[31m          YOU HAVE BEEN WARNED! \033[37m \033[0m \n");
  tabsh_loop();
  return 0;
}