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
	readfile ("./stan.txt");
}
?>
