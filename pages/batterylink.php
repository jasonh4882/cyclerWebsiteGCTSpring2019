<?php
$serverName = "GCTUSSERVER\\CYCLERDATA, 1433";								
$connectionInfo = array( "Database"=>"CYCLERDATABASE", "UID"=>"user_dbf", "PWD"=>"CyclerData");
$connectionInfo2 = array( "Database"=>"CYCLERDATABASE", "UID"=>"jryan@greencubestech.com", "PWD"=>"GCT2019jr");
$conn = SQLSRV_CONNECT( $serverName, $connectionInfo);
$conn2= SQLSRV_CONNECT( $serverName, $connectionInfo2);
if ($conn === false)
{
	echo "could not connect";
	die(print_r(SQLSRV_ERRORS(),true));
}
if ($conn2 === false)
{
	echo "connection2 failed";
	die(print_r(SQLSRV_ERRORS(),true));
}

$battery=$_REQUEST['battery'];
$find=strpos($battery,'x');
$model=substr($battery,0,$find);
$serial=substr($battery,$find+1);
?>

<html>
<title>
Battery Data Table
</title>
	<link rel = "icon" type = "image/ico" href = "../Images/icon.ico" />
<head>
<link rel = "stylesheet" type = "text/css" href = "../styles/styles.css" />
<link rel = "icon" type = "image/ico" href = "Cycler/Images/icon.ico" />

<style>

 table.center {
    margin-left:auto; 
    margin-right:auto;
  }
td,th{
	font-family: verdana;
	font-size:15px;
	border: 1px solid #00000000;
	text-align: left;
	padding: 1px;
}
tr:nth-child(even){
	background-color: #dddddd8a;
}
#cyclerH2{
	width: 100%;
	opacity: 0.9;
	min-height: 30px;
	background-color: #252B2D;
	color:white;
    font-family:Impact;
	text-shadow:
		-1px -1px 0 #000,
		1px -1px 0 #000,
		-1px 1px 0 #000,
		1px 1px 0 #000;
    text-align:center;
    font-size:20pt;	
}
#cyclerHeading{
	width: 100%;
	opacity: 0.9;
	min-height: 60px;
	background-color: #252B2D;
	color:white;
    font-family:Impact;
	text-shadow:
		-1px -1px 0 #000,
		1px -1px 0 #000,
		-1px 1px 0 #000,
		1px 1px 0 #000;
    text-align:center;
    font-size:30pt;
}
</style>

</head>
<body>
<table style="width:80%">
<tr>
<td class="sideTd" rowspan=2></td>
<td class="sideTd" id="navBar" rowspan=2>

<a href="https://greencubestech.com/">
						<img src="../Images/greencubeslogo.png" alt="green cubes logo" width="100"/>
					</a></br>

<br>
<br>
<?php
echo "<style>
.button {
  font: bold 16px Arial;
  text-decoration: none;
 
  color: white;
  padding: 2px 6px 2px 6px;

}
.button:hover {
   
   color: silver;
}

</style>";

$query2 = "SELECT distinct t1.state, stval FROM Battery_Data t1 join STATE_VAL_LOOKUP t2 on t1.state = t2.state
where Model = '%".$model."' and Serial=".$serial." AND t1.state NOT LIKE 'Done' order by stval";  
echo '<a class="navLink" href="chart.php?battery='.$battery.'&state=BulkCharge1&state2=Discharge2&state3=Charge1" class="button"> C1 and D2</a><br/>';
$stmt2 = SQLSRV_QUERY($conn2,$query2); 
while( $row = SQLSRV_FETCH_ARRAY($stmt2,SQLSRV_FETCH_ASSOC))
{
echo '<a class="navLink" href="chart.php?battery='.$battery.'&state='.$row['state'].'" class="button">'.$row['state'].'</a><br/>';
}?>
<hr/>
<lable><a class="navLink" href="../index.htm">Overview</a></label><br/>
					<lable><a class="navLink stationNav" href="station.php?station=1">Station 1</a></label><br/>
					<lable><a class="navLink stationNav" href="station.php?station=2">Station 2</a></label><br/>
					<lable><a class="navLink stationNav" href="station.php?station=3">Station 3</a></label><br/>
					<lable><a class="navLink stationNav" href="station.php?station=4">Station 4</a></label><br/>
					<lable><a class="navLink stationNav" href="station.php?station=5">Station 5</a></label><br/>
					<lable><a class="navLink stationNav" href="station.php?station=6">Station 6</a></label><br/>
					<lable><a class="navLink" href="entry.php">Remote Start</a></label><br/>
					<lable><a class="navLink" href="lookup.php">Battery Lookup</a></label><br/>
					<lable><a class="navLink" href="about.php">About</a></label><br/>
</td>
<?php
echo '<td>
					<div id="cyclerHeading">Battery Cycle Data</div>
					<div id="cyclerH2">'.$battery.'</div>
				</td>';
echo '<td></td>';
echo '</tr>';


?>
<tr>
				<td class="centerTd">
				
<table class="center" width= 100%>
    <thead>
        <tr>
            <th>State</th>
            <th>State Time</th>
            <th>Temperature</th>
			<th>Voltage</th>
            <th>Current</th>
            
        </tr>
    </thead>
    <tbody>
        <!--Use a while loop to make a table row for every DB row-->
        <?php 
				$query = "SELECT Model, Serial, t1.state, sTime, temp, curr, tTime, volt, cycle FROM Battery_Data t1 
join STATE_VAL_LOOKUP t2 on t1.state=t2.state  
where t1.Model = '".$model."' and t1.Serial='".$serial."' order by cycle,stval,sTime";  

$stmt = SQLSRV_QUERY($conn,$query); 

if($stmt === false){
	echo "failure \n";
	die(print_r(SQLSRV_ERRORS(),true));
}
				while( $row = SQLSRV_FETCH_ARRAY($stmt, SQLSRV_FETCH_ASSOC )) : ?>
        <tr>
            <!--Each table column is echoed in to a td cell-->
            <td><?php echo $row['state']; ?></td>
            <td><?php echo $row['sTime']; ?></td>
            <td><?php echo $row['temp']; ?></td>
			 <td><?php echo $row['volt']; ?></td>
            <td><?php echo $row['curr']; ?></td>
            
        </tr>
        <?php endwhile ?>
		  <?php
				SQLSRV_FREE_STMT($stmt);
				SQLSRV_CLOSE( $conn);
		  ?>
    </tbody>
</table>
</td>

</body>
