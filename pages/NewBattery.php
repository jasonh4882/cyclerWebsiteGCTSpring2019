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
				<div id="cyclerHeading">New Battery Model</div>
				</div>
			</td>
		<td></td>
		</tr>
<tr>
		<td class="centerTd">
		<form method="post">
	<label for="Model"> Model #:</label><input type="text" name="Model" placeholder="####" value="" required pattern="\d{4}"><br>
	<label for="Voltage">Voltage V:</label><input type="text" name="Voltage" placeholder="##"value="" required><br>
	<label for="Capacity">Capacity Ah:</label><input type="text" name="Capacity" placeholder="###" value="" required><br>
	<label><input type="submit" value="Create"></label>
			</form>
<?php

	$post = filter_input_array(INPUT_POST);
							$fpost = array();
							foreach($post as $element => $value)
							{
								echo $value."</br>";
								array_push($fpost, $value);
								$model=$fpost[0];
								$voltage=$fpost[1];
								$capacity=$fpost[2];
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
	$sql = "INSERT INTO Volt_Cap_Lookup (Model, Voltage, Capacity)
VALUES ('FBP".$model."',".$voltage.",".$capacity.")";
	$stmt = sqlsrv_query($conn, $sql);
}
}
	if(!$stmt){
		echo "<br> Please Enter a new Model";
		die( print_r( sqlsrv_errors(), true));
	}

							
?>
		
		</td>
		
</tr>
</table>
<td class="sideTd"></td>
</body>

</html>
