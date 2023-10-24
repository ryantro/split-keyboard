# split-keyboard

I chose this as my project for [ECE 561 - Hardware/Software Design of Embedded Systems](https://www.online.colostate.edu/courses/ECE/ECE561.dot)

The project is a split mechanical keyboard built from scratch (and a microprocessor development board).

The noteable features:
- Split halves: Communicate via an I2C bus. The left half is the master device and the left half is the slave device.
- Profile selection: Special keys change the profile of the keyboard, changing the functions of certain keys and the joystick.
- LCD screen: Indicates the currently active profile.
- Joystick: Can act as either a mouse or a joystick depending on profile.

![img](https://github.com/ryantro/split-keyboard/blob/main/Pictures/20210506_090743.jpg)

# Tools

### KiCAD

Schematic and PCB layout designed in KiCAD.

![img](https://github.com/ryantro/split-keyboard/blob/main/Pictures/PCB_KiCAD.png)

PCBs ordered from [https://www.pcbway.com/](https://www.pcbway.com/)

![img](https://github.com/ryantro/split-keyboard/blob/main/Pictures/PCB.jpg)

### Teensyduino

Firmware written in Teensyduino IDE.

![img](https://github.com/ryantro/split-keyboard/blob/main/Pictures/Code.png)

# Refrences

[https://github.com/ruiqimao/keyboard-pcb-guide](https://github.com/ruiqimao/keyboard-pcb-guide)

[https://www.pjrc.com/teensy/teensyduino.html](https://www.pjrc.com/teensy/teensyduino.html)

