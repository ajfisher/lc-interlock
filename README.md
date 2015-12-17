# Laser Cutter Display and Software Interlock

This is a status display and software interlock for a K40 laser cutter. It is
designed to work with a laser cutter where there is a physical interlock loop
that is closed before the laser is allowed to fire.

## Overview:

This set up has the primary function of looking at the external elements
surrounding the laser cutter and determining if everything is within acceptable
bounds before closing a relay on the interlock loop.

Note that this should not be used for software control of absolute safety
components of the interlock loop. Specifically, the door and key switches should
remain separate to this and be purely physical as should the eStop.

A secondary feature of this system is to show the temperature, water flow rate,
door and key status visually on an LCD.

## Design

The design of this system is to work in parallel to an existing RAMPs set up
in order to provide feedback into the system. The primary output of the system
is to CLOSE a mechanical relay which is wired in series into the interlock loop.
The closing of this relay (under software control) will close one point on the
interlock loop, and when the whole loop is closed, the laser will be allowed to
fire.

### Inputs

The inputs are:

* 3x One wire temperature sensors, one either side of the laser tube as well as
one in the water tank.
* Door close (using a SPDT switch so it can also become an input)
* Key enable (using SPDT key switch to it can also become an input)
* Flow meter to meaure water flow rate of the pump

### Outputs

* 20x4 character LCD showing status of the system
* Relay wired into the interlock loop that closes on acceptable operation

### Controller

This system has been designed to work with an arduino nano but any arduino
based uC would work.

## Licence

This software is licenced under the MIT licence. See [LICENCE](LICENCE) file
for details.

This software includes the Arduino OneWire library maintained by Paul Stoffregen
made available at http://www.pjrc.com/teensy/td_libs_OneWire.html original
copyright and licences are maintained in the original files included within this
repository.


