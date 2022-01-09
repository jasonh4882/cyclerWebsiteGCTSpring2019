<!DOCTYPE HTML>
<html>

	<head>
		<title>Cycler Data: Remote Start</title>
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
						Station Remote Start
					</div>
				</td>
				<td></td>
			</tr>
			<tr>
				<td class="centerTd">
					<form method="post">
						<fieldset>
						<legend>Battery Cycling Information Entry</legend>
						
							<h4>Cycler Information:</h4>
							<br />
							Cycler*:
							<br/>
							<input  type="radio" name="cycler" value="C48" checked>48V
							<input  type="radio" name="cycler" value="C80" >80V
							<br/>
							Station #*:
							<br/>
							
						
							<input type="radio" name="station" value="S1" checked>1							
							<input  type="radio" name="station" value="S2" >2
							<input  type="radio" name="station" value="S3">3
							<input  type="radio" name="station" value="S4">4
							<input  type="radio" name="station" value="S5">5
							<input  type="radio" name="station" value="S6">6
							<br/>
							<br/>
							Delay:
							<br />
							<input type="number" min="0" max="12" value="0" name="delay">Hours
							
							<br />
							
							
							<br />
							Starting State*:
							<br/>
							<select name="state" id="states" required>
								<option value="Start">Start</option>
								<option value="Discharge1">Discharge1</option>
								<option value="DischargeWait1">DischargeWait1</option>
								<option value="Charge1">Charge1</option>
								<option value="ChargeWait">ChargeWait</option>
								<option value="Discharge2">Discharge2</option>
								<option value="DischargeWait2">DischargeWait2</option>
								<option value="Charge2">Charge2</option>
							</select>
							<br/>							
							<hr>
							<h4>Battery Information:</h4>
							Model #:
							<br/>
							<input type="text" name="m" value="FBP-" size="8" maxlength="8" placeholder="####">
							<br/>
							Serial #:
							<br/>
							<input type="text" name="s" size="8" maxlength="8" placeholder="########">
							<br/>
							Voltage*:
							<br/>
							<input  type="radio" name="voltage" value="24V" checked>24V
							<input  type="radio" name="voltage" value="36V">36V
							<input  type="radio" name="voltage" value="48V">48V
							<br />
							Capacity*:
							<br />
							<input type="text" name="capacity" size="2" maxlength="4" placeholder="####" required>Ah
							<br />
							<br/>
							<input type="submit" value="Submit" >
							<br/>
							</fieldset>							
					</form>
							<?php
							
							

							
							
							$post = filter_input_array(INPUT_POST);
							$fpost = array();							
							$now=date();
							foreach($post as $element => $value){
								array_push($fpost, $value);
								//echo $value . "\n";
								//$date = date('U', strtotime($now. ' + '.$fpost[0].' hours'));
								
								$lines = file("E:/Cycler/data/php/ReceiveGET.php");
							$new = $fpost[0]." ".$fpost[1]." ".($fpost[2]*3600000)."ms ".$fpost[3]." ".$fpost[4]." ".$fpost[5]." ".$fpost[6]." \n";
							$result = '';

							foreach($lines as $line) {
								if(substr($line, 0, 6) == "".$fpost[0]." ".$fpost[1]."") {
									$result .= $new;
								} else {
									$result .= $line;
								}
							}
							file_put_contents("E:/Cycler/data/php/ReceiveGET.php", $result);								
							}						

?>
				</td>
				<td class="sideTd"></td>
			</tr>
		</table>
	</body>
</html>