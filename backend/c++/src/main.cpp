#include <iostream>
#include <string>
#include <sstream>
#include <ctime>
#include <unistd.h> // sleep()
#include <cstdlib>
#include <curl/curl.h>
#include "include/nlohmann/json.hpp"

using json = nlohmann::json;
using namespace std;

const string url = "http://projetpompe.chez.com/etat_pompe.php";

// Fonction de callback pour curl
size_t WriteCallback(void* contents, size_t size, size_t nmemb, string* output) {
    size_t totalSize = size * nmemb;
    output->append((char*)contents, totalSize);
    return totalSize;
}

// Fonction pour obtenir le JSON depuis l'URL
json getJsonFromUrl(const string& url) {
    CURL* curl;
    CURLcode res;
    string response;

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 0L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            cerr << "Erreur curl : " << curl_easy_strerror(res) << endl;
            curl_easy_cleanup(curl);
            return json();  // Renvoie un JSON vide
        }

        curl_easy_cleanup(curl);
    } else {
        cerr << "Erreur : échec d'initialisation de curl.\n";
        return json();
    }

    try {
        return json::parse(response);
    } catch (...) {
        cerr << "Erreur de parsing JSON.\n";
        return json();
    }
}

void verifierEtatPompe() {
    json data = getJsonFromUrl(url);

    if (data.is_null() || data.contains("error")) {
        cerr << "Erreur ou pas de données récupérées depuis le serveur.\n";
	cerr << "Contenu reçu : " << data.dump() << endl;
        return;
    }

    string timestampStr = data["timestamp"];
    int pump_status = stoi(data["pump_status"].get<string>());

    struct tm tm{};
    strptime(timestampStr.c_str(), "%Y-%m-%d %H:%M:%S", &tm);
    time_t dernierTimestamp = mktime(&tm);
    time_t maintenant = time(nullptr);

    double ecartMinutes = difftime(maintenant, dernierTimestamp) / 60.0;

    cout << "[INFO] Pompe active ? " << pump_status << ", Dernier timestamp : " << timestampStr << ", Écart : " << ecartMinutes << " min\n";

    if (pump_status == 1 && ecartMinutes >= 10.0) {
        cout << "[ALERTE] Pompe active depuis plus de 10 minutes ! Envoi d'un mail.\n";
        system("echo 'La pompe est en marche depuis plus de 10 minutes.' | mail -s 'Alerte pompe' thibaud.lauber67000@gmail.com");
    } else {
        cout << "[OK] Pompe inactive ou active depuis moins de 10 minutes.\n";
    }
}

int main() {
    while (true) {
        verifierEtatPompe();
        sleep(20 * 60); // Attendre 20 minutes
    }
    return 0;
}
