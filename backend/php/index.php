<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Mesures des Pompes</title>
    
    <!-- Bootstrap CSS -->
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.3.0/dist/css/bootstrap.min.css" rel="stylesheet">
    
    <!-- Chart.js -->
    <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
</head>
<body class="bg-light">

<div class="container mt-4">
    <h2 class="text-center">Données de la pompe hydraulique</h2>

    <!-- Formulaire de sélection de plage de temps -->
    <form method="GET" class="mb-3">
        <div class="row">
            <div class="col-md-5">
                <label for="start" class="form-label">Début :</label>
                <input type="datetime-local" class="form-control" name="start" value="<?= $_GET['start'] ?? '' ?>">
            </div>
            <div class="col-md-5">
                <label for="end" class="form-label">Fin :</label>
                <input type="datetime-local" class="form-control" name="end" value="<?= $_GET['end'] ?? '' ?>">
            </div>
            <div class="col-md-2 d-flex align-items-end">
                <button type="submit" class="btn btn-primary w-100">Filtrer</button>
            </div>
        </div>
    </form>

    <?php
    $servername = "localhost";
    $username = "root";
    $password = "root";
    $dbname = "pump_data";

    try {
        $pdo = new PDO("mysql:host=$servername;dbname=$dbname", $username, $password);
        $pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
    } catch (PDOException $e) {
        die("<div class='alert alert-danger'>Erreur : " . $e->getMessage() . "</div>");
    }

    $startTime = $_GET['start'] ?? date('Y-m-d H:i:s', strtotime('-1 hour'));
    $endTime = $_GET['end'] ?? date('Y-m-d H:i:s');

    // Requête pour récupérer les mesures
    $sql = "SELECT * FROM measurements WHERE timestamp < :end";
    $stmt = $pdo->prepare($sql);
    $stmt->bindParam(':end', $endTime);
    $stmt->execute();
    $measurements = $stmt->fetchAll(PDO::FETCH_ASSOC);

    if (!$measurements) {
        echo "<div class='alert alert-warning text-center'>Aucune donnée disponible.</div>";
    } else {
        echo "<table class='table table-striped'>";
        echo "<thead class='table-dark'><tr><th>Timestamp</th><th>Température (°C)</th><th>Humidité (%)</th><th>Vitesse (RPM)</th></tr></thead><tbody>";
        foreach ($measurements as $row) {
            echo "<tr>";
            echo "<td>{$row['timestamp']}</td>";
            echo "<td>{$row['temperature']}</td>";
            echo "<td>{$row['humidity']}</td>";
            echo "<td>{$row['speed']}</td>";
            echo "</tr>";
        }
        echo "</tbody></table>";
    }

    // Préparation des données pour Chart.js
    $labels = [];
    $temperatureData = [];
    $humidityData = [];
    $speedData = [];

    foreach ($measurements as $row) {
        $labels[] = $row['timestamp'];
        $temperatureData[] = $row['temperature'];
        $humidityData[] = $row['humidity'];
        $speedData[] = $row['speed'];
    }
    ?>

    <!-- Ajout du graphique -->
    <div class="card p-3 shadow">
        <canvas id="chart"></canvas>
    </div>
</div>

<script>
    // Récupération des données PHP en JSON
    const labels = <?php echo json_encode($labels); ?>;
    const temperatureData = <?php echo json_encode($temperatureData); ?>;
    const humidityData = <?php echo json_encode($humidityData); ?>;
    const speedData = <?php echo json_encode($speedData); ?>;

    // Initialisation du graphique
    const ctx = document.getElementById('chart').getContext('2d');
    const chart = new Chart(ctx, {
        type: 'line',
        data: {
            labels: labels,
            datasets: [
                {
                    label: 'Température (°C)',
                    data: temperatureData,
                    borderColor: 'red',
                    backgroundColor: 'rgba(255, 0, 0, 0.2)',
                    fill: true
                },
                {
                    label: 'Humidité (%)',
                    data: humidityData,
                    borderColor: 'blue',
                    backgroundColor: 'rgba(0, 0, 255, 0.2)',
                    fill: true
                },
                {
                    label: 'Vitesse (RPM)',
                    data: speedData,
                    borderColor: 'green',
                    backgroundColor: 'rgba(0, 255, 0, 0.2)',
                    fill: true
                }
            ]
        },
        options: {
            responsive: true,
            plugins: {
                legend: {
                    position: 'top'
                }
            }
        }
    });
</script>

<!-- Bootstrap JS -->
<script src="https://cdn.jsdelivr.net/npm/bootstrap@5.3.0/dist/js/bootstrap.bundle.min.js"></script>

</body>
</html>