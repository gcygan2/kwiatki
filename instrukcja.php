<!DOCTYPE html>
<html lang="pl">
	<head>
		<meta charset="UTF-8">
		<meta name="viewport" content="width=device-width, initial-scale=1.0">
		<title>Instrukcja obsługi zdalnego przekaźnika</title>
	</head>
	<body>
<?php
require_once '../Parsedown.php';
$Parsedown = new Parsedown();
echo $Parsedown->text(file_get_contents('README.md'));
?>
</body></html>
