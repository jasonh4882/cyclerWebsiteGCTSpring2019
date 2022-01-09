
<?php
$serverName = "GCTUSSERVER\\CYCLERDATA, 1433";								
$connectionInfo = array( "Database"=>"CYCLERDATABASE", "UID"=>"user_dbf", "PWD"=>"CyclerData");
$conn = SQLSRV_CONNECT( $serverName, $connectionInfo);

if ($conn === false)
{
	echo "could not connect";
	die(print_r(SQLSRV_ERRORS(),true));
}

$query = "execute spModelTable";  

$stmt = SQLSRV_QUERY($conn,$query); 

if($stmt === false){
	echo "failure \n";
	die(print_r(SQLSRV_ERRORS(),true));
}
$datatable = array();
 while( $row = SQLSRV_FETCH_ARRAY($stmt, SQLSRV_FETCH_ASSOC ) )
 {	 
 $data = array('Model'=>$row['Model'],'Vehicle'=>$row['Vehicle'],'Charger'=>$row['Charger'],'Status'=>$row['Status'],'ServiceBOM'=>$row['ServiceBOM']);
 array_push($datatable,$data);
 }

 echo json_encode($datatable);

	SQLSRV_FREE_STMT($stmt);
    SQLSRV_CLOSE( $conn);
	
	
?>

