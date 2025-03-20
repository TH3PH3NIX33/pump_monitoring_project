#include <iostream>
#include "/Applications/MAMP/Library/bin/mysql80/include/mysql.h"
#include <curl/curl.h>
#include <unistd.h>  // Pour sleep()

using namespace std;

const char* HOST = "localhost";
const char* USER = "root";
const char* PASSWORD = "root";
const char* DATABASE = "pump_data";

// Fonction pour vérifier si la pompe tourne depuis plus de 10 minutes
bool pumpRunningTooLong(MYSQL* conn) {
    MYSQL_RES* res;
    MYSQL_ROW row;

    const char* query = "SELECT TIMESTAMPDIFF(MINUTE, MIN(timestamp), NOW()) AS runtime "
                        "FROM measurements WHERE pump_status = 1";

    if (mysql_query(conn, query)) {
        cerr << "Erreur SQL : " << mysql_error(conn) << endl;
        return false;
    }

    res = mysql_store_result(conn);
    if (!res) {
        cerr << "Erreur récupération résultat SQL : " << mysql_error(conn) << endl;
        return false;
    }

    row = mysql_fetch_row(res);
    if (row && row[0]) {
        try {
            int runtime = stoi(row[0]);
            mysql_free_result(res);
            return runtime > 10;  // Retourne vrai si la pompe tourne depuis plus de 10 minutes
        } catch (const exception& e) {
            cerr << "Erreur conversion runtime : " << e.what() << endl;
        }
    }

    mysql_free_result(res);
    return false;
}

// Fonction pour envoyer un mail d'alarme
void sendAlarmEmail() {
    CURL* curl = curl_easy_init();
    if (curl) {
        const char* emailData = "Subject: Alerte Pompe\n\nLa pompe tourne depuis plus de 10 minutes.";

        curl_easy_setopt(curl, CURLOPT_URL, "smtp://smtp.gmail.com:465");
        curl_easy_setopt(curl, CURLOPT_MAIL_FROM, "projetpompehydraulique@gmail.com");
        
        struct curl_slist* recipients = nullptr;
        curl_easy_setopt(curl, CURLOPT_USERNAME, "projetpompehydraulique@gmail.com");
        curl_easy_setopt(curl, CURLOPT_PASSWORD, "dhcbbyzgwqmxdiqv");
        recipients = curl_slist_append(recipients, "thibaud.lauber67000@gmail.com");
        curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, emailData);
        curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_ALL);  // Sécurisation si nécessaire

        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK)
            cerr << "Erreur d'envoi de mail : " << curl_easy_strerror(res) << endl;
        else
            cout << "E-mail d'alarme envoyé avec succès !" << endl;

        curl_slist_free_all(recipients);
        curl_easy_cleanup(curl);
    }
}

int main() {
    while (true) { 
        MYSQL* conn = mysql_init(nullptr);
        if (!conn) {
            cerr << "Erreur d'initialisation MySQL" << endl;
            return 1;
        }

        if (!mysql_real_connect(conn, HOST, USER, PASSWORD, DATABASE, 0, nullptr, 0)) {
            cerr << "Erreur de connexion MySQL : " << mysql_error(conn) << endl;
            return 1;
        }

        if (pumpRunningTooLong(conn)) {
            sendAlarmEmail();
        } else {
            cout << "Pompe OK. Aucune alerte nécessaire." << endl;
        }

        mysql_close(conn);
        
        cout << "Attente de 20 minutes avant la prochaine vérification..." << endl;
        // sleep(1200);
        sleep(15);
    }

    return 0;
}