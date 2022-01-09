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
$query=	"SELECT [Model]
	  ,[Serial]
	  ,q.state	  
 ,round(avg(temp)
 over (order by stval rows between 10  preceding and 10 following), 1) as temp
 ,round(avg(volt)
 over (order by stval rows between 10  preceding and 10 following), 1) as volt
 ,round(avg(curr)
 over (order by stval rows between 10  preceding and 10 following), 1) as curr
  
	FROM  state_val_lookup w join Battery_Data q  on q.state = w.state where model =
	'".$model."' and serial = '".$serial."' and temp < 200
	and q.state = any (select state from STATE_VAL_LOOKUP where state like 
	'".$state."' or state like '".$state2."' or state like '".$state3."')";
$stmt=SQLSRV_QUERY($conn,$query);
?>
<link rel = "stylesheet" type = "text/css" href = "../styles/styles.css" />
<script src="https://cdn.zingchart.com/zingchart.min.js"></script>
<style>
        html,
		 
        
 
        .chart--container {
			
            height: 400px;
            width: 100%;
            min-height: 400px;
			
        }
 
        .zc-ref {
			
            display: none;
        }
 
        zing-grid[loading] {
			
            height: 150px;
        }
    </style>
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
</tr>
<tr>


<td class="centerTd">
 <div id="myChart" class="chart--container">
	<a class="zc-ref" href="https://www.zingchart.com">Powered by ZingChart</a>
	<script type="text/javascript" charset="utf-8" src="pages/stationchart.js"></script> 
 </div>


</td>
</tr>
</table>
</body>