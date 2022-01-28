# GCT Industrial Battery Cycler & Cycler Website #

## Description ##

The Green Cubes Technology Industrial Battery Cycler was first worked on in the summer of 2018 and was finished by the following spring. It's goal was to take industrial grade Lithium Ion batteries, discharge them to empty, then charge them back to full again. This process is integral to the QC process for the batteries, as it verifies that the batteries are able to hit their rated output and have no issues doing so. As a next step, a website was created to remotely monitor the machine.

This battery cycler had many design flaws that prevented it from being all what it was meant to be, the biggest issue being it's inability to cycle any battery with a rated voltage higher than 48v. A new, 80v maximum cycler was created, making the first machine obsolete. This lead to the 48v maximum cycler to eventually be decommissioned and scrapped out for other projects.

As a result, the script files and website directory that were used for the project are no longer in production. While the original cycler may no longer exist in a functional form, many of the finer details must be left out, specifically on the hardware used. The version of the software available, *Cycler-48v-r5.1.0*, is very outdated, containing very little of the source code that is used in the 80v cycler. 

The cycler website was handled on-site by a server computer that would store the data inside an instance of SQL Server. Through a mixture of *PHP* and *JavaScript*, data was accurately updated in real-time for all 6 stations with less than 10 seconds of delay. This was accomplished primarily by files *station.php* as the primary display page and *allRefresh.php* for the backend data fetching. Both of these can be found in [/pages](/pages) and in [/data/php](/data/php) respectively.

 This repository is for storage of these files that may be used for reference at a later date.

## Hardware Used ##

6 separate cycler stations were setup. Past using *Arduino Mega* and *ESP32 microcontrollers* to control each of these stations, the hardware used to build the cycler machines can not be explicitly disclosed. 

    
## Software ##

- The site was hosted on a local server through [IIS](https://www.iis.net/) using a Windows Server 2016 machine.
- The cycler(s) run on Arduino Mega microcontrollers. The 
[Arduino IDE](https://www.arduino.cc/en/software "Arduino IDE Download page") was used to write and upload sketch files to the SoCs.

### Cycler-48v-r5.1.0.ino Arduino Sketch ###

Required Libraries:  
Elegoo_GFX.h-----------*Graphics*  
Elegoo_TFTLCD.h-------*Graphics*  
TouchScreen.h----------*Digitizer*  
SD.h---------------------*SPI Bus*  
Wire.h-------------------*I2C Bus*  
Adafruit_ADS1015.h----*ADC*  
Adafruit_MCP4725.h----*DAC*  
max6675.h--------------*Thermocouple*  

*15 methods* exist for drawing UI elements to the attached TFT screen.  
*10 methods* exist for receiving user input from the drawn UI.  
A method called *setup* is called first. After *setup* finishes, a method called *loop* will be called. When *loop* finishes, *loop* will be called again. This will repeat indefinitely.  
The system can exist in 5 distinct states:  
- **Start**
- **Discharge**
- **Discharge Wait**
- **Charge**
- **Charge Wait**

These states can be described as follows:
- **Start** is for preparing the system. Information is entered by a user so that a battery is cycled to it's specific specifications.
- **Discharge** is for removing charge from the battery. This is done by connecting the battery to sets of resistors to reach a specific power output.
- **Discharge Wait** is for after the battery is fully discharged. To prevent damage, the battery must wait for a period of time for the internal cells to relax.
- **Charge** is for adding charge to the battery. This is done by connecting the battery to a power supply set to output power based on the size of battery. Charge is done in two steps, the first of which pushes a large amperage output to the battery. Once the battery nears full charge, the amperage is lowered significantly to be sure the battery reaches it's maximum capacity.
- **Charge Wait** is for after the battery is fully charged. To prevent damage, the battery must wait for a period of time for the internal cells to relax. After the waiting period finishes, the system starts back over at Discharge and ending after Charge. This is done so that a discharge all the way from full is completed, as the individual cells of a battery arrive not fully charged.

A full run of the machine will consist of the following states: *Start, Discharge, Discharge Wait, Charge, Charge Wait, Discharge(2), Discharge Wait(2), Charge(2)*.  
During the cycling process, various measurements, such as amperage, voltage, temperature, and time spent in each state would be taken. These measurements would then be displayed to the user on the UI as well as being sent across a UART connection to an *ESP32 microcontroller*. The *ESP32* would then handle uploading the measurements to the server. The program for the *ESP32 microcontroller* is unavailable.  
The specifics of how all of this comes about cannot be discussed more specifically than this. 

## Installation ##

1. Download and install the Arduino IDE 
[here](https://www.arduino.cc/en/software "Arduino IDE Download page")
1. Download the *Cycler-48v-r5.1.0.ino* file and place it in a folder called *Cycler-48v-r5.1.0*
 inside the Arduino sketch folder (*this by default is located in "Documents"*)
1. Connect an Arduino Mega to the computer via a USB A to USB B cable.
1. Verify that the device is connected by checking to see if a new Serial device appeared in the 
 **Tools>Port** Navigation menu. Select the new device.
1. Click the **Verify** checkmark button located under **File**.
 If there are no errors and the sketch successfully verifies, click the **Upload** button.
1. Open the serial monitor located in either the **Tools** navigation menu 
or by clicking the **Serial Monitor** magnifying glass button in the far left

## Imagery ##

### Cycler Main Unit ###

![cyclerUnit](/cyclerImages/IMG_3157.JPEG)

### Resistor Temperature Test ###

![resistorHot](/cyclerImages/20200616T170155.jpg)

## Roadmap ##

After employment had concluded, the project was handed off to an internal GCT engineer team to continue work. This repository will see no further updates.

## Authors & Acknowledgments ##

- The battery cycler and the corresponding website were created for [Green Cubes Technology](https://greencubestech.com/)
- The original cycler, capable of cycling up to 48v batteries, was constructed and programmed by Jason Hampshire and Darin Kiefer(<dkiefer@greencubestech.com>).
- The next generation cycler, capable of cycling up to 80v batteries, was constructed and is maintained by an internal engineering team at Green Cubes Technology in Kokomo, Indiana.

## Project Status ##

This project is **Retired** and will continue to see work done internally by Green Cubes Technology.