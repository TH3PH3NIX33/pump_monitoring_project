-- Création de la base de données
CREATE DATABASE IF NOT EXISTS pump_data;
USE pump_data;

-- Création de la table des mesures
CREATE TABLE IF NOT EXISTS measurements (
    id INT AUTO_INCREMENT PRIMARY KEY,
    timestamp DATETIME NOT NULL,
    pump_status TINYINT(1) NOT NULL
);

-- Ajout de quelques fausses données
INSERT INTO measurements (timestamp, pump_status) VALUES
('2025-03-11 08:00:00', 1),
('2025-03-11 08:05:00', 1),
('2025-03-11 08:15:00', 1),
('2025-03-11 08:30:00', 0),
('2025-03-11 09:00:00', 1),
('2025-03-11 09:05:00', 1);

-- Requête pour vérifier si la pompe tourne depuis plus de 10 minutes
SELECT TIMESTAMPDIFF(MINUTE, MIN(timestamp), NOW()) AS runtime
FROM measurements
WHERE pump_status = 1;
