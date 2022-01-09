<?php $get = filter_input_array(INPUT_GET);
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
?>
<?php //server connection
$serverName = "GCTUSSERVER\\CYCLERDATA, 1433";								
$connectionInfo = array( "Database"=>"CYCLERDATABASE", "UID"=>"user_dbf", "PWD"=>"CyclerData");
$conn = SQLSRV_CONNECT( $serverName, $connectionInfo);
if ($conn === false)
{
	echo "could not connect";
	die(print_r(SQLSRV_ERRORS(),true));
}
$model = "~";
$serial = "~";
if ($arr[1] != "")
{$model = $arr[1];
$model =str_replace('-', '', $model);}
if ($arr[2] != "")
{$serial = $arr[2];}
$query="select row = row_number() over ( order by tTime), temp from battery_data t1 join state_val_lookup t2
on t1.state = t2.state
where model = 
'".$model."' and serial = '".$serial."' and temp < 200
and t1.state = any (select state from STATE_VAL_LOOKUP) 
order by row desc";
$stmt=SQLSRV_QUERY($conn,$query);
?>
<script> // variables
var canvas= ['ChartS0','ChartS1','ChartS2','ChartS3','ChartS4','ChartS5','ChartS6','Chart'];
var getReq2 = parseInt("<?php echo $fget[0] ?>");
var station = parseInt("<?php echo $fget[1] ?>");
	
</script> 
<head> <!-- scripts and css -->
<script src="Scripts/Chart.js"></script>
<link rel = "stylesheet" type = "text/css" href = "styles/Chart.css" />
</head>
<canvas id= canvas[station] width="800" height="400"></canvas>
<script>
if (getReq2 >= 1){
			
			var Temps=[<?php
			while( $info = SQLSRV_FETCH_ARRAY($stmt,SQLSRV_FETCH_ASSOC))
				 echo $info['temp'].',';?>];
			 Temps.reverse();
			  <?php
			 $query="select row = row_number() over ( order by tTime), curr from battery_data t1 join state_val_lookup t2
on t1.state = t2.state
where model = 
'".$model."' and serial = '".$serial."' and t1.state = any (select state from STATE_VAL_LOOKUP) 
order by row desc";
$stmt=SQLSRV_QUERY($conn,$query);
			?>
			
			var Currs=[<?php
			while( $info = SQLSRV_FETCH_ARRAY($stmt,SQLSRV_FETCH_ASSOC))
				 echo $info['curr'].',';?>];
			  Currs.reverse();
			 <?php
			 $query="select row = row_number() over ( order by tTime), volt from battery_data t1 join state_val_lookup t2
on t1.state = t2.state
where model = 
'".$model."' and serial = '".$serial."' and t1.state = any (select state from STATE_VAL_LOOKUP) 
order by row desc";
$stmt=SQLSRV_QUERY($conn,$query);
			?>
			 var Volts=[<?php
			while( $info = SQLSRV_FETCH_ARRAY($stmt,SQLSRV_FETCH_ASSOC))
				 echo $info['volt'].',';?>];
			Volts.reverse();
<?php
			 $query="select row = row_number() over ( order by tTime), tTime from battery_data t1 join state_val_lookup t2
on t1.state = t2.state
where model = 
'".$model."' and serial = '".$serial."' and t1.state = any (select state from STATE_VAL_LOOKUP) 
order by row desc";
$stmt=SQLSRV_QUERY($conn,$query);	
?>  var Rows=[<?php
			while( $info = SQLSRV_FETCH_ARRAY($stmt,SQLSRV_FETCH_ASSOC))
				 echo $info['tTime'].',';?>];
			Rows.reverse();		
			<?php
			SQLSRV_CLOSE($conn);
			?>


<?php if($arr[1] != "~"): ?>	
var ctx = document.getElementById(canvas[station]);
 canvas[station] = new Chart(ctx, {
  type: 'line',
  data: {
  labels: Rows,
    datasets: [
      { 
        data: Temps,
		fill: false,
		label: "Temperature",
		borderColor: "red",
		
		
      },
	  { 
        data: Currs,
		fill: false,
		label: "Current",
		borderColor: "green"
		
      },{ 
        data: Volts,
		fill: false,
		label: "Voltage",
		borderColor: "blue"
		
      },
    ]
  },
   options: {
				
				
				animation: false,
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
				elements: { 
					  point: { 
						radius: 0,
						hitRadius: 10, 
						hoverRadius: 5,
							 } 
						  },
				scales: {
					  xAxes: [{
                ticks: {
                    // Include a dollar sign in the ticks
                    callback: function(value, index, values) {	
					var newval = parseInt(value/60);					
						while (newval % 60 > 9)
                        {return parseInt(newval/60)+":"+ newval % 60;}
						 return parseInt(newval/60)+":0"+ newval % 60;
                    }
                }
            }]
						   }			  
			}
});
			

}
<?php endif; ?>		
</script>
