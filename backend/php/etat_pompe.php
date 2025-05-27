<?php
header('Content-Type: application/json');

$servername = "projetpompe.chez.com";
$username = "projetpompe";
$password = "ZxYwY4JHoi";
$dbname = "projetpompe";

try {
    $pdo = new PDO("mysql:host=$servername;dbname=$dbname;charset=utf8", $username, $password);
    $pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

    $stmt = $pdo->prepare("SELECT timestamp, pump_status FROM measurements ORDER BY timestamp DESC LIMIT 1");
    $stmt->execute();
    $row = $stmt->fetch(PDO::FETCH_ASSOC);

    if ($row) {
        $row['pump_status'] = is_null($row['pump_status']) ? "0" : strval($row['pump_status']);
        echo json_encode($row);
    } else {
        echo json_encode(array("error" => "Aucune donnée"));
    }

} catch (PDOException $e) {
    echo json_encode(array("error" => "Erreur connexion : " . $e->getMessage()));
}
?>
