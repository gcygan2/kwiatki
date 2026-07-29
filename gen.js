async function getData() {
	try {
		const response = await fetch("./index.php", {cache: "no-store"});
		if (!response.ok) throw new Error();
		document.getElementById("dane").innerHTML = await response.text();
	}
	catch (err) {
		console.error(err);
	}
	setTimeout(getData, 5000);
}

getData();
