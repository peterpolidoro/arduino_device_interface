#include <EEPROM.h>
#include <iterator>
#include <vector>
#include <pnew.cpp>
#include "Streaming.h"
#include "JsonParser.h"
#include "MemoryFree.h"
#include "Flash.h"
#include "RemoteDevice.h"
#include "Constants.h"
#include "Callbacks.h"

// See README.md for more information

using namespace RemoteDevice;

void setup()
{
  remote_device.setName(constants::device_name);
  remote_device.setModelNumber(constants::model_number);
  remote_device.setFirmwareNumber(constants::firmware_number);

  Method& echo_method = remote_device.createMethod(constants::echo_method_name);
  echo_method.attachCallback(callbacks::echoCallback);
  Parameter& string_parameter = remote_device.createParameter(constants::string_parameter_name);
  string_parameter.setTypeString();
  echo_method.addParameter(string_parameter);

  Method& length_method = remote_device.createMethod(constants::length_method_name);
  length_method.attachCallback(callbacks::lengthCallback);
  length_method.addParameter(string_parameter);

  Method& starts_with_method = remote_device.createMethod(constants::starts_with_method_name);
  starts_with_method.attachCallback(callbacks::startsWithCallback);
  starts_with_method.addParameter(string_parameter);
  Parameter& string2_parameter = remote_device.copyParameter(string_parameter,constants::string2_parameter_name);
  starts_with_method.addParameter(string2_parameter);

  Method& repeat_method = remote_device.createMethod(constants::repeat_method_name);
  repeat_method.attachCallback(callbacks::repeatCallback);
  repeat_method.addParameter(string_parameter);
  Parameter& count_parameter = remote_device.createParameter(constants::count_parameter_name);
  count_parameter.setRange(constants::count_min,constants::count_max);
  repeat_method.addParameter(count_parameter);

  Method& chars_at_method = remote_device.createMethod(constants::chars_at_method_name);
  chars_at_method.attachCallback(callbacks::charsAtCallback);
  chars_at_method.addParameter(string_parameter);
  Parameter& index_array_parameter = remote_device.createParameter(constants::index_array_parameter_name);
  index_array_parameter.setTypeArray();
  chars_at_method.addParameter(index_array_parameter);

  Method& starting_chars_method = remote_device.createMethod(constants::starting_chars_method_name);
  starting_chars_method.attachCallback(callbacks::startingCharsCallback);
  starting_chars_method.addParameter(string_parameter);

  remote_device.createSavedVariable(constants::starting_chars_count_name,constants::starting_chars_count_default);

  Method& set_starting_chars_count_method = remote_device.createMethod(constants::set_starting_chars_count_method_name);
  set_starting_chars_count_method.attachCallback(callbacks::setStartingCharsCountCallback);
  Parameter& starting_chars_count_parameter = remote_device.createParameter(constants::starting_chars_count_name);
  starting_chars_count_parameter.setRange(constants::starting_chars_count_min,constants::starting_chars_count_max);
  set_starting_chars_count_method.addParameter(starting_chars_count_parameter);

  Method& get_starting_chars_count_method = remote_device.createMethod(constants::get_starting_chars_count_method_name);
  get_starting_chars_count_method.attachCallback(callbacks::getStartingCharsCountCallback);

  remote_device.startServer(constants::baudrate);
}

void loop()
{
  remote_device.handleServerRequests();
}
