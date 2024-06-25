# VEX V5 robot control system
<h1 align="center">AstroNvim</h1>

<p align="center">
    <a href="https://github.com/uz-g/saf508085a/pulse"><img src="https://img.shields.io/github/last-commit/uz-g/saf508085a?style=for-the-badge&logo=github&color=7dc4e4&logoColor=D9E0EE&labelColor=302D41"></a>
    <a href="https://github.com/uz-g/saf508085a/releases/latest"><img src="https://img.shields.io/github/v/release/uz-g/saf508085a?style=for-the-badge&logo=gitbook&color=8bd5ca&logoColor=D9E0EE&labelColor=302D41"></a>
    <a href="https://github.com/uz-g/saf508085a/stargazers"><img src="https://img.shields.io/github/stars/uz-g/saf508085a?style=for-the-badge&logo=apachespark&color=eed49f&logoColor=D9E0EE&labelColor=302D41"></a>
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

- drivetrain: 8 motors per side
- intake: 1 motor
- sensors:
  - imu
  - horizontal tracking wheel
- pneumatics:
  - mobile goal clamp
  - intake piston

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
