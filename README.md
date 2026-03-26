# Project: Motorized Clock
-----

**If you haven't used a *Unipolar Stepper Motor* before**, please follow [this tutorial](https://github.com/kingston-hackSpace/Motors/blob/main/STEPPERmotors.md) before working on the clock. 

-----
## PROJECT DESCRIPTION

Make a 60-mins clock using a Unipolar Stepper Motor. Add gears to set hours.  

Time setting via manual button pressing (+) and (-). Confirm time by pressing both buttons simultaneously. 

Continuous power via 5V 1A power supply (connected to mains power). Alternatively, a 5V 1A battery bank can be used, with expected runtime is approximately 18–20 hours. For longer continuous operation, a mains power supply is recommended.

-----
## HARDWARE

- Arduino UNO

- Unipolar Stepper Motor

- ULN2003 Motor Driver

- 5V 1A Power Supply

- push buttons (x2)

- 100µF capacitor (x1)

- Gears (pending details)

-----
## WIRING

See [wiring diagram here](https://github.com/kingston-hackSpace/Project_Motorized-Clock/blob/main/Unipolar_buttons_bb.jpg)

-----
## CODE and INSTRUCTIONS

- Upload [this code](https://github.com/kingston-hackSpace/Project_Motorized-Clock/blob/main/60min-stepperClock.ino) to your Arduino Board 

- To set the time, press the buttons. Each pressing corresponds to one minute. To confirm the current time, press both buttons simultaneously. You can see it at the serial monitor.

-----
## ALTERNATIVE: 10 Min Clock

Add: LEDs(x2), resistors 220ohms (x2)

- Upload [this code](https://github.com/kingston-hackSpace/Project_Motorized-Clock/blob/main/10Min-Clock.ino) to your Arduino Board.

- This new code adds LEDs to monitor when the current time has been sucessfully set up, as well as monitoring LED blinking as second intervals.

- CODE Description: A full stepper motor rotation is now euivalent to 6 sections of 10min. In other words, trigger the motor every 10 mins to complete 1 hr (6 sections.)
  
-----
## EXAMPLES

[3D Printed Holo Clock With Arduino](https://www.instructables.com/3D-Printed-Holo-Clock-With-Arduino/)

[Two different time zones clock](https://blog.arduino.cc/2020/08/05/this-stepper-driven-mechanical-clock-can-be-set-to-two-different-time-zones/)

[Building a Stepper Motor Clock with a Custom PCB](https://www.youtube.com/watch?v=tWS7qVi3ZdI)
