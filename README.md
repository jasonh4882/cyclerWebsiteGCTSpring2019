# GCT Industrial Battery Cycler & Cycler Website

## Description

The Green Cubes Technology Industrial Battery Cycler was first worked on in the summer of 2018 and was finished by the following spring. It's goal was to take industrial grade Lithium Ion batteries, discharge them to empty, then charge them back to full again. This process is integral to the QC process for the batteries, as it verifies that the batteries are able to hit their rated output and have no issues doing so. As a next step, a website was created to remotely monitor the machine.

This battery cycler had many design flaws that prevented it from being all what it was menat to be, the biggest issue being it's inability to cycle any battery with a rated voltage higher than 48v. A new, 80v maximum cycler was created, making the first machine obsolete. This lead to the 48v maximum cycler to eventually be decomissioned and scrapped out for other projects.

As a result, the script files and website directory that were used for the project are no longer used. This repository is for storage of these files that may be used for reference at a later date.

## Hardware Used

Past using Arduino Mega microcontrollers, the hardware used to build the cycler machines can not be disclosed.
    
## Software

- The site was hosted on a local server through [IIS](https://www.iis.net/) using a Windows Server 2016 machine.
- The cycler(s) run on Arduino Mega microcontrollers. The 
[Arduino IDE](https://www.arduino.cc/en/software "Arduino IDE Download page") was used to write and upload sketch files to the SoCs.

## Installation

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
or by clicking the **Serial Monitor** magifying glass button in the far left

## Imagery

### Cycler Main Unit ###

![cyclerUnit](/cyclerImages/IMG_3157.JPEG)

### Resistor Temperature Test ###

![resistorHot](/cyclerImages/20200616T170155.jpg)

## Roadmap

After employment had concluded, the project was handed off to an internal GCT engineer team to continue work. This repository will see no further updates.

## Authors & Acknowledgments

- The battery cycler and the corresponding website were created for [Green Cubes Technology](https://greencubestech.com/)
- The original cycler, capable of cycling up to 48v batteries, was constructed and programmed by Jason Hampshire and Darin Kiefer(<dkiefer@greencubestech.com>).
- The next generation cycler, capable of cycling up to 80v batteries, was constructed and is maintained by an internal engineering team at Green Cubes Technology in Kokomo, Indiana.

## Project Status

This project is **Retired** and will continue to see work done internally by Green Cubes Technology.