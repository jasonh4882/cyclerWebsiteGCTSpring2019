
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
	'".$model."' and serial = ".$serial." and temp < 200
	and q.state = any (select state from STATE_VAL_LOOKUP where state like 
	'BulkCharge1' or state like '".$state2."' or state like '".$state3."')";
	$stmt=SQLSRV_QUERY($conn,$query);
			?>
			var Currs=[<?php
			while( $info = SQLSRV_FETCH_ARRAY($stmt,SQLSRV_FETCH_ASSOC))
				 echo $info['curr'].',';?>];
			 <?php
"SELECT [Model]
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
			 var Volts=[<?php
			while( $info = SQLSRV_FETCH_ARRAY($stmt,SQLSRV_FETCH_ASSOC))
				 echo $info['volt'].',';?>];
				
			<?php
			SQLSRV_CLOSE($conn);
			?>

		const myConfig = {
				
                backgroundColor: '#FFFFFF',
				alpha: 0.8,
				
                type: 'line',
                title: {
                    text: ' <?php echo $battery ?> Battery Data: <?php echo $state.', '.$state2?>',
					fontFamily: 'Impact',
                    fontSize: 30,
					color: 'white',
					
					backgroundColor: ' #252B2D',
					alpha: 0.9,
                },
                legend: {
                    draggable: true,
					alpha: 0.7,
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
						text: 'Temperature (C)',
						},
						{
						values: Currs,
						text:  'Current (A)',
						},
						{
						values: Volts,
						text: 'Voltage (V)',
						}
				]
            
			};
		
            // render chart with width and height to
            // fill the parent container CSS dimensions
            zingchart.render({
				
                id: 'myChart',
                data: myConfig,
                height: '100%',
                width: '100%'
            });
		
        });
 