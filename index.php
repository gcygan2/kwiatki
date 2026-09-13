<?php
if (isset ($_GET['c']) && isset ($_GET['d'])) {
	setcookie('stan', $_GET['d'], time()+3600*24*31);
	file_put_contents($_GET['d'], $_GET['c'], LOCK_EX);
	$fp = fopen("dziennik.csv", "at");
	if ($fp) {
		flock($fp, LOCK_EX);
		fwrite($fp, gmdate("Y-m-d\TH:i:s\Z", time()+date("Z"))."\t".$_GET['d']."\t".$_GET['c'].$tekst.PHP_EOL);
		flock($fp, LOCK_UN);
		fclose ($fp);
	}
	echo "<meta http-equiv=\"refresh\" content=\"0; url=panel.html\">\n";
} else if (isset ($_GET['k'])) {
	readfile ($_GET['k']);
	file_put_contents($_GET['k'], 'brak', LOCK_EX);
} else if (isset ($_COOKIE['stan'])) {
	readfile ($_COOKIE['stan']);
}
?>
