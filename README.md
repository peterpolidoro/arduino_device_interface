arduino_remote_device
---------------------

Authors:

    Peter Polidoro <polidorop@janelia.hhmi.org>

License:

    BSD

##Example Usage

###Arduino Firmware

This firmware requires at least 3.2KB of SRAM and 25.6KB of flash
memory, so it needs Arduino Mega level resources.

[Arduino Sketch Examples](./examples)

###Host Computer Interface

For more detailed host computer interface information (Arduino Serial
Monitor, Python, and Matlab) see example sketch README files:

[LedController](./examples/LedController)

[MinimalDevice](./examples/MinimalDevice)

[StringController](./examples/StringController)

####Arduino Serial Monitor

Open the Serial Monitor in the Arduino IDE.

Set the baud rate to match the value in your Arduino sketch.

Set the line ending to 'Newline'.

To get help information about the Arduino device, type a single
question mark ? into the input field and press the 'Send' button or
press the 'Enter' key.

```shell
?
```

The response will contain a field named "methods", which is an array
of user methods. To execute a method, simply type it into the input
field and press the 'Send' button or press the 'Enter' key.

To get more verbose help, including information about the parameters
each method takes, type two questions marks:

```shell
??
```

To get more information about a single method, enter the method
followed by a question mark ?

Example Method Help:

```shell
setSerialNumber ?
```

To get more verbose help about the method, including more information
about the parameters each method takes, type the method name followed
by two questions marks:

```shell
setSerialNumber ??
```

####Python

<https://github.com/JaneliaSciComp/python_remote_device>

####Matlab

<https://github.com/JaneliaSciComp/matlab_remote_device>

##Installation

Andy Brown's Standard Template Library (STL) for AVR with C++ streams
must be installed before compiling.

See install instructions either here:

<https://github.com/peterpolidoro/avr-stl>

or here:

<http://andybrown.me.uk/wk/2011/01/15/the-standard-template-library-stl-for-avr-with-c-streams/>

This library and its dependencies can be installed together or
separately.

###Install This Library and its Dependencies Together

Clone the arduino-libraries repository:

```shell
git clone https://github.com/JaneliaSciComp/arduino-libraries.git
```

Install symbolic library links into the default sketchbook directory
using Python when running Linux or Mac OS X:

```shell
python symlinks.py --install
```
Or for more information on installing libraries, see:

<http://arduino.cc/en/Guide/Libraries>

###Install This Library and its Dependencies Separately

####Clone This Repository into the Directory 'RemoteDevice'

```shell
git clone https://github.com/JaneliaSciComp/arduino_remote_device.git RemoteDevice
```

Install symbolic library link into the default sketchbook directory
using Python when running Linux or Mac OS X:

```shell
python symlink.py --install
```
Or for more information on installing libraries, see:

<http://arduino.cc/en/Guide/Libraries>

####Download and Install the Arduino Library Dependencies

Streaming:

<http://arduiniana.org/libraries/streaming/>

ArduinoJson:

<https://github.com/bblanchon/ArduinoJson>

MemoryFree:

<https://github.com/maniacbug/MemoryFree>
