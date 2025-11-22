# Linear Actuator For Laser Receiver

## Custom Sensor Array vs Standard Laser Receiver on Linear Actuator
A Custom CCD or photodiode array is going to be too expensive, time consuming, and it might not work with every laser. 

On the Job, we are already moving the stick up and down, and tilting it to get the laser receiver to beep.

I propose we just hold the stick still, and have a linear actuator move the standard laser receiver until it beeps. We can tie into the laser receivers speaker wires and use that to position it perfectly. Once it's positioned, we can give the user the measurement.


### Linear Actuator
### Pros
- Cheap
- Simple to program
- Technology already exists
- Can be expanded to any length cheaply
- Can mount any laser receiver

### Cons
- Bulky
- Heavier
- Requires more power
- The physical size of the laser receiver affects the total scan area. And user needs to set the max extension so that it doesn't crash the laser receiver into the enclosure.
- Scanning is slower. 1ft per second is acceptable
  - Scanning speed largely depends on how quickly the laser receiver can detect the laser. 
- The UI is more complicated. (Calibrate, Jog, "Home", Scan)
- Different laser receivers have different speakers and signals
- If the laser receiver moves up and down, we either need to listen for the (fast, solid, slow) beeps using a microphone, or tap into the speaker wires.
- If we tap into the speaker wires, we will need to make a little radio to transmit the speaker signals to our esp32. The laser receiver will be moving up and down, we cant use a wire, it would snag or break.

# Build requirements
- Needs to scan quickly. 1ft per second is a good speed.
- Needs to move a 300g laser reeciver up and down.
- No creep. If bumped, powered off, etc. Belt drive probably wont work.
- 2 to 3 ft scan area
- Total weight under 5 pounds. Loose requirement.
- Needs to work in a dusty, hot environment.
- Encoder on stepper motor for exact position
- Maybe boost convertor for higher speeds

# Laser Receiver Dimensions
- Topcon LS-80L (NEW Model)
  - x in (Tall) | x in (Wide) | x in (Thick) | x grams (Weight)
- Topcon LS-70B (OLD Model)
  - 6.5 in (Tall) | 3 in (Wide) | 1.05 in (Thick) | 220 grams (Weight)
 

# Parts List
- [$50 | StepperOnline | ESS17-07](https://www.omc-stepperonline.com/ess-series-0-60nm-85oz-in-nema-17-integrated-closed-loop-stepper-servo-motor-24-36vdc-1000cpr-ess17-07)
  - [3D Model .IGS](https://www.omc-stepperonline.com/index.php?route=product/product/get_file&file=1266/ESS17-07.IGS)
  - Input Voltage: 24 - 36VDC
  - Holding Torque: 0.6Nm(85oz.in)
  - Weight: 1.2 lbs (531g)
  - Body Length: 2.36in (60mm)
  - Logic Input Voltage: 5V but will likely work with 3.3v
  - There is a smaller version, 0.25 lbs lighter and 1/2 inch shorter. 🔥 It has less low end torque, but better torque at higher sppeds 🔥 [ESS17-04](https://www.omc-stepperonline.com/ess-series-0-48nm-68oz-in-nema-17-integrated-closed-loop-stepper-servo-motor-24-36vdc-1000cpr-ess17-04)
- [$160 | Makita 40V XGT 2.5Ah](https://a.co/d/japQ2qb)
  - 1.7 lbs (0.77 kg)
  - needs a switching voltage regulator to pull down to 36V
- Makita 40V Adapter Plate 
