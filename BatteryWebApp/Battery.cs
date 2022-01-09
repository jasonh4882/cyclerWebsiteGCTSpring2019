using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;

namespace BatteryWebApp
{ 
    public class Battery
    {
        public string Model { get; set; }
        public string Serial { get; set; }
        public DateTime DateCycled { get; set; }

    }
}