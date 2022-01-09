<!DOCTYPE html>

<html>
 

<head >
<title>Cycler Data: Battery Data</title>
		<link rel = "stylesheet" type = "text/css" href = "../styles/styles.css" />
		<link rel = "icon" type = "image/ico" href = "../Images/icon.ico" />
		
		 
   <script language="javascript" type="text/javascript" src="https://ajax.googleapis.com/ajax/libs/jquery/3.4.0/jquery.min.js"></script>
<link rel="stylesheet" type="text/css" href="//cdn.datatables.net/1.10.19/css/jquery.dataTables.min.css" />
    <script src="//cdn.datatables.net/1.10.19/js/jquery.dataTables.min.js"></script>
	<script src="../Scripts/moment.min.js"></script>
	<script src="..//scripts/jquery-dateformat.min.js"></script>
</head>
   <script type ="text/javascript">
   $(document).ready(function() {
	   
		
            $.ajax({
                url: 'phpbase.php',
				type: 'post',
                dataType: 'json',
                success: function (data) {
					$('#datatable').DataTable
					({
						
                        data: data,
                        "columns": 
						[
                            {
                                "data": 'Battery',
								'render': function(	battery){
									return '<a href=' +"batterylink.php?battery="+battery+' target="_blank">' + battery
									}
							},
						    {
                                "data": 'Model'},
                            {
                                "data": 'Serial'},
                            {
							 "data" : 'DateCycled',
							 
							 //'sortable' : false,
							 'render': function(jsonDate){
							 
							 var json = JSON.stringify(jsonDate);
							   var date = new Date(parseInt(jsonDate));
							   var month = date.getMonth() + 1;
							 var year = date.getFullYear();
							
							
														
							return $.format.date(date,"  yyyy/MM/dd HH:mm");
				
							
							
							 }
							}								
						]
					});
                }
            });
        });
        </script>   
    <body style="font-family: Arial">   
							 
	<table style="width:100%">
			<tr>
				<td class="sideTd" rowspan=2></td>
				<td class="sideTd" id="navBar" rowspan=2>
					<a href="https://greencubestech.com/">
						<img src="../Images/greencubeslogo.png" alt="green cubes logo" width="100"/>
					</a>
					<br/>
					<br/>
					<br/>
					<br/>
					<lable><a class="navLink" href="../index.htm">Overview</a></label><br/>
					<lable><a class="navLink stationNav" href="station.php?station=1">Station 1</a></label><br/>
					<lable><a class="navLink stationNav" href="station.php?station=2">Station 2</a></label><br/>
					<lable><a class="navLink stationNav" href="station.php?station=3">Station 3</a></label><br/>
					<lable><a class="navLink stationNav" href="station.php?station=4">Station 4</a></label><br/>
					<lable><a class="navLink stationNav" href="station.php?station=5">Station 5</a></label><br/>
					<lable><a class="navLink stationNav" href="station.php?station=6">Station 6</a></label><br/>
					<lable><a class="navLink" href="entry.php">Remote Start</a></label><br/>
					<lable><a class="navLink" href="lookup.php">Battery Lookup</a></label><br/>
							<a class="navLink" href="newbattery.php">New Model...</a></label><br/>
							<a class="navLink" href="upload.php">Upload Data...</a></label><br/>
					<lable><a class="navLink" href="about.php">About</a></label><br/>
				</td>
				<td>
					<div id="cyclerHeading">Battery Database Lookup</div>
					</div>
				</td>
		<td></td>

			</tr>
<tr>
				<td class="centerTd">
					      
		  <table id ="datatable">
           <thead>
               <tr>
                    <th>Battery</th>
				   <th>Model</th>
                   <th>Serial</th>
                   <th>DateCycled</th>
               </tr>
           </thead>
           <tfoot>
	
               <tr>
				   <th>Battery</th>
                   <th>Model</th>
                   <th>Serial</th>
                   <th>DateCycled</th>
               </tr>
           </tfoot>
		   
       </table>

    <td class="sideTd"></td>

</body>

</html>
