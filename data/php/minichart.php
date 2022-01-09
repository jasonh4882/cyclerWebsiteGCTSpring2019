<head>
	<script src="https://cdn.zingchart.com/zingchart.min.js"></script>

</head>	
	<div id="mychart" class="chart--container">
	<script>
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
		
$serverName = "GCTUSSERVER\\CYCLERDATA, 1433";								
$connectionInfo = array( "Database"=>"CYCLERDATABASE", "UID"=>"user_dbf", "PWD"=>"CyclerData");
$conn = SQLSRV_CONNECT( $serverName, $connectionInfo);
if ($conn === false)
{
	echo "could not connect";
	die(print_r(SQLSRV_ERRORS(),true));
}
$model = "FBP1157";
$serial = "19080003";
if ($arr[1] != "")
{$model = $arr[1];}
if ($arr[2] != "")
{$serial = $arr[2];}
$query="select top 10 temp, curr, volt from battery_data t1 join state_val_lookup t2
on t1.state = t2.state
where model = 
'".$model."' and serial = '".$serial."' and temp < 200
and t1.state = any (select state from STATE_VAL_LOOKUP) 
order by cycle, stval";
$stmt=SQLSRV_QUERY($conn,$query);
?>

ZC.LICENSE = ["569d52cefae586f634c54f86dc99e6a9", "b55b025e438fa8a98e32482b5f768ff5"]; // window.onload event for Javascript to run after HTML
        // because this Javascript is injected into the document head
        window.addEventListener('load', () => {
            // Javascript code to execute after DOM content
 
            // full ZingChart schema can be found here:
            // https://www.zingchart.com/docs/api/json-configuration/

			
			var Temps=[<?php
			while( $info = SQLSRV_FETCH_ARRAY($stmt,SQLSRV_FETCH_ASSOC))
				 echo $info['temp'].',';?>];
			 <?php
			 $query="select TOP 10 temp, curr, volt from battery_data t1 join state_val_lookup t2
on t1.state = t2.state
where model = 
'".$model."' and serial = '".$serial."' and temp < 200
and t1.state = any (select state from STATE_VAL_LOOKUP) 
order by cycle, stval";
$stmt=SQLSRV_QUERY($conn,$query);
			?>
			var Currs=[<?php
			while( $info = SQLSRV_FETCH_ARRAY($stmt,SQLSRV_FETCH_ASSOC))
				 echo $info['curr'].',';?>];
			 <?php
			 $query="select TOP 10 temp, curr, volt from battery_data t1 join state_val_lookup t2
on t1.state = t2.state
where model = 
'".$model."' and serial = '".$serial."' and temp < 200
and t1.state = any (select state from STATE_VAL_LOOKUP) 
order by cycle, stval";
$stmt=SQLSRV_QUERY($conn,$query);
			?>
			 var Volts=[<?php
			while( $info = SQLSRV_FETCH_ARRAY($stmt,SQLSRV_FETCH_ASSOC))
				 echo $info['volt'].',';?>];
				
			<?php
			SQLSRV_CLOSE($conn);
			?>

		const myConfig = {
                type: 'line',
                title: {
                    text: '',
                    fontSize: 24,
                },
                legend: {
                  layout: '1x3',
				  
					
                },
                scaleX: {
                    // set scale label
                    label: {
                        text: 'Time (minutes)'
                    }
                    
                },
                scaleY: {
                    // scale label with unicode character
                    label: {
                        text: 'Data'
                    }
                },
                plot: {
                    // animation docs here:
                    // https://www.zingchart.com/docs/tutorials/design-and-styling/chart-animation/#animation__effect
                   
                },
                series: [
						{
                        // plot 1 values, linear data
                        values: Temps,
						text: 'Temp (C)',
						},
						{
						values: Currs,
						text:  'Current (A)',
						},
						{
						values: Volts,
						text: 'Volts (V)',
						}
				]
            };
 
            // render chart with width and height to
            // fill the parent container CSS dimensions
            zingchart.render({
                id: 'mychart',
                data: myConfig,
                height: 'auto',
                width: 'auto',
				output: 'canvas'
            });
        });
    </script>
	</div>

	