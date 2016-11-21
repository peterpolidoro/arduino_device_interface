#InterruptTester

Authors:

    Peter Polidoro <polidorop@janelia.hhmi.org>

License:

    BSD

##Host Computer Interface

###Arduino Serial Monitor

Open the Serial Monitor in the Arduino IDE.

Set the baudrate to match the value in the Arduino sketch (115200).

Set the line ending to 'Newline'.

To get help information about the modular device, type a single
question mark ? into the input property and press the 'Send' button or
press the 'Enter' key.

```shell
?
```

Example Response:

```json
{
  "id":"?",
  "result":{
    "device_id":{
      "name":"minimal_device",
      "form_factor":"5x3",
      "serial_number":0
    },
    "API":{
      "firmware":["all"],
      "functions":[
        "getDeviceId",
        "getDeviceInfo",
        "getApi",
        "getApiVerbose",
        "getPropertyDefaultValues",
        "setPropertiesToDefaults",
        "getPropertyValues",
        "getMemoryFree"
      ],
      "parameters":[
        "firmware"
      ],
      "properties":[
        "serialNumber"
      ],
      "callbacks":[]
    }
  }
}
```

"functions" is an array of user functions. To execute a function, simply
type it into the input property and press the 'Send' button or press the
'Enter' key.

Example Function (This function only exists on AVR processors):

```shell
getMemoryFree
```

Example Response:

```json
{
  "id":"getMemoryFree",
  "result":4715
}
```

To get more verbose help about the modular device, including
information about the parameters each function takes, type two question
marks ?? into the input property and press the 'Send' button or press the
'Enter' key.

```shell
??
```

Example Response:

```json
{
  "id":"??",
  "result":{
    "device_id":{
      "name":"minimal_device",
      "form_factor":"5x3",
      "serial_number":0
    },
    "device_info":{
      "processor":"ATmega2560",
      "hardware":[
        {
          "name":"Mega2560"
        }
      ],
      "firmware":[
        {
          "name":"ModularServer",
          "version":"2.0.0"
        },
        {
          "name":"InterruptTester",
          "version":"1.0.0"
        }
      ]
    },
    "API":{
      "firmware":["all"],
      "functions":[
        {
          "name":"getDeviceId",
          "firmware":"ModularServer",
          "parameters":[],
          "result_type":"object"
        },
        {
          "name":"getDeviceInfo",
          "firmware":"ModularServer",
          "parameters":[],
          "result_type":"object"
        },
        {
          "name":"getApi",
          "firmware":"ModularServer",
          "parameters":[
            "firmware"
          ],
          "result_type":"object"
        },
        {
          "name":"getApiVerbose",
          "firmware":"ModularServer",
          "parameters":[
            "firmware"
          ],
          "result_type":"object"
        },
        {
          "name":"getPropertyDefaultValues",
          "firmware":"ModularServer",
          "parameters":[],
          "result_type":"object"
        },
        {
          "name":"setPropertiesToDefaults",
          "firmware":"ModularServer",
          "parameters":[],
          "result_type":null
        },
        {
          "name":"getPropertyValues",
          "firmware":"ModularServer",
          "parameters":[],
          "result_type":"object"
        },
        {
          "name":"getMemoryFree",
          "firmware":"ModularServer",
          "parameters":[],
          "result_type":"long"
        }
      ],
      "parameters":[
        {
          "name":"firmware",
          "firmware":"ModularServer",
          "type":"array",
          "array_element_type":"string",
          "array_element_subset":[
            "all",
            "ModularServer",
            "InterruptTester"
          ],
          "array_length_min":1,
          "array_length_max":8
        }
      ],
      "properties":[
        {
          "name":"serialNumber",
          "firmware":"ModularServer",
          "type":"long",
          "min":0,
          "max":65535,
          "value":0,
          "default_value":0,
          "functions":[
            "getValue",
            "setValue",
            "getDefaultValue",
            "setValueToDefault"
          ],
          "parameters":[
            "value"
          ]
        }
      ],
      "callbacks":[]
    }
  }
}
```

Example Function with Parameters:

```shell
getApi
```

Example Response:

```json
{
  "id":"getApi",
  "error":{
    "message":"Invalid params",
    "data":"Incorrect number of parameters. 0 given. 1 needed.",
    "code":-32602
  }
}
```

When a function executes successfully, the response will contain a
"result" property. In some cases the result may be null, but the function
execution was still successful. When there is an error, there will not
exist a "result" property, but there will exist an "error" property.

To get more information about a function, enter the function followed by
a question mark.

Example Function Help:

```shell
getApi ?
```

Example Response:

```json
{
  "id":"getApi",
  "result":{
    "name":"getApi",
    "firmware":"ModularServer",
    "parameters":[
      "firmware"
    ],
    "result_type":"object"
  }
}
```

Or you can enter a question mark followed by the function.

Example Function Help:

```shell
? getApi
```

Example Response:

```json
{
  "id":"?",
  "result":{
    "name":"getApi",
    "firmware":"ModularServer",
    "parameters":[
      "firmware"
    ],
    "result_type":"object"
  }
}
```

To get more verbose information about all of the parameters a function
takes, enter the function followed by two questions marks.

```shell
getApi ??
```

Example Response:

```json
{
  "id":"getApi",
  "result":{
    "name":"getApi",
    "firmware":"ModularServer",
    "parameters":[
      {
        "name":"firmware",
        "firmware":"ModularServer",
        "type":"array",
        "array_element_type":"string",
        "array_element_subset":[
          "all",
          "ModularServer",
          "InterruptTester"
        ],
        "array_length_min":1,
        "array_length_max":8
      }
    ],
    "result_type":"object"
  }
}
```

Example Function:

```shell
getApi ["InterruptTester"]
```

Example Response:

```json
{
  "id":"getApi",
  "result":{
    "firmware":["InterruptTester"],
    "functions":[],
    "parameters":[],
    "properties":[],
    "callbacks":[]
  }
}
```

Properties are special functions that get and set field values that are
stored on the device, even after the device is powered off.

Properties have their own set of functions with their own set of
property function parameters.

Example Property:
```shell
? serialNumber
```

Example Response:

```json
{
  "id":"?",
  "result":{
    "name":"serialNumber",
    "firmware":"ModularServer",
    "type":"long",
    "min":0,
    "max":65535,
    "value":0,
    "default_value":0,
    "functions":[
      "getValue",
      "setValue",
      "getDefaultValue",
      "setValueToDefault"
    ],
    "parameters":[
      "value"
    ]
  }
}
```

To get information about a property function:

```shell
? serialNumber setValue
```

```json
{
  "id":"?",
  "result":{
    "name":"setValue",
    "firmware":"ModularServer",
    "parameters":[
      "value"
    ],
    "result_type":"long"
  }
}
```

To get information about a property function parameter:

```shell
? serialNumber setValue value
```

```json
{
  "id":"?",
  "result":{
    "name":"value",
    "firmware":"ModularServer",
    "type":"long",
    "min":0,
    "max":65535
  }
}
```

Example property function call:

```shell
serialNumber setValue -1
```

```json
{
  "id":"serialNumber",
  "error":{
    "message":"Invalid params",
    "data":"Parameter value not valid. Value not in range: 0 <= value <= 65535",
    "code":-32602
  }
}
```

Example property function call:

```shell
serialNumber setValue 32
```

```json
{
  "id":"serialNumber",
  "result":32
}
```

The serial number setting persists even after the device is powered
off. The serial number is used to differentiate several identical
devices connected to a single host machine at one time.

Example Function:

```shell
getPropertyValues
```

Example Response:

```json
{
  "id":"getPropertyValues",
  "result":{
    "serialNumber":32
  }
}
```

To reset the serial number to the default value, use the
setValueToDefault property function.

Or use the setPropertiesToDefault function to set all properties to
their default values.

Example Function:

```shell
setPropertiesToDefaults
```

Example Response:

```json
{
  "id":"setPropertiesToDefaults",
  "result":null
}
```

Example Function:

```shell
getPropertyValues
```

Example Response:

```json
{
  "id":"getPropertyValues",
  "result":{
    "serialNumber":0
  }
}
```

Use the getDeviceId function to get a unique set of values to identify
the device.

Example:

```shell
getDeviceId
```

Example Response:

```json
{
  "id":"getDeviceId",
  "result":{
    "name":"minimal_device",
    "form_factor":"5x3",
    "serial_number":0
  }
}
```

The serial\_number property can be changed to uniquely identify devices
with the same name and form\_factor.

Use the getDeviceInfo function to get information about the hardware and
firmware of the device.

Example:

```shell
getDeviceInfo
```

Example Response:

```json
{
  "id":"getDeviceInfo",
  "result":{
    "processor":"ATmega2560",
    "hardware":[
      {
        "name":"Mega2560"
      }
    ],
    "firmware":[
      {
        "name":"ModularServer",
        "version":"2.0.0"
      },
      {
        "name":"InterruptTester",
        "version":"1.0.0"
      }
    ]
  }
}
```

Every function, parameter, property, and callback belongs to one firmware set.

To get the API limited to one or more firmware sets, use the getApi
function.

Example:

```shell
getApi ["all"]
```

Example Response:

```json
{
  "id":"getApi",
  "result":{
    "firmware":["all"],
    "functions":[
      "getDeviceId",
      "getDeviceInfo",
      "getApi",
      "getApiVerbose",
      "getPropertyDefaultValues",
      "setPropertiesToDefaults",
      "getPropertyValues",
      "getMemoryFree"
    ],
    "parameters":[
      "firmware"
    ],
    "properties":[
      "serialNumber"
    ],
    "callbacks":[]
  }
}
```

Example:

```shell
getApi ["InterruptTester"]
```

Example Response:

```json
{
  "id":"getApi",
  "result":{
    "firmware":["InterruptTester"],
    "functions":[],
    "parameters":[],
    "properties":[],
    "callbacks":[]
  }
}
```

###Python

Example Python session:

```python
from modular_device import ModularClient
dev = ModularClient() # Automatically finds device if one available
dev.get_device_id()
{'form_factor': '5x3', 'name': 'minimal_device', 'serial_number': 0}
dev.get_methods()
['get_memory_free',
 'serial_number',
 'get_api',
 'get_api_verbose',
 'get_property_values',
 'get_device_id',
 'get_property_default_values',
 'set_properties_to_defaults',
 'get_device_info']
dev.get_memory_free()
4715
dev.get_api()
IOError: (from server) message: Invalid params, data: Incorrect number of parameters. 0 given. 1 needed., code: -32602
dev.get_api('?')
{'firmware': 'ModularServer',
 'name': 'getApi',
 'parameters': ['firmware'],
 'result_type': 'object'}
dev.get_api(['InterruptTester'])
{'callbacks': [],
 'firmware': ['InterruptTester'],
 'functions': [],
 'parameters': [],
 'properties': []}
dev.serial_number('setValue',-1)
IOError: (from server) message: Invalid params, data: Parameter value not valid. Value not in range: 0 <= value <= 65535, code: -32602
dev.serial_number('setValue',32)
32
dev.get_property_values()
{'serialNumber': 32}
result = dev.call_server_method('?')
result['device_id']['serial_number']
32
dev.convert_to_json(result['device_id'])
'{"serial_number":32,"name":"minimal_device","form_factor":"5x3"}'
dev.send_json_request('["set_properties_to_defaults"]')
dev.serial_number('getValue')
0
```

For more details on the Python interface:

<https://github.com/janelia-pypi/modular_client_python>

###Matlab

Example Matlab session:

```matlab
% Linux and Mac OS X
ls /dev/tty*
serial_port = '/dev/ttyACM0'     % example Linux serial port
serial_port = '/dev/tty.usbmodem262471' % example Mac OS X serial port
% Windows
getAvailableComPorts()
ans =
  'COM1'
  'COM4'
serial_port = 'COM4'             % example Windows serial port
dev = ModularClient(serial_port) % creates a device object
dev.open()                       % opens a serial connection to the device
dev.getDeviceId()
ans =
  name: 'minimal_device'
  form_factor: '5x3'
  serial_number: 0
dev.getMethods()                 % get device methods
  Modular Device Methods
  ---------------------
  getDeviceId
  getDeviceInfo
  getApi
  getApiVerbose
  getPropertyDefaultValues
  setPropertiesToDefaults
  getPropertyValues
  getMemoryFree
  serialNumber
dev.getMemoryFree()
ans =
  4715
dev.getApi()
(from server) message: Invalid params, data: Incorrect number of parameters. 0 given. 1 needed.,
code: -32602
dev.getApi('?')
ans =
  name: 'getApi'
  firmware: 'ModularServer'
  parameters: {'firmware'}
  result_type: 'object'
dev.getApi('firmware','?')
ans =
  name: 'firmware'
  firmware: 'ModularServer'
  type: 'array'
  array_element_type: 'string'
  array_element_subset: {'all'  'ModularServer'  'InterruptTester'}
  array_length_min: 1
  array_length_max: 8
dev.getApi({'InterruptTester'})
ans =
  firmware: {'InterruptTester'}
  functions: {0x1 cell}
  parameters: {0x1 cell}
  properties: {0x1 cell}
  callbacks: {0x1 cell}
dev.serialNumber('setValue',-1)
(from server) message: Invalid params, data: Parameter value not valid. Value not in range: 0 <=
value <= 65535, code: -32602
dev.serialNumber('setValue',32);
ans =
  32
dev.getPropertyValues()
ans =
  serialNumber: 32
result = dev.callServerMethod('?');
result.device_id.serial_number
ans =
  32
json = dev.convertToJson(result.device_id)
{"name": "minimal_device","form_factor": "5x3","serial_number": 32}
dev.sendJsonRequest('["setPropertiesToDefaults"]')
dev.serialNumber('getValue')
ans =
  0
dev.close()
clear dev
```

For more details on the Matlab interface:

<https://github.com/janelia-matlab/modular_client_matlab>

##Installation

<https://github.com/janelia-arduino/arduino-libraries>