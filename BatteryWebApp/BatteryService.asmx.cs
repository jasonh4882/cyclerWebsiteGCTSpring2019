using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.Services;
using System.Configuration;
using System.Data;
using System.Data.SqlClient;
using System.Web.Script.Serialization;
using System.Web.Optimization;

namespace BatteryWebApp
{
    /// <summary>
    /// Summary description for BatteryService
    /// </summary>
    [WebService(Namespace = "http://tempuri.org/")]
    [WebServiceBinding(ConformsTo = WsiProfiles.BasicProfile1_1)]
    [System.ComponentModel.ToolboxItem(false)]
    // To allow this Web Service to be called from script, using ASP.NET AJAX, uncomment the following line. 
    [System.Web.Script.Services.ScriptService]
    public class BatteryService : System.Web.Services.WebService
    {

        [WebMethod]
        public void GetBatteries()
        {
            string cs = "Data Source=192.168.1.5;Initial Catalog=CYCLERDATABASE;User Id=user_dbf;Password=CyclerData" providerName = "System.Data.SqlClient";
            List<Battery> Batteries = new List<Battery>();
            using (SqlConnection con = new SqlConnection(cs))
            {
                SqlCommand cmd = new SqlCommand("spGetBBattery", con);
                cmd.CommandType = CommandType.StoredProcedure;
                con.Open();
                SqlDataReader rdr = cmd.ExecuteReader();
                while (rdr.Read())
                {
                    DateTime value = DateTime.Today;
                    Battery battery = new Battery();
                    battery.Model = rdr["Model"].ToString();
                    battery.Serial = rdr["Serial"].ToString();
                    battery.DateCycled = (DateTime)(rdr["DateCycled"]);
                    Batteries.Add(battery);

                }

            }
            JavaScriptSerializer js = new JavaScriptSerializer();
            Context.Response.Write(js.Serialize(Batteries));

        }
    }
}
