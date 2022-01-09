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
$state=$_REQUEST['state'];
$state2=$_REQUEST['state2'];
$state3=$_REQUEST['state3'];
$query=	"SELECT row = ROW_NUMBER()  OVER (ORDER BY stval,sTime)
	 
	FROM  state_val_lookup w join Battery_Data q  on q.state = w.state where model =
	'".$model."' and serial = '".$serial."' and temp < 200
	and q.state = any (select state from STATE_VAL_LOOKUP where state like 
	'".$state."' or state like '".$state2."' or state like '".$state3."')";
$stmt=SQLSRV_QUERY($conn,$query);
?>
<link rel = "stylesheet" type = "text/css" href = "../styles/styles.css" />
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
					</a></br>

<br>
<br>
<?php

$query2 = "SELECT distinct t1.state, stval FROM Battery_Data t1 join STATE_VAL_LOOKUP t2 on t1.state = t2.state
where Model = '".$model."' and Serial=".$serial." AND t1.state NOT LIKE 'Done' order by stval";  
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
<td>
					<div id="cyclerHeading"><?php echo $battery?></div>
				</td>
				<td></td>
</tr>
<tr>


<td class="centerTd">
 <script src="Scripts/Chart.js"></script>
<link rel = "stylesheet" type = "text/css" href = "styles/Chart.css" />
<canvas id= "canvas" width="800" height="400"></canvas>
<script>
			var Rows=[<?php
			while( $info = SQLSRV_FETCH_ARRAY($stmt,SQLSRV_FETCH_ASSOC))
				 echo $info['row'].',';?>];
			  <?php
			 $query="SELECT  
 round(avg(temp)
 over (order by stval rows between 10  preceding and 10 following), 1) as temp
 FROM  state_val_lookup w join Battery_Data q  on q.state = w.state where model =
	'".$model."' and serial = '".$serial."' and temp < 200
	and q.state = any (select state from STATE_VAL_LOOKUP where state like 
	'".$state."' or state like '".$state2."' or state like '".$state3."')";
$stmt=SQLSRV_QUERY($conn,$query);
			?>
			
			var Temps=[<?php
			while( $info = SQLSRV_FETCH_ARRAY($stmt,SQLSRV_FETCH_ASSOC))
				 echo $info['temp'].',';?>];
			  <?php
			 $query="SELECT round(avg(curr)
over (order by stval rows between 10  preceding and 10 following), 1) as curr  
FROM  state_val_lookup w join Battery_Data q  on q.state = w.state where model =
'".$model."' and serial = '".$serial."' and temp < 200
and q.state = any (select state from STATE_VAL_LOOKUP where state like 
'".$state."' or state like '".$state2."' or state like '".$state3."')";
$stmt=SQLSRV_QUERY($conn,$query);
			?>
			var Currs=[<?php
			while( $info = SQLSRV_FETCH_ARRAY($stmt,SQLSRV_FETCH_ASSOC))
				 echo $info['curr'].',';?>];
			 <?php
			 $query="SELECT round(avg(volt)
 over (order by stval rows between 10  preceding and 10 following), 1) as volt
 FROM  state_val_lookup w join Battery_Data q  on q.state = w.state where model =
	'".$model."' and serial = '".$serial."' and temp < 200
	and q.state = any (select state from STATE_VAL_LOOKUP where state like 
	'".$state."' or state like '".$state2."' or state like '".$state3."')";
$stmt=SQLSRV_QUERY($conn,$query);
			?>
			 var Volts=[<?php
			while( $info = SQLSRV_FETCH_ARRAY($stmt,SQLSRV_FETCH_ASSOC))
				 echo $info['volt'].',';?>];
				
			<?php
			SQLSRV_CLOSE($conn);
			?>

 	
var ctx = document.getElementById("canvas");
 canvas = new Chart(ctx, {
  type: 'line',
  data: {
  labels: Rows,
    datasets: [
      { 
        data: Temps,
		label: "Temperature",
		borderColor: "green"
		
      },
	  { 
        data: Currs,
		label: "Current",
		borderColor: "blue"
		
      },
	  { 
        data: Volts,
		label: "Voltage",
		borderColor: "red"
		
      }
    ]
	
  },
  options: {
                legend: {
                    display: true
                },
                tooltips: {
					
					mode: 'index',
                    callbacks:{
						label: function (t, d){
							if (t.datasetIndex === 0) {
                        return  t.yLabel + ' C';
                    } else if (t.datasetIndex === 1) {
                        return t.yLabel + ' A';
                    }else if (t.datasetIndex === 2) {
                        return t.yLabel + ' V';
                    }
					}
					}
                },
                scales: {
                    xAxes: [{
                        ticks: {
                            maxTicksLimit: 20
                        }
                    }]
                },
				elements: { 
					  point: { 
						radius: 0,
						hitRadius: 10, 
						hoverRadius: 5,
							 } 
						  } 
				}
 
});
	
</script>
<canvas id= "canvas" width="800" height="400"></canvas>	
</td>
</tr>
</table>
</body>