#ifndef CGIPROCESS_HPP
#define CGIPROCESS_HPP

#include <string>
#include <ctime>
#include <sys/types.h>

/**
 * @brief Represents an active CGI process for non-blocking execution
 *
 * This structure tracks the state of a CGI script running asynchronously.
 * The pipes are registered in the main poll() loop.
 */
struct CgiProcess {
	enum State {
		CGI_WRITING_BODY,   // Writing request body to CGI stdin
		CGI_READING_OUTPUT, // Reading CGI output from stdout
		CGI_DONE,           // CGI finished, ready to send response
		CGI_ERROR           // CGI failed
	};

	pid_t pid;              // Child process ID
	int pipe_in;            // Write end of stdin pipe (to send body to CGI)
	int pipe_out;           // Read end of stdout pipe (to read CGI output)
	int client_fd;          // Client socket waiting for this CGI response

	std::string body;       // Request body to send to CGI (POST data)
	size_t body_written;    // Bytes already written to CGI stdin

	std::string output;     // Accumulated CGI output

	time_t start_time;      // When CGI started (for timeout)
	int timeout;            // Timeout in seconds

	State state;            // Current state

	CgiProcess()
		: pid(-1)
		, pipe_in(-1)
		, pipe_out(-1)
		, client_fd(-1)
		, body()
		, body_written(0)
		, output()
		, start_time(0)
		, timeout(30)
		, state(CGI_WRITING_BODY)
	{}

	bool hasBodyToWrite() const {
		return body_written < body.size();
	}

	bool isTimedOut() const {
		return (time(NULL) - start_time) > timeout;
	}
};

#endif
