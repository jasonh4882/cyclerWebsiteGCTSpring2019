<!DOCTYPE html>

<html>
 

<head >
<title>Cycler Data: Battery Data</title>
		<link rel = "stylesheet" type = "text/css" href = "../styles/styles.css" />
		<link rel = "icon" type = "image/ico" href = "../Images/icon.ico" />
</head>

<body style="font-family: Arial">   
						 
<table style="width:100%">
		<tr>
			<td class="sideTd" rowspan=2></td>
			<td class="sideTd" id="navBar" rowspan=2>
				<a href="https://greencubestech.com/">
					<img src="../Images/greencubeslogo.png" alt="green cubes logo" width="100"/>
				</a>
				<br/>
				<br/>
				<br/>
				<br/>
				<lable><a class="navLink" href="../index.htm">Overview</a></label><br/>
				<lable><a class="navLink stationNav" href="station.php?station=1">Station 1</a></label><br/>
				<lable><a class="navLink stationNav" href="station.php?station=2">Station 2</a></label><br/>
				<lable><a class="navLink stationNav" href="station.php?station=3">Station 3</a></label><br/>
				<lable><a class="navLink stationNav" href="station.php?station=4">Station 4</a></label><br/>
				<lable><a class="navLink stationNav" href="station.php?station=5">Station 5</a></label><br/>
				<lable><a class="navLink stationNav" href="station.php?station=6">Station 6</a></label><br/>
				<lable><a class="navLink" href="entry.php">Remote Start</a></label><br/>
				<lable><a class="navLink" href="lookup.php">Battery Lookup</a></label><br/>
						<a class="navLink" href="newbattery.php">New Model...</a></label><br/>
						<a class="navLink" href="upload.php">Upload Data...</a></label><br/>
				<lable><a class="navLink" href="about.php">About</a></label><br/>
			</td>
			<td>
				<div id="cyclerHeading">Upload Battery Data</div>
				</div>
			</td>
		
		</tr>
<tr>


		<td class="centerTd">
		<form method="post">
	<input type="file" name="filename" placeholder="data.csv" value="" required><br>
	
	<label><input type="submit" value="Upload"></label>
			</form>
<?php

	$post = filter_input_array(INPUT_POST);
							$fpost = array();
							foreach($post as $element => $value)
							{
								echo $value."</br>";
								array_push($fpost, $value);
								$filename=$fpost[0];								
							} 

	$serverName = "GCTUSSERVER\\CYCLERDATA, 1433";								
$connectionInfo = array( "Database"=>"CYCLERDATABASE", "UID"=>"user_dbf", "PWD"=>"CyclerData");
$conn = SQLSRV_CONNECT( $serverName, $connectionInfo);
if ($conn === false)
{
	echo "could not connect";
	die(print_r(SQLSRV_ERRORS(),true));
}
else{
	if ( $model != "")
	{
	$sql = "BULK INSERT TestTable
FROM '".$filename."'";
	$stmt = sqlsrv_query($conn, $sql);
}
}
	if(!$stmt){
		echo "<br> Please select a file";
		die( print_r( sqlsrv_errors(), true));
	}

							
?>
		
		</td>
</tr>
</body>

</html>
