<!DOCTYPE HTML>
<html>

	<head>
		<title>Cycler Data: About</title>
		<link rel = "stylesheet" type = "text/css" href = "../styles/styles.css" />
		<link rel = "icon" type = "image/ico" href = "../Images/icon.ico" />
	</head>

	<body>
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
					<lable><a class="navLink" href="entry.php">Remote Start</a></label>
					<lable><a class="navLink" href="lookup.php">Battery Lookup</a></label>
					<lable><a class="navLink" href="about.php">About</a></label>
				</td>
				<td>
					<div id="cyclerHeading">
						<label>About</label>
					</div>
				</td>
				<td></td>
			</tr>
			<tr>
				<td class="centerTd">
				<h4>SQL Server Info</h4>
				<?php
					$serverName = "GCTUSSERVER\\CYCLERDATA, 1433";
					$connectionInfo = array( "Database"=>"CYCLERDATABASE", "UID"=>"user_dbf", "PWD"=>"CyclerData");
					$conn = sqlsrv_connect( $serverName, $connectionInfo);
					if(!$conn) {
						 echo "Connection could not be established.<br />";
						 die( print_r( sqlsrv_errors(), true));
					}
					else{
						$srvInfo = sqlsrv_server_info($conn);
						foreach($srvInfo as $sEle => $sVal){
							echo $sEle.": ".$sVal."<br/>";
						}
						$cliInfo = sqlsrv_client_info($conn);
						foreach($cliInfo as $cEle => $cVal){
							echo $cEle.": ".$cVal."<br/>";
						}
					}
				?>
				<br/>
				</td>
				<td class="sideTd"></td>
			</tr>
		</table>
	</body>
</html>