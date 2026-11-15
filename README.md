Greninja x Iron Man Suit
A work-in-progress Iron Man suit build. The helmet is done and the rest of the suit will be added here as I build it.
The helmet is motorized and controlled by an Arduino Nano. The faceplate opens and closes on servos and the eyes light up. Greninja-inspired design.

Contents;
Progress
Helmet
Build Guide


Progress:
Part and Status
Helmet: Done (wire management tweaks left)
Rest of suit: Coming soon

Helmet:
Features
 - Servo-driven faceplate
 - LED eyes
 - Limit switch for faceplate position
 - Arduino Nano controller
Parts
 - Arduino Nano	1	
 - MG90S servo	2	(for Faceplate)
 - LED	2	(for Eyes)
 - Limit switch	1	
 - Battery	1	(5mA phone bank)

Code:
Sketches are in /code. Final version is helmet_v4.ino.
Servos are driven with writeMicroseconds() instead of write() for a wider range of motion.

To upload:
Install the Arduino IDE
Open code/helmet_v4.ino
Select Arduino Nano and your port
Upload

3D Printing:
Settings: Pla, 0.2mm, 2-3 walls, 10%-20% infill
Most .3mf files are in /models. Two were too big for GitHub

Build Guide:
The mask and mechanism follow the Frankly Built Iron Man series. It covers printing, finishing, the servo mechanism and the Arduino Nano electronics.
This repo has my wiring, code and design changes.

Credits:
Built and modified by VMAXwill. Mask build based on the Frankly Built series.
