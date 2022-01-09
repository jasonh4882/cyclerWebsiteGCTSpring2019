<?php 
	$get = filter_input_array(INPUT_GET);
	$fget = array();
	foreach($get as $element => $value){
		array_push($fget, $value);
	} 
?>
<!DOCTYPE HTML>
<html>
	<head>
		<title id="sHeader">Cycler Data: Station</title>
		<link rel = "stylesheet" type = "text/css" href = "../styles/styles.css" />
		<link rel = "icon" type = "image/ico" href = "../Images/icon.ico" />
		<script src="https://ajax.googleapis.com/ajax/libs/jquery/3.4.0/jquery.min.js"></script>
		<script src="../Scripts/Chart.js"></script>
		<link rel = "stylesheet" type = "text/css" href = "../styles/Chart.css" />
	</head>

	<body>
		<table style="width:100%">
			<tr>
				<td class="sideTd" rowspan=2></td>
				<td class="sideTd" id="navBar" rowspan=2>
					<a href="https://greencubestech.com/">
						<img src="../Images/greencubeslogo.png" alt="green cubes logo" width="100"/>
					</a>
					<span id="nowTime">12:00:00 AM</span>
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
					<div id="cyclerHeading">Station</div>
				</td>
				<td></td>
			</tr>
			<tr>
				<td class="centerTd">				
					<div class="column">
						<h3 id="sTitle">Station</h3>
						<label>Connection: <span id="s0Connection" class="offline">Offline</span><br/></label>
						<label>Status: <span id="s0Status" class="offline">Offline</span><br/></label>
						<h4>Current Cycling<br/> Information<br/></h4>
						<ul>
							<li>Model: <span id="s0Model">FBP-####</span></li>
							<li>Serial #: <span id="s0Serial">########</</span></li>
							<li>State: <span id="s0State">#######</</span></li>
							<li>State Time: <span id="s0STime">00:00:00</span></li>
							<li>Total Time: <span id="s0TTime">00:00:00</span></li>
							<li>Estimated Time:<span id="s0TCycle">00:00:00</span></li>
							<li>Temperature: <span id="s0Temp">####° C</span></li>
							<li>Voltage: <span id="s0Voltage">###### V</span></li>
							<li>Current: <span id="s0Current">###### A</span></li>
							<li>Voltage: <span id="s0Volt">## V </span></li>
							<li>Capacity: <span id="s0Cap">## Ah</span></li>
							<li># of cells: <span id="s0Cells">##</span></li>
						</ul>
						<div id="ref"></div>
						<hr/>		
					
						
					</div>
					<div class="lgcolumn">

					
					<canvas id="Chart">Error</canvas>
					
					</div>
				</td>
				<td class="sideTd"></td>
				
			</tr>
		</table>
		<script type="text/javascript">
			var currentStation = "<?php echo $fget[0] ?>";
			document.getElementById("cyclerHeading").innerHTML = "Station " + currentStation;
			document.getElementById("sTitle").innerHTML = "Station " + currentStation;
			document.getElementById("sHeader").innerHTML = "Cycler Data: Station " + currentStation;
			refresh();
			var refreshAll = setInterval(refresh, 1000);
			function refresh(){
				var d = new Date();
				var t = d.toLocaleTimeString();
				document.getElementById("nowTime").innerHTML = t;
				$('#ref').load('../data/php/allRefresh.php?req=3&s-ID=0' + '&f-ID=' + currentStation + '&ts=' + d.getTime());
			}
		</script>
		<script type="text/javascript">
			var currentStation = "<?php echo $fget[0] ?>";					
			refresh();
				var refreshAll = setInterval(refresh, 60000);
				function refresh(){			
				var d = new Date();	
				$('#Chart').load('../data/php/charts.php?req=1&s-ID=0' + '&f-ID=' + currentStation +'&ts=' + d.getTime());
				}
		</script>
	</body>
</html>