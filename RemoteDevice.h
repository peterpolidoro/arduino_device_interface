// ----------------------------------------------------------------------------
// RemoteDevice.h
//
//
// Authors:
// Peter Polidoro polidorop@janelia.hhmi.org
// ----------------------------------------------------------------------------
#ifndef REMOTE_DEVICE_H
#define REMOTE_DEVICE_H
#include "utility/Server.h"


namespace RemoteDevice
{
class Method;

class RemoteDevice
{
public:
  RemoteDevice(Stream &stream);
  void setServerStream(Stream &stream);
  void handleServerRequests();
  void setName(_FLASH_STRING &device_name);
  void setModelNumber(int model_number);
  void setFirmwareNumber(int firmware_number);
  Method& createMethod(_FLASH_STRING &method_name);
  Method& copyMethod(Method &method,_FLASH_STRING &method_name);
  Parameter& createParameter(_FLASH_STRING &parameter_name);
  Parameter& copyParameter(Parameter &parameter,_FLASH_STRING &parameter_name);
  ArduinoJson::Parser::JsonValue getParameterValue(_FLASH_STRING &parameter_name);
  template<typename T>
  void addToResponse(const char* key, T value)
  {
    server_.addToResponse(key,value);
  }
  template<typename T>
  void addToResponse(T value)
  {
    server_.addToResponse(value);
  }
  void addNullToResponse(const char* key);
  void addNullToResponse();
  void addBooleanToResponse(const char* key, boolean value);
  void addBooleanToResponse(boolean value);
  void addKeyToResponse(const char* key);
  void startResponseObject();
  void stopResponseObject();
  void startResponseArray();
  void stopResponseArray();
private:
  Server server_;
};
}

extern RemoteDevice::RemoteDevice remote_device;

void getMemoryFreeCallback();

#endif
