/*
 * Terminal.cpp
 *
 *  Created on: Apr 19, 2014
 *      Author: enrico
 */

#include "Terminal2.h"

#include <sys/select.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

#include <string>
#include <iostream>

enum PIPE_FILE_DESCRIPTERS
{
  READ_FD  = 0,
  WRITE_FD = 1
};

enum CONSTANTS
{
  BUFFER_SIZE = 100
};

Terminal2::Terminal2() {
	// TODO Auto-generated constructor stub

}

void Terminal2::start() {
	int       parentToChild[2];
	  int       childToParent[2];
	  pid_t     pid;
	  std::string    dataReadFromChild;
	  char      buffer[ BUFFER_SIZE + 1 ];
	  ssize_t   readResult;
//	  int       status;

	  if (pipe(parentToChild) != 0) {
		  std::cerr << "Error in parentToChild" << std::endl;
		  throw std::exception();
	  }

	  if (pipe(childToParent) != 0) {
		  std::cerr << "Error in childToParent" << std::endl;
		  throw std::exception();
	  }

	  switch ( pid = fork() )
	  {
	    case -1:
		  std::cerr << "Error in fork" << std::endl;
		  throw std::exception();

	    case 0: /* Child */
	      if(dup2( parentToChild[ READ_FD  ], STDIN_FILENO  ) == -1) {
			  std::cerr << "Error " << std::endl;
			  throw std::exception();
	      }

	      if (dup2( childToParent[ WRITE_FD ], STDOUT_FILENO ) == -1) {
			  std::cerr << "Error " << std::endl;
			  throw std::exception();
	      }

	      if (dup2( childToParent[ WRITE_FD ], STDERR_FILENO ) == -1 ) {
			  std::cerr << "Error " << std::endl;
			  throw std::exception();
	      }

	      if (close( parentToChild [ WRITE_FD ] ) != 0) {
			  std::cerr << "Error " << std::endl;
			  throw std::exception();
	      }

	      if ( close( childToParent [ READ_FD  ] ) != 0) {
			  std::cerr << "Error " << std::endl;
			  throw std::exception();
	      }

	      /*   file,  arg0,  arg1,   arg2 */
	      execlp(  "bc",  "bc");

//	      break;
	      std::cerr << "This line should never be reached!!!" << std::endl;
	      throw std::exception();


	    default: /* Parent */
	      std::cout << "Child " << pid << " process running..." << std::endl;

	      if(  close( parentToChild [ READ_FD  ] ) != 0) {
			  std::cerr << "Error " << std::endl;
			  throw std::exception();
	      }

	      if( close( childToParent [ WRITE_FD ] ) != 0) {
			  std::cerr << "Error " << std::endl;
			  throw std::exception();
	      }

	      fd_set readfds;

	      FD_ZERO(&readfds);
	      FD_SET( childToParent[ READ_FD ], &readfds );

	      while ( true )
	      {
	    	  switch ( select ( 1 + childToParent[READ_FD], &readfds, NULL, NULL, NULL ) )
//
//	        switch ( readResult = read( childToParent[ READ_FD ],
//	                                    buffer, BUFFER_SIZE ) )
	        {
	          case 0: /* End-of-File, or non-blocking read. */
	        	  std::cerr << "Timeout expired" << std::endl;
	        	  throw std::exception();
//	            std::cout << "End of file reached..."         << std::endl
//	                 << "Data received was ("
//	                 << dataReadFromChild.size() << "):" << std::endl
//	                 << dataReadFromChild                << std::endl;
//
//	            if ( waitpid( pid, & status, 0 ) != pid ) {
//	  			  std::cerr << "Error " << std::endl;
//	  			  throw std::exception();
//	            }
//
//	            std::cout << std::endl
//	                 << "Child exit staus is:  " << WEXITSTATUS(status) << std::endl
//	                 << std::endl;
//
//	            exit(0);


	          case -1:
	            if ( (errno == EINTR) || (errno == EAGAIN) )
	            {
	              errno = 0;
	              break;
	            }
	            else
	            {
	              std::cerr << "select() failed" << std::endl;
	              throw std::exception();
	            }

	          default:
//	            dataReadFromChild.append( buffer, readResult );
	        	  readResult = read( childToParent[ READ_FD ], buffer, BUFFER_SIZE );
	        	  std::cout << std::string(buffer, readResult);
	            break;
	        }
	      } /* while ( true ) */
	  } /* switch ( pid = fork() )*/
}

Terminal2::~Terminal2() {
	// TODO Auto-generated destructor stub
}

