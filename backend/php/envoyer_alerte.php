<?php
use PHPMailer\PHPMailer\PHPMailer;
use PHPMailer\PHPMailer\Exception;

require 'PHPMailer/src/PHPMailer.php';
require 'PHPMailer/src/SMTP.php';
require 'PHPMailer/src/Exception.php';

$mail = new PHPMailer(true);

try {
    // Config SMTP
    $mail->isSMTP();
    $mail->Host       = 'smtp.gmail.com';
    $mail->SMTPAuth   = true;
    $mail->Username   = 'projetpompehydraulique@gmail.com';
    $mail->Password   = 'dhcbbyzgwqmxdiqv';
    $mail->SMTPSecure = 'tls';
    $mail->Port       = 587;

    // Email
    $mail->setFrom('projetpompehydraulique@gmail.com', 'Alerte Pompe');
    $mail->addAddress('thibaud.lauber67000@gmail.com');
    $mail->Subject = '🚨 Alerte : Pompe active trop longtemps';
    $mail->Body    = "La pompe est active depuis plus de 10 minutes.\nMerci de vérifier.";

    $mail->send();
    echo 'OK';
} catch (Exception $e) {
    echo 'ERREUR : ' . $mail->ErrorInfo;
}
?>