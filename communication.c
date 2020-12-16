# include <stdio.h>
# include <time.h>
# include <unistd.h>
# include <errno.h>
# include <ctype.h>
# include <unistd.h>
# include <stdlib.h>
# include <sys/stat.h>
# include <sys/wait.h>
# include <string.h>
# include <fcntl.h>
# include <sys/ipc.h>
# include <sys/shm.h>

# define BUFFER_SIZE 25
# define READ_END 0
# define WRITE_END 1

int main(int argc, char** argv)
{

   pid_t pid_child;
   pid_t wpid;
   pid_t pid;
   int i;
   char write_msg[BUFFER_SIZE];
   char read_msg[BUFFER_SIZE];
   int fd[2];

   // init file pointer
   FILE *fptr;

   // init filename space, ask user for file
   char filename[50];
   printf("Enter the full file path file location: ");
   printf("\n\n");
   scanf("%s", &filename);
   fptr = fopen(filename, "w");
   
   // create child process
   pid_child = fork();

   switch(pid_child)
   {
    case -1:
     // the fork failed
     printf("The fork failed!\n");
     break;

    case 0:
     // child process, write all files with cs360* into filelist.txt
     printf("Child Pid: %d", getpid());
     printf("\n\n");
     system("ls -1 /home/stu/blake.culbert/c_programs/cs360* > filelist.txt");
     break;

    default:
     // parent process, wait for child to finish
     wpid = wait(0);
     printf("\nParent: Wait completed for child %d\n", wpid);

     // Display files in list
     system("more filelist.txt");
     printf("\n\n");

     // obtain user given search string
     printf("Enter the search string: ");
     printf("\n\n");
     scanf("%s", &write_msg);

     // create the pipe
     if (pipe(fd) == -1) {
       fprintf(stderr, "Pipe Failed");
     }

     // create another child for pipe communication
     pid = fork();

     // parent process writes search string to write end of pipe
     if (pid > 0) {
       close(fd[READ_END]);
       write(fd[WRITE_END], write_msg, strlen(write_msg)+1);
       close(fd[WRITE_END]);
     }

     // child process
     else {

       // receive search string from parent, close read end of pipe
       close(fd[WRITE_END]);
       read(fd[READ_END], read_msg, BUFFER_SIZE);
       // display to user given search string
       printf("read %s\n\n", read_msg);
       close(fd[READ_END]);

       int len = 255;
       char line[len];
       int length;
       char str_to_execute[80] = {0};
       char search_str[20] = {0};

       // close last file in file pointer
       fclose(fptr);

       // open new file under read only mode
       fptr = fopen(filename, "r");

       // put piped message into search_str
       strcat(search_str, read_msg);
       
       // while a file is available to grep
       while (fgets(line, len, fptr)) {

         // remove new line character at the end of each line in file
         length = strlen(line);
         if( line[length-1] == '\n')
             line[length-1] = 0;

         // start creation of our system grep command
         strcat(str_to_execute, "grep ");
         strcat(str_to_execute, search_str);
         strcat(str_to_execute, " ");
         strcat(str_to_execute, line);
         strcat(str_to_execute, " | wc\n");
         system(str_to_execute);

         // empty str_to_execute for next loop
         str_to_execute[0] = 0;
         printf("\n\n");
       }

       printf("\n\n");
     }

     break;

    }
    
    return(0);
}



