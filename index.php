<?php
if (isset ($_GET['c'])) {
	file_put_contents('./stan.txt', $_GET['c'], LOCK_EX);
	$fp = fopen("./dziennik.txt", "at");
	if ($fp) {
		flock($fp, LOCK_EX);
		fwrite($fp, gmdate("Y-m-d\TH:i:s\Z", time()+date("Z"))."\t".$_GET['c'].$tekst.PHP_EOL);
		flock($fp, LOCK_UN);
		fclose ($fp);
	}
	echo "<meta http-equiv=\"refresh\" content=\"0; url=./panel.html\">\n";
} else if (isset ($_GET['k']) && $_GET['k'] == "1234") {
	readfile ("./stan.txt");
	file_put_contents('./stan.txt', 'brak', LOCK_EX);
} else {
?>
<!DOCTYPE html>
<html lang="pl">
	<head>
		<meta charset="UTF-8">
		<meta name="viewport" content="width=device-width, initial-scale=1.0">
		<title>Status</title>
	</head>
	<body>
		<pre>
<?php 
echo "Realizowana akcja: ".file ("./stan.txt")[0].$tekst.PHP_EOL."Dziennik:".$tekst.PHP_EOL;
readfile ("./dziennik.txt");
?>
		</pre>
	</body>
</html>
<?php
}
?>
