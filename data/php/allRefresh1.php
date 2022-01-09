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
	?>
	var getReq1 = parseInt("<?php echo $fget[0] ?>");
	var station = "<?php echo $fget[1] ?>";
	station = ("s" + station);
	if (getReq1 >= 1){
		var randTime = "";
		var isConnected = "";
		var status = "";
		
		var serialNum = "";
		var sysState = "";
		var sTime = "";
		var tTime = "";
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
		<?php endif ?>
		if(document.getElementById(station + "Model").textContent != model && model != "" && model.length == 8){
			document.getElementById(station + "Model").innerHTML = model;
		}
		<?php if($arr[2] != "~"): ?>var serialNum = "<?php echo $arr[2] ?>";
		<?php else: ?>serialNum = "########";
		<?php endif ?>
		if(document.getElementById(station + "Serial").textContent != serialNum && serialNum != "" && serialNum.length == 8){
			document.getElementById(station + "Serial").innerHTML = serialNum;
		}
		<?php if($arr[3] != "~"): ?>var sysState = "<?php echo $arr[3] ?>";
		<?php else: ?>sysState = "#######";
		<?php endif ?>
		if(prevState != sysState && sysState != ""){
			document.getElementById(station + "State").innerHTML = sysState;
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
	}
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