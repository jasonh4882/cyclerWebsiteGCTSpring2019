<?php
$serverName = "GCTUSSERVER\\CYCLERDATA, 1433";								
$connectionInfo = array( "Database"=>"CYCLERDATABASE", "UID"=>"user_dbf", "PWD"=>"CyclerData");
$conn = SQLSRV_CONNECT( $serverName, $connectionInfo);

if ($conn === false)
{
	echo "could not connect";
	die(print_r(SQLSRV_ERRORS(),true));
}

$battery=$_REQUEST['battery'];
$find=strpos($battery,'x');
$model=substr($battery,0,$find);
$serial=substr($battery,$find+1);
$state=$_REQUEST['state'];
$query = "SELECT Model, Serial, t1.state, sTime, temp, curr, tTime, volt, cycle FROM Battery_Data t1 
join STATE_VAL_LOOKUP t2 on t1.state=t2.state  
where t1.Model = '".$model."' and t1.Serial=".$serial." and t1.state= '".$state."' order by cycle,stval,sTime";  

$stmt = SQLSRV_QUERY($conn,$query); 

if($stmt === false){
	echo "failure \n";
	die(print_r(SQLSRV_ERRORS(),true));
}
// $datatable = array();
 // while( $row = SQLSRV_FETCH_ARRAY($stmt, SQLSRV_FETCH_ASSOC ) )
 // {
 // echo $row['state'],$row['sTime'],$row['temp'],$row['volt'],$row['curr'],$row['tTime'];
// }


?>
<html>
<head>
<style>
table {
	border-collapse:collapse;
	background-color:c3c3c3;
}
td,th{
	border: 1px solid #dddddd;
	text-align: left;
	padding: 1px;
}
tr:nth-child(even){
	background-color: #dddddd;
}
</style>
</head>
<table align="center>
<table cellspacing="5" cellpadding="0">
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
        <?php while( $row = SQLSRV_FETCH_ARRAY($stmt, SQLSRV_FETCH_ASSOC )) : ?>
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