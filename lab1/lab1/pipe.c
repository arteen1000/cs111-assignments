/* Copyright 2023 -- Arteen Abrishami */

#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <assert.h>

#include <sys/types.h>
#include <sys/wait.h>

int handle_close_ret(int);
int handle_dup_ret(int, int);

int main(int argc, char *argv[])
{
  
  if (argc == 1)
    return EINVAL;
  else if (argc == 2) {
     execvp( argv[1], (char *[]) { argv[1], NULL } );
     int err = errno;
     perror("execvp");
     return err;
  }     

  int fds[argc - 2][2];
  pid_t rc;
  
  int status = 0;  

  for (int i = 0; i < argc - 1; i++) {
    
    rc = pipe(fds[i]);
    
    if (rc == -1) {
      
      status = errno;
      perror("pipe");
      
      return status;
      
    }

    rc = fork();

    if (rc == -1) {
      
      status = errno;
      perror("fork");
      
      return status;
      
    }

    else if (rc == 0) {

      /* all but last child take put stdout to current write-end */
      
      if ( ! (i == argc-2) ) {
	
      rc = close(STDOUT_FILENO);
      if ( (status = handle_close_ret(rc)) ) return status;
      
      rc = dup(fds[i][1]);
      if ( (status = handle_dup_ret(rc, STDOUT_FILENO)) ) return status;
      
      rc = close(fds[i][1]);
      if ( (status = handle_close_ret(rc)) ) return status;
      
      }

      /* all but first child take stdin from prev read-end */
      
      if ( i ) {
	rc = close(STDIN_FILENO);
	if ( (status = handle_close_ret(rc)) ) return status;
      
	rc = dup(fds[i-1][0]);
	if ( (status = handle_dup_ret(rc, STDIN_FILENO)) ) return status;
	
	rc = close(fds[i-1][0]);
	if ( (status = handle_close_ret(rc)) ) return status;
      }

      rc = execvp(argv[i+1], (char *[]) { argv[i+1], NULL });
      if (rc == -1) {
	status = errno;
	perror("execvp");
	return status;
      }
      
    }

    else if (rc > 0) {
      
      pid_t child_pid = rc;
      int wstatus;
      
      rc = close(fds[i][1]);
      if ( (status = handle_close_ret(rc)) ) return status;
      

      rc = waitpid(child_pid, &wstatus, 0);
      if (rc == -1) {
	status = errno;
	perror("waitpid");
	return status;
      }

      /* close read ends to avoid pipe() ret EMFILE */

      if (i > 0) {
	  rc = close(fds[i-1][0]);
	  if ( (status = handle_close_ret(rc)) ) return status;
      }
	  

      if ( WIFEXITED(wstatus) ) {
	if ( WEXITSTATUS(wstatus) ) return WEXITSTATUS(wstatus);
      } else {
	return ECHILD;
      }
      
    }

  }

  return status;
  
}

int handle_close_ret(int rc) {

  if (rc == -1) {
    int err = errno;
    perror("close");
    return err;
  }

  return 0;
}

int handle_dup_ret(int rc, int fd) {
  
  if (rc == -1) {
    int err = errno;
    perror("dup");
    return err;
  }

  assert(rc == fd);
  return 0;
}
