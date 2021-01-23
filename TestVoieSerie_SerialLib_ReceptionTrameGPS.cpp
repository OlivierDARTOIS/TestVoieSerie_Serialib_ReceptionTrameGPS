// Exemple d'utilisation de la voie série avec serialib
// Réception et analyse d'une trame GPS RMC : récupération de l'heure
// O. Dartois - Janvier 2021

#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <sstream>

#include "serialib.h"

// Fonction de découpage d'une chaine en sous chaine à partir d'un délimiteur
std::vector<std::string> split(const std::string& s, char delim) {
	std::stringstream ss(s);
	std::string item;
	std::vector<std::string> elems;
	while (std::getline(ss, item, delim))
		elems.push_back(item);
	return elems;
}

int main()
{
	// Instanciation d'un objet de la classe serialib
	serialib serial;

	// Nom du port COM à ouvrir
	const std::string portCOM("COM4");

	// Ouverture du port série à la vitesse spécifié
	char erreur = serial.Open(portCOM.c_str(), 115200);

	// Si pb à l'ouverture du port série, on sort avec un message
	if (erreur != 1) {
		std::cout << "Ouverture du port serie KO !" << std::endl;
		return 0;
	}

	std::cout << "Ouverture du port serie OK" << std::endl;

	// Réception des caractères sur la voie série
	erreur = 0;
	char trameGPS[200]; // Stockage de la trame GPS recue par la voie serie
	bool debutTrame = false; // pour être certain d'etre au debut d'une trame
	// cette boucle vérifie que l'on est bien au début d'une trame GPS
	// elle commence toute par le caractère '$'
	do {
		char caracRecu;
		erreur = serial.ReadChar(&caracRecu);
		// Erreur de réception sur le port série
		if (erreur < 0) {
			std::cout << "Erreur réception" << std::endl;
			serial.Close();
			return 0;
		}
		if (caracRecu == '$') debutTrame = true;
	} while (debutTrame == false);

	do {
		// Lecture d'une chaine se terminant par un retour chariot '\n' (fin de ligne d'une trame GPS)
		// sur le port série avec un "timeout"  de 5s
		erreur = serial.ReadString(trameGPS, '\n', 200, 5000);
		// Erreur de réception sur le port série
		if (erreur < 0) {
			std::cout << "Erreur réception" << std::endl;
			serial.Close();
			return 0;
		}
		// Fin du temps imparti pour la réception d'un caractère
		else if (erreur == 0)
			std::cout << "Reception timeout" << std::endl;
		else {
			// Analyse de la trame pour en extraire l'heure
			std::string trameGPSAAnalyser(trameGPS);
			if (trameGPSAAnalyser.substr(1, 5) == "GPRMC") {
				// découpage de la trame en morceaux suivant le caractère ','
				std::vector<std::string> partiesTrameGPS = split(trameGPSAAnalyser, ',');
				// la partie 1 contient l'heure au format HHMMSS
				std::cout << partiesTrameGPS.at(1).substr(0, 2) << ":"; // affichage heures
				std::cout << partiesTrameGPS.at(1).substr(2, 2) << ":"; // affichage minutes
				std::cout << partiesTrameGPS.at(1).substr(4, 2) << std::endl; // affichage secondes
			}
		}
	} while (erreur != 0);

	// Fermeture du port série
	serial.Close();

	return 0;
}
