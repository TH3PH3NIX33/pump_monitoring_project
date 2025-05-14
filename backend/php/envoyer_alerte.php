<?php
// Configuration
$destinataire = "thibaud.lauber67000@gmail.com";
$sujet = "🚨 Alerte pompe en fonctionnement prolongé";
$message = "La pompe est active depuis plus de 10 minutes.\nMerci de vérifier le système.";
$headers = "From: alerte@projetpompe.chez.com\r\n";

// Envoi de l'email
if (mail($destinataire, $sujet, $message, $headers)) {
    echo "OK";
} else {
    echo "ERREUR";
}
?>