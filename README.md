# Linear Actuator For Laser Receiver

# Todo
- Order a linear rail
- Order 2mm right angle pcb connector and male connector w wires
- Order M18 adapter
- Order or make a 5mm to 3/8 coupler
- Order limit switches or something like them
- Order Al extrusion and bolts and hardware to attach things
- Order a bearing? to support the end of the lead screw and 3d print that structure
- Model and 3d print a carriage that connects the linear rail to the lead screw nut.
- 3d print a qd mount for the laser receiver

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
- $71 | [StepperOnline | ESS17-04](https://www.omc-stepperonline.com/ess-series-0-48nm-68oz-in-nema-17-integrated-closed-loop-stepper-servo-motor-24-36vdc-1000cpr-ess17-04)
  - [3D Model .IGS](https://www.omc-stepperonline.com/index.php?route=product/product/get_file&file=1265/ESS17-04.IGS)
  - Input Voltage: 24 - 36VDC
  - Weight: 419g
  - [SpecSheet](file:///Users/markfaulkner/Downloads/ESS17-04_07_Manual.pdf)
- $83 | [M18 HIGH OUTPUT CP3.0](https://www.milwaukeetool.com/products/48-11-1835)
  - Weight: 595g
- $8 | [M18 Adapter | Amazon](https://a.co/d/7E8KwFV)
  - Weight: na
- $57 | [300W Boost Converter | I7C4W008A120V-001-R | Digikey](https://www.digikey.com/en/products/detail/tdk-lambda/I7C4W008A120V-001-R/11697408)
  - Weight: 25g, 1.5 x 1.5 x 0.5 inches
  - Output Voltage: 9.6 to 48V
  - [SpecSheet](https://product.tdk.com/en/search/power/switching-power/dc-dc-converter/info?part_no=i7C4W008A120V-001-R)
- $40 | [Roton Torqspline Lead Screw, 3/8 X 1.000](https://www.roton.com/product/torqspline-steel-screw-right-60998/)
  - Weight: 368g for all 36 inches
- $20 | [Roton Torqspline Flange Nut, 3/8 X 1.000, RH, Plastic](https://www.roton.com/product/torqspline-plastic-flange-nut-right-92054/)
  - Weight: na
- $ | []()
  - Weight: na
- $ | []()
  - Weight: na
- $ | []()
  - Weight: na

