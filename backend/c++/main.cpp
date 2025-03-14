#include <iostream>
#include <mysql/mysql.h>
#include <curl/curl.h>

const char* HOST = "localhost";
const char* USER = "root";
const char* PASSWORD = "";
const char* DATABASE = "pump_data";

bool pumpRunningTooLong(MYSQL* conn) {
    MYSQL_RES* res;
    MYSQL_ROW row;

    const char* query = "SELECT TIMESTAMPDIFF(MINUTE, MIN(timestamp), NOW()) AS runtime "
                        "FROM measurements WHERE pump_status = 1";

    if (mysql_query(conn, query)) {
        std::cerr << "Erreur SQL : " << mysql_error(conn) << std::endl;
        return false;
    }

    res = mysql_store_result(conn);
    if ((row = mysql_fetch_row(res)) && row[0]) {
        int runtime = std::stoi(row[0]);
        mysql_free_result(res);
        return runtime > 10;
    }

    mysql_free_result(res);
    return false;
}

void sendAlarmEmail() {
    CURL* curl = curl_easy_init();
    if (curl) {
        const char* emailData = "Subject: Alerte Pompe\n\nLa pompe tourne depuis plus de 10 minutes.";

        curl_easy_setopt(curl, CURLOPT_URL, "smtp://smtp.votreserveur.com");
        curl_easy_setopt(curl, CURLOPT_MAIL_FROM, "votre_email@exemple.com");
        struct curl_slist* recipients = nullptr;
        recipients = curl_slist_append(recipients, "destinataire@exemple.com");
        curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, emailData);

        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK)
            std::cerr << "Erreur d'envoi de mail : " << curl_easy_strerror(res) << std::endl;

        curl_slist_free_all(recipients);
        curl_easy_cleanup(curl);
    }
}

int main() {
    MYSQL* conn = mysql_init(nullptr);
    if (!conn) {
        std::cerr << "Erreur d'initialisation MySQL" << std::endl;
        return 1;
    }

    if (!mysql_real_connect(conn, HOST, USER, PASSWORD, DATABASE, 0, nullptr, 0)) {
        std::cerr << "Erreur de connexion MySQL : " << mysql_error(conn) << std::endl;
        return 1;
    }

    if (pumpRunningTooLong(conn)) {
        sendAlarmEmail();
    }

    mysql_close(conn);
    return 0;
}
