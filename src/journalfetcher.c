#include <asm-generic/errno-base.h>
#include <bits/types/sigset_t.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

#define MAX_CMD_LEN 200

int status;
const char* journalctlErrCall = "journalctl -r --grep=err";
const char* journalctlWarCall = "journalctl -r --grep=err";

int system(const char *command)
{
  sigset_t blockMask, origMask;
  struct sigaction saIgnore, saOrigQuit, saOrigInt, saDefault;
  int status, savedErrno;
  pid_t childPid;

  if (command == NULL) // Checks if shell is available
  {
    return system(":") == 0;
  }
  sigemptyset(&blockMask); // Block SIGCHLD
  sigaddset(&blockMask, SIGCHLD);
  sigprocmask(SIG_BLOCK, &blockMask, &origMask);
  
  saIgnore.sa_handler = SIG_IGN; // Ignore SIGINT and SIGQUIT 
  saIgnore.sa_flags = 0;
  sigemptyset(&saIgnore.sa_mask);
  sigaction(SIGINT, &saIgnore, &saOrigInt);
  sigaction(SIGQUIT, &saIgnore, &saOrigQuit);

  switch (childPid = fork()) 
  {
    case -1: // fork() failed
      status = -1;
      break; // Carry on to reset signal attributes
    
    case 0:
      saDefault.sa_handler = SIG_DFL;
      saDefault.sa_flags = 0;
      sigemptyset(&saDefault.sa_mask);

      if (saOrigInt.sa_handler != SIG_IGN) 
      {
        sigaction(SIGINT, &saDefault, NULL);
      }
      if (saOrigQuit.sa_handler != SIG_IGN)
      {
        sigaction(SIGQUIT, &saDefault, NULL);
      }

      sigprocmask(SIG_SETMASK, &origMask, NULL);

      //execl("/bin/sh", "sh", "-c", command, (char *) NULL);
      FILE* journalErrors = popen(command, "r");
      if (!journalErrors)
      {
        perror("popen() failed.");
        exit(1);
      }
      char output[100];
      fgets(output, sizeof(output), journalErrors);
      printf("Lines = %s", output);

      pclose(journalErrors);
      _exit(127);

    default: // Parent: Wait for child to terminate
      while (waitpid(childPid, &status, 0) == -1) 
      {
        if (errno != EINTR)
        {
          status = -1;
          break;
        }
      }
    break;
  }
  // Unblock SIGCHLD, restore dispositions of SIGINT and SIGQUIT
  errno = savedErrno;
  
  sigprocmask(SIG_SETMASK, &origMask, NULL);
  sigaction(SIGINT, &saOrigInt, NULL);
  sigaction(SIGQUIT, &saOrigQuit, NULL);

  return status;
}

int main(int argc, char *argv[])
{
  system(journalctlErrCall);
  return EXIT_SUCCESS;
}

