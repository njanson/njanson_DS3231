# njanson_DS3231

[TOC]

# Overview

This library presents a simple interface to the DS3231 real-time clock for the
Arduino platform.  Most of the information on the library's use may be found in
the header file of the library.

# Initialization

Instantiate the object and call `begin()`.  You'll probably want to set the
clock to a reasonable number, at least to start out with.  Note that if there
is a battery-backup unit (as with Switchdoc's version), your RTC will maintain
state even when it loses power on VCC.

```
njanson_DS3231 ds3231;

if (!ds3231.begin()) {
  Serial.println("Unable to find DS3231!  Check your wiring.");
  exit(1);
}

// Set the clock to Saturday, 2017-01-28 20:58:00.
ds3231.SetClock(2017, 1, 28, 7, 20, 58, 00);
```

# Reading

The library implements reading by updating the internal state of the object.
Therefore, you must call the `Read()` function and access the member variables
via their accessor methods.

```
ds3231.Read();
year = ds3231.year();
...
```

# Setting the Clock

Use the `SetClock()` function.  This should be mostly self-explanatory, but one
field bears explanation: the weekDay field's valid values are from 1 - 7, where
1 is Sunday and 7 is Saturday.  `SetClock()` will also update the internal
state of the object once it's confirmed that the correct number of bytes have
been written.

```
// Set the clock to Saturday, 2017-01-28 20:58:00.
if (!ds3231.SetClock(2017, 1, 28, 7, 20, 58, 00)) {
  Serial.println("Failed to set clock!");
}
```
