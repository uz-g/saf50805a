<h1 align="center">50805a.</h1>

<p align="center">
    <a href="https://github.com/uz-g/saf50805a/pulse"><img src="https://img.shields.io/github/last-commit/uz-g/saf50805a?style=for-the-badge&logo=github&color=7dc4e4&logoColor=D9E0EE&labelColor=302D41"></a>
    <a href="https://github.com/uz-g/saf50805a/stargazers"><img src="https://img.shields.io/github/stars/uz-g/saf50805a?style=for-the-badge&logo=apachespark&color=eed49f&logoColor=D9E0EE&labelColor=302D41"></a>
</p>
this project implements a control system for a VEX V5 robot using pros and lemlib.

## features

- dual joystick tank drive
- autonomous mode selection via lvgl gui
- intake control system
- pneumatic systems for mobile goal clamp and intake positioning
- temperature monitoring for drivetrain and intake motors
- reverse drive functionality
- match timer with controller rumble alert
- and many more driver convenience features

## hardware configuration

- drivetrain: 8 motors per side (77w)
- intake: 1 motor (11w)
- sensors:
  - imu
  - horizontal tracking wheel
- pneumatics:
  - mobile goal clamp (small)
  - intake piston (large)

## dependencies

- pros
- lemlib
- lvgl

## usage

1. flash the program to your VEX V5 brain
2. run the program
2. use the lvgl gui on the brain screen to select autonomous mode
3. in driver control:
   - left/right joysticks control drivetrain
   - r1/r2 buttons control intake
   - l1 toggles mobile goal clamp
   - l2 toggles intake position
   - b button toggles reverse drive

## autonomous modes

- close side
- far side
- skills
- off (default)

## main.cpp code snippet
![Code snippet main cpp](https://github.com/uz-g/saf50805a/assets/83078325/2fe7f481-29a7-493e-839a-589c524d31b2)
This snippet showcases:
1. Real-time match timer with controller feedback
2. Advanced use of C++ chrono library for precise timing
3. Motor temperature monitoring and averaging
4. Dynamic LCD updates with formatted temperature data
