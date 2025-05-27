<?php
require 'PHPMailer/class.phpmailer.php';
require 'PHPMailer/class.smtp.php';

$mail = new PHPMailer();

$mail->IsSMTP();
$mail->Host       = 'smtp.gmail.com';
$mail->SMTPAuth   = true;
$mail->Username   = 'projetpompehydraulique@gmail.com';
$mail->Password   = 'dhcbbyzgwqmxdiqv';
$mail->SMTPSecure = 'tls';
$mail->Port       = 587;

$mail->SetFrom('projetpompehydraulique@gmail.com', 'Alerte Pompe');
$mail->AddAddress('thibaud.lauber67000@gmail.com');
$mail->Subject = '🚨 Alerte : Pompe active trop longtemps';
$mail->Body    = "La pompe est active depuis plus de 10 minutes.\nMerci de vérifier.";

if (!$mail->Send()) {
    echo 'ERREUR : ' . $mail->ErrorInfo;
} else {
    echo 'OK';
}
?>