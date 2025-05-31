#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <ctime>
#include <unistd.h>
#include <cstdlib>
#include "include/nlohmann/json.hpp" 

using json = nlohmann::json;
using namespace std;

const string url = "http://projetpompe.chez.com/etat_pompe.php";
const string fichierTemp = "/tmp/etat_pompe.json";

json getJsonFromFile(const string& filepath) {
    ifstream in(filepath);
    if (!in) {
        cerr << "Erreur : impossible d’ouvrir le fichier JSON." << endl;
        return json();
    }

    try {
        json data;
        in >> data;
        return data;
    } catch (...) {
        cerr << "Erreur de parsing JSON." << endl;
        return json();
    }
}

void verifierEtatPompe() {
    // Récupérer les données avec wget
    string commande = "wget -q -O " + fichierTemp + " " + url;
    int resultat = system(commande.c_str());

    if (resultat != 0) {
        cerr << "Erreur : échec de récupération avec wget.\n";
        return;
    }

    json data = getJsonFromFile(fichierTemp);
    if (data.is_null() || data.contains("error")) {
        cerr << "Erreur ou pas de données récupérées.\n";
        cerr << "Contenu reçu : " << data.dump() << endl;
        return;
    }

    string timestampStr = data["timestamp"];
    if (!data.contains("pump_status") || data["pump_status"].is_null()) {
        cerr << "Erreur : pump_status manquant ou null." << endl;
        return;
    }

    int pump_status = 0;
    try {
        if (data["pump_status"].is_string()) {
            pump_status = stoi(data["pump_status"].get<string>());
        } else if (data["pump_status"].is_number_integer()) {
            pump_status = data["pump_status"];
        } else {
            cerr << "Erreur : pump_status invalide." << endl;
            return;
        }
    } catch (...) {
        cerr << "Erreur lors de la conversion de pump_status." << endl;
        return;
    }

    struct tm tm{};
    strptime(timestampStr.c_str(), "%Y-%m-%d %H:%M:%S", &tm);
    time_t dernierTimestamp = mktime(&tm);
    time_t maintenant = time(nullptr);

    double ecartMinutes = difftime(maintenant, dernierTimestamp) / 60.0;

    cout << "[INFO] Pompe active ? " << pump_status
         << ", Dernier timestamp : " << timestampStr
         << ", Écart : " << ecartMinutes << " min\n";

    if (pump_status == 1 && ecartMinutes >= 10.0) {
        cout << "[ALERTE] Pompe active depuis plus de 10 minutes ! Envoi du mail via PHP.\n";
        //system("wget -q -O /dev/null http://projetpompe.chez.com/envoyer_alerte.php");
        system("/usr/local/bin/alerte_mailjet.sh");
    } else {
        cout << "[OK] Pompe inactive ou active depuis moins de 10 minutes.\n";
    }
}

int main() {
    while (true) {
        verifierEtatPompe();
        sleep(20 * 60);
    }
    return 0;
}