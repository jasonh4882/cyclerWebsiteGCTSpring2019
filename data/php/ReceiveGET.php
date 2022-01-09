GET request received
<?php

		$get = filter_input_array(INPUT_GET);
		$fget = '';
	foreach($get as $elem => $val)
		{
		$fget = $val;
		break;
		} 
		$aget = array();
		$fpath = '../GETdata/s'.$fget.'.dbf';
		$file = fopen($fpath, 'w');
	foreach($get as $element => $value)
		{
		$data = $element."=".$value."\n";
		if($element == 'station'){
			continue;
		}
		fwrite($file, $data);
		array_push($aget, $value);
		} 
		$now = "now=".time();
		
	
		fwrite($file, $now);
		fclose($file);
	
	if($aget[2] == "0")
		{
		$aget[2] = $fget.date("ymd");			
		}		
	if($aget[4] <= $sTime)
		{
		$state = $aget[3];
		$sTime = $aget[4];
		}
	else
	{
	$sTime = $aget[4]+1;
	}
   
		
	if(time() % 60 == 0)
	{
	if($aget[2] != "" && $aget[3] != "Done" && strpos($aget[1], "FBP") !== false && strlen($aget[1]) == 8){
		
		
		$aget[1] = str_replace('-', '', $aget[1]);
		$serverName = "GCTUSSERVER\\CYCLERDATA, 1433";
		$connectionInfo = array( "Database"=>"CYCLERDATABASE", "UID"=>"user_dbf", "PWD"=>"CyclerData");
		$conn = sqlsrv_connect( $serverName, $connectionInfo);
		if(!$conn) {
			 echo "Connection could not be established.<br />";
			 die( print_r( sqlsrv_errors(), true));
		}
		$sql = "IF EXISTS (SELECT * FROM INFORMATION_SCHEMA.TABLES WHERE TABLE_NAME = N'".$aget[1]."x".$aget[2]."') BEGIN PRINT 'Yes' END ELSE BEGIN PRINT 'No' END";
		$stmt = sqlsrv_query($conn, $sql);
		$error = sqlsrv_errors();
		$error_array = $error[0];
		$message = substr($error_array[2], -3);
		if($message != 'Yes'){
			$sql = 	"CREATE TABLE ".$aget[1]."x".$aget[2]."(state varchar(15), sTime int, temp numeric(6, 2), volt numeric(6, 2), curr numeric(6, 2), tTime int, dater DATETIME not null)";
			$stmt = sqlsrv_query($conn, $sql);
			$sql = '';
		}
		if($message != 'No'){
		$sql = "INSERT INTO ".$aget[1]."x".$aget[2]." (state, sTime, temp, volt, curr, tTime, dater ) VALUES (?, ?, ?, ?, ?, ?, GETDATE())";
		$params = array($aget[3], $aget[4], $aget[5], $aget[6], $aget[7], $aget[8] );
		$stmt = sqlsrv_query($conn, $sql, $params);
		$sql = "INSERT INTO Battery_Data (model, serial, state, sTime, temp, curr, tTime, volt) VALUES (?, ?, ?, ?, ?, ?, ?, ?)";
		$params = array($aget[1], $aget[2], $aget[3], $aget[4], $aget[5], $aget[7], $aget[8], $aget[6] );
		$stmt = sqlsrv_query($conn, $sql, $params);
		$sql = '';
		}
		if(!$stmt){
			die( print_r(sqlsrv_errors(), true));
		}	
	}
	 if($aget[2] != "" && $aget[3] == "Done" && strpos($aget[1], "FBP") !== false){
		$aget[1] = str_replace('-', '', $aget[1]);
		$serverName = "GCTUSSERVER\\CYCLERDATA, 1433";
		$connectionInfo = array( "Database"=>"CYCLERDATABASE", "UID"=>"user_dbf", "PWD"=>"CyclerData");
		$conn = sqlsrv_connect( $serverName, $connectionInfo);
		if(!$conn) {
			 echo "Connection could not be established.<br />";
			 die( print_r( sqlsrv_errors(), true));
		}
		$sql = "IF EXISTS (SELECT * FROM Battery_CycleDate WHERE Model like '".$aget[1]."' and Serial like '".$aget[2]."') BEGIN PRINT 'Yes' END ELSE BEGIN PRINT 'No' END";
		$stmt = sqlsrv_query($conn, $sql);
		$error = sqlsrv_errors();
		$error_array = $error[0];
		$message = substr($error_array[2], -3);
		if($message != 'No'){
			$sql = 	"INSERT INTO Battery_CycleDate (Model, Serial, DateCycled ) VALUES (?, ?, GETDATE())";
			$params = array($aget[1], $aget[2] );
			$stmt = sqlsrv_query($conn, $sql, $params);
			$sql = '';
		}
		if(!$stmt){
			die( print_r(sqlsrv_errors(), true));
		}	
	}
		$sTime = $aget[4];
	}
	?>
C48 S1 3600000ms Charge1 FBP-1222 88888888 36V
C48 S2 3600000ms Start FBP-1233 10000000 36V
C48 S3
C48 S4 10800000ms Start FBP-1000 10000001 36V
C48 S5 
C48 S6
C80 S1 3HRS Start FBP-1000 00000111 48V
C80 S2
C80 S3 10800000ms Discharge1 FBP-1245 20093456 48V 
C80 S4 0HRS Start FBP-1235 12000000 36V
C80 S5
C80 S6 3HRS Start FBP-0000 19070002 48V