% sweep_ctl(1) User Manuals
% Daniel Hofmann
% February 18, 2017

# NAME

sweep_ctl - get and set Sweep LiDAR hardware properties

# SYNOPSIS

sweep_ctl dev get|set key [value]

# DESCRIPTION

Command line tool to interact with the Sweep LiDAR device.

# OPTIONS

get property
:   Returns value currently set for property.

set property value
:    Sets value for property.

# PROPERTIES

motor\_speed
:    The device's motor speed in Hz.

sample\_rate
:    The device's sample rate in Hz.

# EXAMPLE

    $ sweep_ctl /dev/ttyUSB0 get motor_speed
    3

    $ sweep_ctl /dev/ttyUSB0 set motor_speed 5
    5
