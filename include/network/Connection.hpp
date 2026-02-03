#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <string>
#include <stdexcept>
#include <sys/types.h>
#include <ctime>

#define MAX_BODY_SIZE

class Connection {
	public:
//		CONSTRUCTORS & DESTRUCTOR

		explicit Connection(int fd);
		~Connection();


//		PUBLIC ATTRIBUTES

		int 				fd;
		static const size_t	maxRequestSize = 5 * 1024 * 1024;
		size_t				totalBytesReceived;
		std::string			recv_buffer;
		std::string			send_buffer;
		size_t				bytes_sent;
		time_t				last_activity;	// Timestamp de dernière activité (pour timeout)
		bool				should_close;	// Fermer la connexion apres envoi (Connection: close)


//		MEMBER FUCTIONS

		ssize_t read_available();
		ssize_t write_pending();
		bool has_pending_data() const;
		void update_activity();


//		EXCEPTION CLASS

		class ConnectionException : public std::runtime_error {
		public:
			explicit ConnectionException(const std::string& message);
		};


	private:

		void set_nonblocking();

};

#endif

//		a supprimer?

	// Copie interdite (C++98 style)
	// Connection(const Connection&);
	// Connection& operator=(const Connection&);