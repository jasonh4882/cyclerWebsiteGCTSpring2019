<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="WebForm.aspx.cs" Inherits="BatteryWebApp.WebForm" %>

<!DOCTYPE html>

<html>
<head runat ="server">
        <title>Battery Table Lookup</title>
    <script src="https://ajax.googleapis.com/ajax/libs/jquery/3.4.0/jquery.min.js"></script>
    <link rel="stylesheet" type="text/css" href="//cdn.datatables.net/1.10.10/css/jquery.dataTables.min.css" />
    <script src="//cdn.datatables.net/1.10.10/js/jquery.dataTables.min.js"></script>
    <script type ="text/javascript">
        $(document).ready(function () {
            $.ajax({
                url: 'BatteryService.asmx/GetBatteries',
                method: 'post',
                dataType: 'json',
                success: function (data) {
                    $('#datatable').dataTable({
                        data: data,
                        columns: [
                            {
                                'data': 'Model'},
                            {
                                'data': 'Serial'},
                            {
                                'data': 'DateCycled',
                                'render': function (jsonDate) {
                                    var date = new Date(parseInt(jsonDate.substr(6)));
                                    var month = date.getMonth() + 1;
                                    if (date.getFullYear() < 2018) { return "Not Available"; }
                                    else {
                                        return month + "/" + date.getDate() + "/" + date.getFullYear();
                                    }
                                   
                                }

                            }
                        ]
                    });
                }
            });
        });
        </script>
</head>
    <body style="font-family: Arial">
    <form id="form1" runat="server">
       <table id ="datatable">
           <thead>
               <tr>
                   <th>Model</th>
                   <th>Serial</th>
                   <th>DateCycled</th>
               </tr>
           </thead>
           <tfoot>
               <tr>
                   <th>Model</th>
                   <th>Serial</th>
                   <th>DateCycled</th>
               </tr>
           </tfoot>
       </table>
    </form>
</body>
</html>

