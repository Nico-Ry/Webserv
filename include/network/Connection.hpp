#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <string>
#include <stdexcept>
#include <sys/types.h>
#include <ctime>

class Connection {
public:
	class ConnectionException : public std::runtime_error {
	public:
		explicit ConnectionException(const std::string& message);
	};

	// Constructeur: met automatiquement le fd en mode non-bloquant
	explicit Connection(int fd);

	// Destructeur: ferme le fd
	~Connection();

	// Lit toutes les donnees disponibles dans recv_buffer
	// Retourne: >0 bytes lus, 0 si connexion fermee, -1 si erreur
	ssize_t read_available();

	// Envoie les donnees de send_buffer
	// Retourne: >0 bytes envoyes, 0 si rien a envoyer, -1 si erreur
	ssize_t write_pending();

	// Verifie si tout le send_buffer a ete envoye
	bool has_pending_data() const;

	// Attributs publics (simplifie l'acces pour ce projet)
	int fd;
	std::string recv_buffer;
	std::string send_buffer;
	size_t bytes_sent;
	time_t last_activity;  // Timestamp de dernière activité (pour timeout)
	bool should_close;     // Fermer la connexion apres envoi (Connection: close)

	// Met à jour le timestamp d'activité
	void update_activity();

private:
	// Met le socket en mode non-bloquant
	void set_nonblocking();

	// Copie interdite (C++98 style)
	Connection(const Connection&);
	Connection& operator=(const Connection&);
};

#endif
