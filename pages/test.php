<html>
<head>
	<title id="sHeader">TestLoadDiv</title>
		<link rel = "stylesheet" type = "text/css" href = "../styles/styles.css" />
		<link rel = "icon" type = "image/ico" href = "../Images/icon.ico" />
		<script src="https://ajax.googleapis.com/ajax/libs/jquery/3.4.0/jquery.min.js"></script>
		<script src="https://cdn.zingchart.com/zingchart.min.js"></script>
</head>
<body>

<div id="load"></div>

</body>

<script type="text/javascript">
$('#load').load('../data/php/minichart.php');
</script>