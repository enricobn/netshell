/*
 * Terminal1.cpp
 *
 *  Created on: Apr 19, 2014
 *      Author: enrico
 */

#include "Terminal.h"

#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stddef.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

#include "ITerminalListener.h"
#include "TerminalSettings.h"

#include <sys/ioctl.h>
#include <stdarg.h>

#include <iostream>
#include <thread>
#include <string.h>
#include <signal.h>
#include <wait.h>
#include <map>

static std::map<pthread_t, bool> thread_exit = std::map<pthread_t, bool>();

static void thread_signal_handler(int sig) {
//	pid_t pid = getpid();
//	std::cout << "handling signal " << sig << " pid=" << pid << std::endl;
	// since I could, in the future, use the same handler for other signals
	if (sig == SIGTERM) {
		pthread_t th = pthread_self();
		std::cout << "marked thread " << th << " to exit" << std::endl;
		thread_exit[th] = true;
	}
}

//static void father_signal_handler(int sig) {
//	pid_t pid = getpid();
//	std::cout << "handling father signal " << sig << " pid=" << pid << std::endl;
//	if (sig == SIGCHLD) {
//		int stat = 0;
//		while(pid_t wait_pid = waitpid(-1, &stat, WNOHANG) > 0) {
//			if (WIFSTOPPED(stat)) {
//				std::cout << "killing child process " << wait_pid << std::endl;
//				if (kill(wait_pid, SIGKILL) == 0) {
//					std::cout << "killed child process " << wait_pid << std::endl;
//				}
//			}
//		}
//	}
//}

// from http://rachid.koucha.free.fr/tech_corner/pty_pdip.html#API_of_the_pseudo-terminals
Terminal::Terminal(ITerminalListener* terminal_listener, const char *file, size_t n_args, ...) :
fdm(-1),
fds(-1),
terminal_listener(terminal_listener),
th(NULL),
child_process(-1)
{
	this->file = file;
	va_list vl;
	va_start(vl, n_args);
	char** tmp = new char*[n_args + 1];
    for(size_t i = 0; i < n_args; i++) {
		char *a = va_arg(vl, char*);
		tmp[i] = a;
	}
    tmp[n_args] = NULL;
	va_end(vl);
	this->arg = (const char**)tmp;
}

void Terminal::write(const void *buffer, size_t size) {
	::write(fdm, buffer, size);
}

void* Terminal::thread_wrapper(void* terminal) {
	((Terminal*)terminal)->run();
	return NULL;
}

void Terminal::run() {
	char input[150];
	int rc;
	fd_set fd_in;
	sigset_t mask;
	sigset_t orig_mask;

#ifdef TERMINAL_SETTINGS
	TerminalSettings settings = TerminalSettings(fdm, true);
#endif
	/**
	 * I'm not sure of this signal handling code, but for now it works.
	 *
	 * The issue was to handle correctly the SIGTERM signal while in the select, since there can be a signal race.
	 *
	 * The idea comes from http://www.linuxprogrammingblog.com/code-examples/using-pselect-to-avoid-a-signal-race
	 *
	 * To do this, I block the SIGTERM signal for this thread (pthread_sigmask), so it's not handled directly,
	 * but only in the pselect (using orig_mask which has it).
	 * Then I register a signal handler for SIGTERM for the thread (which is fired only during the pselect),
	 * then in the handler I flag the thread as terminated even if ,I think, it could not happen to reach the while check
	 * since the pselect exits first with an EINTR errno.
	 */
	struct sigaction act;

	memset (&act, 0, sizeof(act));
	act.sa_handler = thread_signal_handler;

	/* I catch SIGTERM to mark it as terminated in thread_exit map */
	if (sigaction(SIGTERM, &act, 0)) {
		perror ("sigaction");
		throw std::exception();
	}

	sigemptyset (&mask);
	sigaddset (&mask, SIGTERM);

	if (pthread_sigmask(SIG_BLOCK, &mask, &orig_mask) < 0) {
		perror ("sigprocmask");
		throw std::exception();
	}

	thread_exit[*th] = false;

	// TODO I don't know if it's necessary (look at the comment above)
	while (!thread_exit[*th])
	{
	    // Wait for data from standard input and master side of PTY
	    FD_ZERO(&fd_in);
//	    FD_SET(0, &fd_in);
	    FD_SET(fdm, &fd_in);

	    rc = pselect(fdm + 1, &fd_in, NULL, NULL, NULL, &orig_mask);
	    if (rc < 0)
	    {
	    	if (errno != EINTR) { // Interrupted system call
	    		fprintf(stderr, "Error %d on select()\n", errno);
	    	}
	    	break;
	    }
	    else
	    {
	        // If data on master side of PTY
	        if (FD_ISSET(fdm, &fd_in))
	        {
	          rc = read(fdm, input, sizeof(input));
	          if (rc > 0)
	          {
#ifdef TERMINAL_SETTINGS
	        	  TerminalSettings new_settings = TerminalSettings(fdm, false);

	        	  if (settings.icanon != new_settings.icanon) {
	        		  std::cout << "ICANON " << new_settings.icanon  << std::endl;
	        	  }

	        	  if (settings.icrnl != new_settings.icrnl) {
	        		  terminal_listener->on_attr_change(new_settings.icrnl);
	        		  std::cout << "ICRNL " << new_settings.icrnl << std::endl;
	        	  }

	        	  if (settings.igncr != new_settings.igncr) {
	        		  std::cout << "IGNCR " << new_settings.igncr << std::endl;
	        	  }

	        	  settings = new_settings;
#endif
//        		  std::cout << std::string(input, rc) << std::endl;


//	        		 struct winsize w = winsize();
//	        		 ioctl(fdm, TIOCGWINSZ, &w);
//
//	        		 printf ("lines %d\n", w.ws_row);
//	        		 printf ("columns %d\n", w.ws_col);

//	        		 w.ws_col = 100;
//	        		 w.ws_row = 24;
//	        		 ioctl(fdm, TIOCSWINSZ, &w);

	        	  // listener has been cleared so I exit thread
	        	  if (terminal_listener == NULL) {
	        		  break;
	        	  }
	        	  // Send data to listener
	        	  terminal_listener->on_terminal_output(input, rc);
	          }
	          else
	          {
	            if (rc < 0)
	            {
//		            if ( (errno == EINTR) || (errno == EAGAIN) )
//		            {
//		              errno = 0;
//		              break;
//		            }
	            	// the child process has been closed or killed (for example in the destructor)
	              fprintf(stderr, "Error %d on read master PTY\n", errno);
	              break;
	            }
	          }
	        }
	    } // End switch
	} // End while
	pthread_exit(NULL);
	thread_exit.erase(*th);
}

void Terminal::start() {
	int rc;

	fdm = posix_openpt(O_RDWR);

	if (fdm < 0)
	{
		fprintf(stderr, "Error %d on posix_openpt()\n", errno);
		throw std::exception();
	}

	rc = grantpt(fdm);
	if (rc != 0)
	{
		fprintf(stderr, "Error %d on grantpt()\n", errno);
		throw std::exception();
	}

	rc = unlockpt(fdm);
	if (rc != 0)
	{
		fprintf(stderr, "Error %d on unlockpt()\n", errno);
		throw std::exception();
	}

	 // I set the size to 80 x 24
	 struct winsize w = winsize();
	 ioctl(fdm, TIOCGWINSZ, &w);
	 w.ws_col = 120;
	 w.ws_row = 40;
	 ioctl(fdm, TIOCSWINSZ, &w);

	 // Open the slave side ot the PTY
	 fds = open(ptsname(fdm), O_RDWR);

	// Create the child process
	child_process = fork();

	if (child_process != 0)
	{
		// FATHER

		// I'm not going to get result from the child process, so it can be completely killed,
		// otherwise is set as defunct
		signal(SIGCHLD, SIG_IGN);

		// Close the slave side of the PTY
		close(fds);

		th = (pthread_t*)malloc(sizeof th);

		int res = pthread_create(th, NULL, Terminal::thread_wrapper, this);
		if (res)
		{
			fprintf(stderr,"Error - pthread_create() return code: %d\n", res);
			throw std::exception();
		}
	}
	else
	{
	  // CHILD

	  // Close the master side of the PTY
	  close(fdm);

	  struct termios slave_orig_term_settings; // Saved terminal settings
	  struct termios new_term_settings; // Current terminal settings

	  // Save the defaults parameters of the slave side of the PTY
	  rc = tcgetattr(fds, &slave_orig_term_settings);

	  // Set RAW mode on slave side of PTY
	  new_term_settings = slave_orig_term_settings;
//	  cfmakeraw (&new_term_settings);
//	  tcsetattr (fds, TCSANOW, &new_term_settings);

	  // The slave side of the PTY becomes the standard input and outputs of the child process
	  close(0); // Close standard input (current terminal)
	  close(1); // Close standard output (current terminal)
	  close(2); // Close standard error (current terminal)

	  dup(fds); // PTY becomes standard input (0)
	  dup(fds); // PTY becomes standard output (1)
	  dup(fds); // PTY becomes standard error (2)

	  // Now the original file descriptor is useless
	  close(fds);

	  // Make the current process a new session leader
	  setsid();

	  // As the child is a session leader, set the controlling terminal to be the slave side of the PTY
	  // (Mandatory for programs like the shell to make them manage correctly their outputs)
	  ioctl(0, TIOCSCTTY, 1);

	  // Execution of the program
	  {
		  // TODO handle failure
		rc = execvp(file, (char* const*)arg);
		if (rc == -1) {
			std::string message = "error executing " + std::string(file);
			perror(message.c_str());
			throw std::exception();
		}
	  }
	}

//	return 0;
}

// TODO use a vector?
void Terminal::remove_listener() {
	terminal_listener = NULL;
}

Terminal::~Terminal() {
	// I'm the father
	if (child_process > 0) {
		std::cout << "Terminal destructor: father." << std::endl;

		if (th != NULL) {
			std::cout << "killing Terminal thread " << *th << std::endl;
			if (pthread_kill(*th, SIGTERM) == 0) {
				std::cout << "killed Terminal thread " << *th << std::endl;
			} else {
				perror("Error killing Terminal thread.");
			}
//			thread_exit.erase(*th);
//			th = NULL;
		}
		std::cout << "killing Terminal child process " << child_process << std::endl;
		if (kill(child_process, SIGTERM) == 0) {
			std::cout << "killed Terminal child process " << child_process << std::endl;
		}
	} else {
		std::cout << "Terminal destructor: child." << std::endl;
	}
}

