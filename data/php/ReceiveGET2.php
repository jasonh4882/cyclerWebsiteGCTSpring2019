GET request received
<?php
	$get = filter_input_array(INPUT_GET);
	$fget = '';
	foreach($get as $elem => $val){
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
	
	if(time() % 60 == 0 && $aget[2] != ""){
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
		}
		$sql = "INSERT INTO ".$aget[1]."x".$aget[2]." (state, sTime, temp, volt, curr, tTime, dater ) VALUES (?, ?, ?, ?, ?, ?, GETDATE())";
		$params = array($aget[3], $aget[4], $aget[5], $aget[6], $aget[7], $aget[8] );
		$stmt = sqlsrv_query($conn, $sql, $params);
		if(!$stmt){
			die( print_r(sqlsrv_errors(), true));
		}
	}
	?>