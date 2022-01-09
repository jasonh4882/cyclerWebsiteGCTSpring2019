<script type="text/javascript">
	<?php 
		$get = filter_input_array(INPUT_GET);
		$fget = array();
		foreach($get as $element => $value){
			array_push($fget, $value);
		} 
		$fpath = '../GETdata/s'.$fget[2].'.dbf';
		$file = fopen($fpath, "r");
		$rawlines = fread($file, filesize($fpath));
		$lines = preg_split("/[\s,]+/",$rawlines);
		$arr = array();
		foreach($lines as $line){
			$exline = explode("=",$line);
			$var = trim($exline[1]);
			array_push($arr, $var);
		}
		fclose($file);
		
		
		
		$time1 = (int)$arr[9];
		$time2 = time();
		$rawTime = (int)$arr[8];
		if(($time2 - $time1) >= 12){
			for($q = 0; $q < sizeof($arr); $q++){
				$arr[$q] = "~";
			}
		}
		elseif($arr[3] == ""){
			for($q = 1; $q < sizeof($arr); $q++){
				$arr[$q] = "~";
			}
		}
		elseif($arr[8] == $arr[4] || $arr[6] == $arr[7] && $arr[6] != "0.00"){
			for($q = 1; $q < sizeof($arr); $q++){
				$arr[$q] = "";
			}
		}
		else{
			$total = [
				0 => "00",
				1 => "00",
				2 =>$rawTime,
			];
			if((int)$total[2] >59){
				$total[1] = (int)($total[2] / 60); 
				$total[2] = (int)$total[2] % 60;
				if((int)$total[1] >59){
					$total[0] = (int)($total[1] / 60); 
					$total[1] = (int)$total[1] % 60;
				}
			}
			$total[2] = sprintf('%02d', (int)$total[2]);
			$total[1] = sprintf('%02d', (int)$total[1]);
			$total[0] = sprintf('%02d', (int)$total[0]);
			$arr[8] = implode(":", $total);
			
			$rawTime2 = (int)$arr[4];
			$total2 = [
				0 => "00",
				1 => "00",
				2 =>$rawTime2,
			];
			if((int)$total2[2] >59){
				$total2[1] = (int)($total2[2] / 60); 
				$total2[2] = (int)$total2[2] % 60;
				if((int)$total2[1] >59){
					$total2[0] = (int)($total2[1] / 60); 
					$total2[1] = (int)$total2[1] % 60;
				}
			}
			$total2[2] = sprintf('%02d', (int)$total2[2]);
			$total2[1] = sprintf('%02d', (int)$total2[1]);
			$total2[0] = sprintf('%02d', (int)$total2[0]);
			$arr[4] = implode(":", $total2);
			
		}
	
	
		$serverName = "GCTUSSERVER\\CYCLERDATA, 1433";								
		$connectionInfo = array( "Database"=>"CYCLERDATABASE", "UID"=>"user_dbf", "PWD"=>"CyclerData");
		$conn = SQLSRV_CONNECT( $serverName, $connectionInfo);

		if ($conn === false)
		{
			echo "could not connect";
			die(print_r(SQLSRV_ERRORS(),true));
		}
		$model = str_replace('-', '', $arr[1]);
		$query = "select  max(abs(curr)) as curr, capacity, voltage 
		from Battery_Data t 
		join VOLT_CAP_LOOKUP r 
		on t.model= r.model 
		where t.model = '".$model."'  
		and curr < 150 group by capacity, voltage";  

		$stmt = SQLSRV_QUERY($conn,$query); 
		
		$curr = "~";
		$cap = "~"; 
		$volt = "~";
		$tcharge = "~";
		
	
		while( $row = SQLSRV_FETCH_ARRAY($stmt,SQLSRV_FETCH_ASSOC))
		{
		$curr = $row['curr'];
		$cap = $row['capacity'];
		$volt = $row['voltage'];
		$cells = (int)($volt/3);
		$tcharge = $cap/$curr *3600;
		$tcycraw = (int)((2*$tcharge + 3600*(6.5))-$rawTime);
		
		
		
		$tcycarr = [
				0 => "00",
				1 => "00",
			

			2 =>$tcycraw,
					];
			if((int)$tcycarr[2] >59){
				$tcycarr[1] = (int)($tcycarr[2] / 60); 
				$tcycarr[2] = (int)$tcycarr[2] % 60;
				if((int)$tcycarr[1] >59){
					$tcycarr[0] = (int)($tcycarr[1] / 60); 
					$tcycarr[1] = (int)$tcycarr[1] % 60;
				}
			}
			$tcycarr[2] = sprintf('%02d', (int)$tcycarr[2]);
			$tcycarr[1] = sprintf('%02d', (int)$tcycarr[1]);
			$tcycarr[0] = sprintf('%02d', (int)$tcycarr[0]);
			$tcycle = implode(":", $tcycarr);	
			
		}	
if($arr[2] == 0)
		{
		$arr[2] = $fget[2].date("ymd");
		}		
$serial = $arr[2];
	
$Discharge1 = '~';
$DischargeWait1 = '~';
$BulkCharge1 = '~';
$Charge1 = '~';
$ChargeWait = '~';
$Discharge2 = '~';
$DischargeWait2 = '~';
$BulkCharge2 = '~';
$Charge2 = '~';
$FinishCharge2 = '~';
$query="SELECT distinct state = t2.state, sTimes = max(sTime),stval
FROM Battery_Data t1 join state_val_lookup t2 on t1.state = t2.state where model = '".$model."' AND serial = '".$serial."' group by t2.stval, t2.state order by stval";
$stmt=SQLSRV_QUERY($conn,$query);
	while( $row = SQLSRV_FETCH_ARRAY($stmt,SQLSRV_FETCH_ASSOC))
		{
		$total3 = [
				0 => "00",
				1 => "00",
				2 =>$row['sTimes'],
					];
			if((int)$total3[2] >59){
				$total3[1] = (int)($total3[2] / 60); 
				$total3[2] = (int)$total3[2] % 60;
				if((int)$total3[1] >59){
					$total3[0] = (int)($total3[1] / 60); 
					$total3[1] = (int)$total3[1] % 60;
				}
			}
			$total3[2] = sprintf('%02d', (int)$total3[2]);
			$total3[1] = sprintf('%02d', (int)$total3[1]);
			$total3[0] = sprintf('%02d', (int)$total3[0]);
			$sTimes = implode(":", $total3);	
			
			
if( $row['state'] == 'Discharge1')
{$Discharge1 = $arr[4];
if 	($arr[3] != 'Discharge1')
{$Discharge1 = $sTimes;}}
elseif($row['state'] == 'DischargeWait1')
{$DischargeWait1 = $arr[4];
if 	($arr[3] != 'DischargeWait1')
{$DischargeWait1 = $sTimes;}}
elseif($row['state'] == 'BulkCharge1')
{$BulkCharge1 = $arr[4];
if 	($arr[3] != 'BulkCharge1')
{$BulkCharge1 = $sTimes;}}
elseif($row['state'] == 'ChargeWait')
{$ChargeWait = $arr[4];
if 	($arr[3] != 'ChargeWait')
{$ChargeWait = $sTimes;}}
elseif($row['state'] == 'Discharge2')
{$Discharge2 = $arr[4];
if 	($arr[3] != 'Discharge2')
{$Discharge2 = $sTimes;}}
elseif($row['state'] == 'DischargeWait2')
{$DischargeWait2 = $arr[4];
if 	($arr[3] != 'DischargeWait2')
{$DischargeWait2 = $sTimes;}}
elseif($row['state'] == 'BulkCharge2')
{$BulkCharge2 = $arr[4];
if 	($arr[3] != 'BulkCharge2')
{$BulkCharge2 =  $sTimes;}}
elseif($row['state'] == 'FinishCharge2')
{$FinishCharge2 = $arr[4];
if 	($arr[3] != 'FinishCharge2')
{$FinishCharge2 = $sTimes;}}
else
{$Discharge1 = 'ERROR!';}
		}
?>
	var getReq1 = parseInt("<?php echo $fget[0] ?>");
	var station = "<?php echo $fget[1] ?>";
	station = ("s" + station);
	if (getReq1 >= 1){
		var randTime = "";
		var isConnected = "";
		var status = "";
		var model = "";
		var serialNum = "";
		var sysState = "";
		var sTime = "";
		var tTime = "";	
		var temp = "";
		var volt = "";
		var curr = "";
		var tcycle = "";
		var cells = "";
		var capacity = "";
		var capacity = "";
		var rmodel = "";		
		var prevState = document.getElementById(station + "State").textContent;
		<?php if($arr[0] != "~"): ?>
		isConnected = "Online";
		status = "<?php echo $arr[0] ?>";
		<?php else: ?>
		isConnected = "Offline";
		status = "Offline";
		<?php endif ?>
		if(isConnected == "Online"){
			if(document.getElementById(station + "Connection").classList.contains("offline")){
				document.getElementById(station + "Connection").classList.remove("offline");
			}
			if(document.getElementById(station + "Connection").classList.contains("online") == false){
				document.getElementById(station + "Connection").classList.add("online");
			}
		}else if(isConnected == "Offline"){
			if(document.getElementById(station + "Connection").classList.contains("online")){
				document.getElementById(station + "Connection").classList.remove("online");
			}
			if(document.getElementById(station + "Connection").classList.contains("offline") == false){
				document.getElementById(station + "Connection").classList.add("offline");
			}
		}
		if((status == "Idle" || status == "Open") && (prevState == "Done" || prevState == "#######")){
			if(document.getElementById(station + "Status").classList.contains("offline")){
				document.getElementById(station + "Status").classList.remove("offline");
			}
			if(document.getElementById(station + "Status").classList.contains("online") == false){
				document.getElementById(station + "Status").classList.add("online");
			}
		}else if(status == "Cycling"){
			if(document.getElementById(station + "Status").classList.contains("offline")){
				document.getElementById(station + "Status").classList.remove("offline");
			}
			if(document.getElementById(station + "Status").classList.contains("online")){
				document.getElementById(station + "Status").classList.remove("online");
			}
			if(document.getElementById(station + "Status").classList.contains("busy") == false){
				document.getElementById(station + "Status").classList.add("busy");
			}
		}else if(status == "Offline"){
			if(document.getElementById(station + "Status").classList.contains("online")){
				document.getElementById(station + "Status").classList.remove("online");
			}
			if(document.getElementById(station + "Status").classList.contains("offline") == false){
				document.getElementById(station + "Status").classList.add("offline");
			}
		}
		if(document.getElementById(station + "Connection").textContent != isConnected){
			document.getElementById(station + "Connection").innerHTML = isConnected;
		}
		if(document.getElementById(station + "Status").textContent != status){
			if((status == "Idle" || status == "Open") && (prevState == "Done" || prevState == "#######")){
				document.getElementById(station + "Status").innerHTML = status;
			}
			else if(status == "Cycling" || status == "Offline"){
				document.getElementById(station + "Status").innerHTML = status;
			}
		}
		<?php if($arr[1] != "~"): ?>var model = "<?php echo $arr[1] ?>";
									
		<?php else: ?>model = "FBP-####";
					 
					   rmodel ="FBP-####";
					 
		<?php endif ?>
		if(document.getElementById(station + "Model").textContent != model && model != "" && model.length == 8){
			document.getElementById(station + "Model").innerHTML = model;
			document.getElementById(station + "rModel").innerHTML = model;			
		}
		
		<?php if($arr[2] != "~"): ?>var serialNum = "<?php echo $arr[2] ?>";
		<?php else: ?>serialNum = "########";
		<?php endif ?>
		if(document.getElementById(station + "Serial").textContent != serialNum && serialNum != "" && serialNum.length >= 6){
			document.getElementById(station + "Serial").innerHTML = serialNum;
			}
		
		<?php if($arr[3] != "~"): ?>var sysState = "<?php echo $arr[3] ?>";
		<?php else: ?>sysState = "#######";
		<?php endif ?>
		if(prevState != sysState && sysState != ""){
			document.getElementById(station + "State").innerHTML = sysState;
			document.getElementById(station + "rState").innerHTML = sysState;
		}
		<?php if($arr[4] != "~"): ?>var sTime = "<?php echo $arr[4] ?>";
		<?php else: ?>sTime = "00:00:00";
		<?php endif ?>

		if(document.getElementById(station + "STime").textContent != sTime && sTime != ""){
			document.getElementById(station + "STime").innerHTML = sTime;
		}
		<?php if($arr[8] != "~"): ?>var tTime = "<?php echo $arr[8] ?>";
		<?php else: ?>tTime = "00:00:00";
		<?php endif ?>
		if(document.getElementById(station + "TTime").textContent != tTime && tTime != ""){
			document.getElementById(station + "TTime").innerHTML = tTime;
		}
		<?php if($arr[5] != "~"): ?>temp = "<?php echo $arr[5].'° C' ?>";
		<?php else: ?>temp = "####° C";
		<?php endif ?>
		if(document.getElementById(station + "Temp").textContent != temp && temp != "° C" && temp.includes(".")){
			document.getElementById(station + "Temp").innerHTML = temp;
		}
			
		<?php if($arr[6] != "~"): ?>volt = "<?php echo $arr[6].' V' ?>";
		<?php else: ?>volt = "###### V";
		<?php endif ?>
		if(document.getElementById(station + "Voltage").textContent != volt && volt != " V" && volt.includes(".")){
			document.getElementById(station + "Voltage").innerHTML = volt;
		}
		<?php if($arr[7] != "~"): ?>curr = "<?php echo $arr[7].' A' ?>";
		<?php else: ?>curr = "###### A";
		<?php endif ?>
		if(document.getElementById(station + "Current").textContent != curr && curr != " A" && curr.includes(".")){
			document.getElementById(station + "Current").innerHTML = curr;
		}
		<?php if($tcharge != "~"): ?> tcycle = "<?php echo $tcycle ?>";
		<?php else: ?>tcycle = "00:00:00";
		<?php endif ?>
		if(document.getElementById(station + "TCycle").textContent != tcycle && tcycle != ""){
			document.getElementById(station + "TCycle").innerHTML = tcycle;
		}
		<?php if($cap != "~" && $volt != "~"): ?> cells = "<?php echo $cells ?>";
		<?php else: ?>cells = "##";
		<?php endif ?>
		if(document.getElementById(station + "Cells").textContent != cells && cells != ""){
			document.getElementById(station + "Cells").innerHTML = cells;
		}
		<?php if($cap != "~"): ?> capacity = "<?php echo $cap .' Ah' ?>";
		<?php else: ?>capacity = "## Ah";	
		<?php endif ?>				
		if(document.getElementById(station + "Cap").textContent != capacity && capacity != ""){
			document.getElementById(station + "Cap").innerHTML = capacity;
		}										
		<?php if($volt != "~"): ?> voltage = "<?php echo $volt ?>";
		<?php else: ?>voltage = "## V";
		<?php endif ?>
		if(document.getElementById(station + "Volt").textContent != voltage && voltage != "" ){
			document.getElementById(station + "Volt").innerHTML = voltage;	
		}
		<?php if($Discharge1 != "~"): ?> Discharge1 = "<?php echo $Discharge1 ?>";
		<?php else: ?>Discharge1 = "00:00:00";
		<?php endif ?>
		if(document.getElementById(station + "Discharge1").textContent != Discharge1 && Discharge1 != ""){
			document.getElementById(station + "Discharge1").innerHTML = Discharge1;
		}
		<?php if($DischargeWait1 != "~"): ?> DischargeWait1 = "<?php echo $DischargeWait1 ?>";
		<?php else: ?>DischargeWait1 = "00:00:00";
		<?php endif ?>
		if(document.getElementById(station + "DischargeWait1").textContent != DischargeWait1 && DischargeWait1 != ""){
			document.getElementById(station + "DischargeWait1").innerHTML = DischargeWait1;
		}
		<?php if($BulkCharge1 != "~"): ?> BulkCharge1 = "<?php echo $BulkCharge1 ?>";
		<?php else: ?>BulkCharge1 = "00:00:00";
		<?php endif ?>
		if(document.getElementById(station + "BulkCharge1").textContent != BulkCharge1 && BulkCharge1 != ""){
			document.getElementById(station + "BulkCharge1").innerHTML = BulkCharge1;
		}
		<?php if($ChargeWait != "~"): ?> ChargeWait = "<?php echo $ChargeWait ?>";
		<?php else: ?>ChargeWait = "00:00:00";
		<?php endif ?>
		if(document.getElementById(station + "ChargeWait").textContent != ChargeWait && ChargeWait != ""){
			document.getElementById(station + "ChargeWait").innerHTML = ChargeWait;
		}
		<?php if($Discharge2 != "~"): ?> Discharge2 = "<?php echo $Discharge2 ?>";
		<?php else: ?>Discharge2 = "00:00:00";
		<?php endif ?>
		if(document.getElementById(station + "Discharge2").textContent != Discharge2 && Discharge2 != ""){
			document.getElementById(station + "Discharge2").innerHTML = Discharge2;
		}
		<?php if($DischargeWait2 != "~"): ?> DischargeWait2 = "<?php echo $DischargeWait2 ?>";
		<?php else: ?>DischargeWait2 = "00:00:00";
		<?php endif ?>
		if(document.getElementById(station + "DischargeWait2").textContent != DischargeWait2 && DischargeWait2 != ""){
			document.getElementById(station + "DischargeWait2").innerHTML = DischargeWait2;
		}
		<?php if($BulkCharge2!= "~"): ?> BulkCharge2 = "<?php echo $BulkCharge2 ?>";
		<?php else: ?>BulkCharge2 = "00:00:00";
		<?php endif ?>
		if(document.getElementById(station + "BulkCharge2").textContent != BulkCharge2 && BulkCharge2 != ""){
			document.getElementById(station + "BulkCharge2").innerHTML = BulkCharge2;
		}
		<?php if($FinishCharge2 != "~"): ?> FinishCharge2 = "<?php echo $FinishCharge2 ?>";
		<?php else: ?>FinishCharge2 = "00:00:00";
		<?php endif ?>
		if(document.getElementById(station + "FinishCharge2").textContent != FinishCharge2 && FinishCharge2 != ""){
			document.getElementById(station + "FinishCharge2").innerHTML = FinishCharge2;
		}
		}	 							 
		rmodel ="<?php echo $model ?>";	
							
					randTime = parseInt("<?php echo $rawTime ?>");
	if(randTime % 2 == 0){
		if (getReq1 >= 2){
			var volt = "";
			var curr = "";
			<?php if($arr[6] != "~"): ?>volt = "<?php echo $arr[6].' V' ?>";
			<?php else: ?>volt = "###### V";
			<?php endif ?>
			if(document.getElementById(station + "Voltage").textContent != volt && volt != " V" && volt.includes(".")){
				document.getElementById(station + "Voltage").innerHTML = volt;
			}
			<?php if($arr[7] != "~"): ?>curr = "<?php echo $arr[7].' A' ?>";
			<?php else: ?>curr = "###### A";
			<?php endif ?>
			if(document.getElementById(station + "Current").textContent != curr && curr != " A" && curr.includes(".")){
				document.getElementById(station + "Current").innerHTML = curr;
			}
		}
		if (getReq1 >= 3){
			var temp = "";
			
			<?php if($arr[5] != "~"): ?>temp = "<?php echo $arr[5].'° C' ?>";
			<?php else: ?>temp = "####° C";
			<?php endif ?>
			if(document.getElementById(station + "Temp").textContent != temp && temp != "° C" && temp.includes(".")){
				document.getElementById(station + "Temp").innerHTML = temp;
			}
			
		}
	}
</script>
