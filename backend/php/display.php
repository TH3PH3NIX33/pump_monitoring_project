<?php
// Connexion à la base de données MySQL
$servername = "localhost";
$username = "root";
$password = "";
$dbname = "pump_data";

try {
    $pdo = new PDO("mysql:host=$servername;dbname=$dbname", $username, $password);
    $pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
} catch (PDOException $e) {
    die("Erreur : " . $e->getMessage());
}

// Récupération des paramètres de plage de temps
$startTime = $_GET['start'] ?? date('Y-m-d H:i:s', strtotime('-1 hour'));
$endTime = $_GET['end'] ?? date('Y-m-d H:i:s');

// Requête pour récupérer les mesures
$sql = "SELECT * FROM measurements WHERE timestamp BETWEEN :start AND :end";
$stmt = $pdo->prepare($sql);
$stmt->execute(['start' => $startTime, 'end' => $endTime]);
$measurements = $stmt->fetchAll(PDO::FETCH_ASSOC);

// Affichage des mesures sous forme de tableau
echo "<table border='1'>";
echo "<tr><th>Timestamp</th><th>Temperature</th><th>Humidity</th><th>Speed</th></tr>";
foreach ($measurements as $row) {
    echo "<tr>";
    echo "<td>{$row['timestamp']}</td>";
    echo "<td>{$row['temperature']}</td>";
    echo "<td>{$row['humidity']}</td>";
    echo "<td>{$row['speed']}</td>";
    echo "</tr>";
}
echo "</table>";
?>
