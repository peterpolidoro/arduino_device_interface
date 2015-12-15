// ----------------------------------------------------------------------------
// Server.cpp
//
//
// Authors:
// Peter Polidoro polidorop@janelia.hhmi.org
// ----------------------------------------------------------------------------
#include "Server.h"


namespace ModularDevice
{
Server::Server(Stream &stream) :
  json_stream_(stream)
{
  addServerStream(stream);
  setName(constants::empty_constant_string);
  model_number_ = 0;
  firmware_major_ = 0;
  firmware_minor_ = 0;
  firmware_patch_ = 0;
  request_method_index_ = -1;
  parameter_count_ = 0;
  error_ = false;
  result_in_response_ = false;
  server_stream_index_ = 0;
  server_running_ = false;

  Method& get_device_info_method = createMethod(constants::get_device_info_method_name);
  get_device_info_method.attachReservedCallback(&Server::getDeviceInfoCallback);

  Method& get_method_ids_method = createMethod(constants::get_method_ids_method_name);
  get_method_ids_method.attachReservedCallback(&Server::getMethodIdsCallback);

  Method& get_parameters_method = createMethod(constants::get_parameters_method_name);
  get_parameters_method.attachReservedCallback(&Server::getParametersCallback);

  Method& help_method = createMethod(constants::help_method_name);
  help_method.attachReservedCallback(&Server::help);

  Method& verbose_help_method = createMethod(constants::verbose_help_method_name);
  verbose_help_method.attachReservedCallback(&Server::verboseHelp);

  eeprom_index_ = 0;
  eeprom_initialized_index_ = eeprom_index_;
  eeprom_init_name_ptr_ = &constants::eeprom_initialized_saved_variable_name;
  eeprom_uninitialized_ = true;
  saved_variable_array_.push_back(SavedVariable(*eeprom_init_name_ptr_,
                                                eeprom_index_,
                                                constants::eeprom_initialized_value));
  eeprom_index_ += sizeof(constants::eeprom_initialized_value);
  createSavedVariable(constants::serial_number_constant_string,constants::serial_number_default);
}

void Server::addServerStream(Stream &stream)
{
  bool stream_found = false;
  for (unsigned int i=0;i<server_stream_ptr_array_.size();++i)
  {
    if (server_stream_ptr_array_[i] == &stream)
    {
      stream_found = true;
    }
  }
  if (!stream_found)
  {
    server_stream_ptr_array_.push_back(&stream);
  }
}

void Server::setName(const ConstantString &name)
{
  name_ptr_ = &name;
}

void Server::setModelNumber(const unsigned int model_number)
{
  model_number_ = model_number;
}

void Server::setSerialNumber(const unsigned int serial_number)
{
  setSavedVariableValue(constants::serial_number_constant_string,serial_number);
}

void Server::setFirmwareVersion(const unsigned char firmware_major,const unsigned char firmware_minor,const unsigned char firmware_patch)
{
  firmware_major_ = firmware_major;
  firmware_minor_ = firmware_minor;
  firmware_patch_ = firmware_patch;
}

Method& Server::createMethod(const ConstantString &method_name)
{
  int method_index = findMethodIndex(method_name);
  if (method_index < 0)
  {
    method_array_.push_back(Method(method_name));
    return method_array_.back();
  }
  else
  {
    method_array_[method_index] = Method(method_name);
    return method_array_[method_index];
  }
}

Method& Server::copyMethod(Method method,const ConstantString &method_name)
{
  method_array_.push_back(method);
  method_array_.back().setName(method_name);
  return method_array_.back();
}

Parameter& Server::createParameter(const ConstantString &parameter_name)
{
  int parameter_index = findParameterIndex(parameter_name);
  if (parameter_index < 0)
  {
    parameter_array_.push_back(Parameter(parameter_name));
    return parameter_array_.back();
  }
  else
  {
    parameter_array_[parameter_index] = Parameter(parameter_name);
    return parameter_array_[parameter_index];
  }
}

Parameter& Server::copyParameter(Parameter parameter,const ConstantString &parameter_name)
{
  parameter_array_.push_back(parameter);
  parameter_array_.back().setName(parameter_name);
  return parameter_array_.back();
}

ArduinoJson::JsonVariant Server::getParameterValue(const ConstantString &name)
{
  int parameter_index = findMethodParameterIndex(request_method_index_,name);
  // index 0 is the method, index 1 is the first parameter
  return (*request_json_array_ptr_)[parameter_index+1];
}

void Server::writeNullToResponse()
{
  json_stream_.writeNull();
}

void Server::writeResultKeyToResponse()
{
  json_stream_.writeKey(constants::result_constant_string);
  result_in_response_ = true;
}

void Server::beginResponseObject()
{
  json_stream_.beginObject();
}

void Server::endResponseObject()
{
  json_stream_.endObject();
}

void Server::beginResponseArray()
{
  json_stream_.beginArray();
}

void Server::endResponseArray()
{
  json_stream_.endArray();
}

void Server::resetDefaults()
{
  for (unsigned int i=0; i<saved_variable_array_.size(); ++i)
  {
    saved_variable_array_[i].setDefaultValue();
  }
}

void Server::startServer()
{
  if (eeprom_uninitialized_)
  {
    initializeEeprom();
  }
  server_running_ = true;
}

void Server::stopServer()
{
  server_running_ = false;
}

void Server::handleRequest()
{
  if (server_running_ && (json_stream_.available() > 0))
  {
    int bytes_read = json_stream_.readJsonIntoBuffer(request_,constants::STRING_LENGTH_REQUEST);
    if (bytes_read > 0)
    {
      JsonSanitizer sanitizer(constants::JSON_TOKEN_MAX);
      if (sanitizer.firstCharIsValidJson(request_))
      {
        json_stream_.setCompactPrint();
      }
      else
      {
        json_stream_.setPrettyPrint();
      }
      json_stream_.beginObject();
      error_ = false;
      result_in_response_ = false;
      sanitizer.sanitize(request_,constants::STRING_LENGTH_REQUEST);
      StaticJsonBuffer<constants::STRING_LENGTH_REQUEST> json_buffer;
      if (sanitizer.firstCharIsValidJsonObject(request_))
      {
        error_ = true;
        writeKeyToResponse(constants::error_constant_string);
        beginResponseObject();
        writeToResponse(constants::message_constant_string,constants::server_error_error_message);
        writeToResponse(constants::data_constant_string,constants::object_request_error_data);
        writeToResponse(constants::code_constant_string,constants::server_error_error_code);
        endResponseObject();
      }
      else
      {
        request_json_array_ptr_ = &(json_buffer.parseArray(request_));
        if (request_json_array_ptr_->success())
        {
          processRequestArray();
        }
        else
        {
          error_ = true;
          writeKeyToResponse(constants::error_constant_string);
          beginResponseObject();
          writeToResponse(constants::message_constant_string,constants::parse_error_message);
          writeToResponse(constants::data_constant_string,request_);
          writeToResponse(constants::code_constant_string,constants::parse_error_code);
          endResponseObject();
        }
        if (!error_ && !result_in_response_)
        {
          writeNullToResponse(constants::result_constant_string);
        }
      }
      endResponseObject();
      json_stream_.writeNewline();
    }
    else if (bytes_read < 0)
    {
      json_stream_.setCompactPrint();
      json_stream_.beginObject();
      error_ = true;
      writeKeyToResponse(constants::error_constant_string);
      beginResponseObject();
      writeToResponse(constants::message_constant_string,constants::server_error_error_message);
      writeToResponse(constants::data_constant_string,constants::request_length_error_data);
      writeToResponse(constants::code_constant_string,constants::server_error_error_code);
      endResponseObject();
      endResponseObject();
      json_stream_.writeNewline();
    }
  }
  incrementServerStream();
}

void Server::processRequestArray()
{
  const char* method_string = (*request_json_array_ptr_)[0];
  request_method_index_ = processMethodString(method_string);
  if (!(request_method_index_ < 0))
  {
    int array_elements_count = countJsonArrayElements((*request_json_array_ptr_));
    int parameter_count = array_elements_count - 1;
    // method ?
    if ((parameter_count == 1) && (strcmp((*request_json_array_ptr_)[1],"?") == 0))
    {
      writeResultKeyToResponse();
      beginResponseObject();
      writeKeyToResponse(constants::method_info_constant_string);
      methodHelp(false,request_method_index_);
      endResponseObject();
    }
    // method ??
    else if ((parameter_count == 1) && (strcmp((*request_json_array_ptr_)[1],"??") == 0))
    {
      writeResultKeyToResponse();
      beginResponseObject();
      writeKeyToResponse(constants::method_info_constant_string);
      methodHelp(true,request_method_index_);
      endResponseObject();
    }
    // method parameter ?
    // method parameter ??
    else if ((parameter_count == 2) &&
             ((strcmp((*request_json_array_ptr_)[2],"?") == 0) ||
              (strcmp((*request_json_array_ptr_)[2],"??") == 0)))
    {
      int parameter_index = processParameterString((*request_json_array_ptr_)[1]);
      Parameter& parameter = *(method_array_[request_method_index_].parameter_ptr_array_[parameter_index]);
      writeResultKeyToResponse();
      beginResponseObject();
      writeKeyToResponse(constants::parameter_info_constant_string);
      parameterHelp(parameter);
      endResponseObject();
    }
    else if (method_array_[request_method_index_].isReserved())
    {
      executeMethod();
    }
    else if (parameter_count != method_array_[request_method_index_].parameter_count_)
    {
      error_ = true;
      writeKeyToResponse(constants::error_constant_string);
      beginResponseObject();
      writeToResponse(constants::message_constant_string,constants::invalid_params_error_message);
      char incorrect_parameter_number[constants::incorrect_parameter_number_error_data.length()+1];
      constants::incorrect_parameter_number_error_data.copy(incorrect_parameter_number);
      char error_str[constants::STRING_LENGTH_ERROR];
      error_str[0] = 0;
      strcat(error_str,incorrect_parameter_number);
      char parameter_count_str[constants::STRING_LENGTH_PARAMETER_COUNT];
      dtostrf(parameter_count,0,0,parameter_count_str);
      strcat(error_str,parameter_count_str);
      strcat(error_str," given. ");
      dtostrf(method_array_[request_method_index_].parameter_count_,0,0,parameter_count_str);
      strcat(error_str,parameter_count_str);
      strcat(error_str," needed.");
      writeToResponse(constants::data_constant_string,error_str);
      writeToResponse(constants::code_constant_string,constants::invalid_params_error_code);
      endResponseObject();
    }
    else
    {
      bool parameters_ok = checkParameters();
      if (parameters_ok)
      {
        executeMethod();
      }
    }
  }
}

int Server::processMethodString(const char *method_string)
{
  int method_index = -1;
  int method_id = atoi(method_string);
  if (strcmp(method_string,"0") == 0)
  {
    method_index = 0;
    writeToResponse(constants::id_constant_string,0);
  }
  else if (method_id > 0)
  {
    method_index = method_id;
    writeToResponse(constants::id_constant_string,method_id);
  }
  else
  {
    method_index = findMethodIndex(method_string);
    writeToResponse(constants::id_constant_string,method_string);
  }
  if ((method_index < 0) || (method_index >= (int)method_array_.size()))
  {
    error_ = true;
    writeKeyToResponse(constants::error_constant_string);
    beginResponseObject();
    writeToResponse(constants::message_constant_string,constants::method_not_found_error_message);
    writeToResponse(constants::code_constant_string,constants::method_not_found_error_code);
    endResponseObject();
    method_index = -1;
  }
  return method_index;
}

int Server::countJsonArrayElements(ArduinoJson::JsonArray &json_array)
{
  int elements_count = 0;
  for (ArduinoJson::JsonArray::iterator it=json_array.begin();
       it!=json_array.end();
       ++it)
  {
    elements_count++;
  }
  return elements_count;
}

void Server::executeMethod()
{
  if (method_array_[request_method_index_].isReserved())
  {
    method_array_[request_method_index_].reservedCallback(this);
  }
  else
  {
    method_array_[request_method_index_].callback();
  }
}

void Server::methodHelp(bool verbose, int method_index)
{
  beginResponseObject();
  const ConstantString& method_name = method_array_[method_index].getName();
  writeToResponse(constants::name_constant_string,method_name);

  writeKeyToResponse(constants::parameters_constant_string);
  json_stream_.beginArray();
  Array<Parameter*,constants::METHOD_PARAMETER_COUNT_MAX>& parameter_ptr_array = method_array_[method_index].parameter_ptr_array_;
  for (unsigned int i=0; i<parameter_ptr_array.size(); ++i)
  {
    if (verbose)
    {
      parameterHelp(*(parameter_ptr_array[i]));
    }
    else
    {
      const ConstantString& parameter_name = parameter_ptr_array[i]->getName();
      writeToResponse(parameter_name);
    }
  }
  json_stream_.endArray();
  writeToResponse(constants::result_type_constant_string,method_array_[method_index].getReturnType());
  endResponseObject();
}

int Server::processParameterString(const char *parameter_string)
{
  int parameter_index = -1;
  int parameter_id = atoi(parameter_string);
  if (strcmp(parameter_string,"0") == 0)
  {
    parameter_index = 0;
  }
  else if (parameter_id > 0)
  {
    parameter_index = parameter_id;
  }
  else
  {
    parameter_index = findMethodParameterIndex(request_method_index_,parameter_string);
  }
  Array<Parameter*,constants::METHOD_PARAMETER_COUNT_MAX>& parameter_ptr_array = method_array_[request_method_index_].parameter_ptr_array_;
  if ((parameter_index < 0) || (parameter_index >= (int)parameter_ptr_array.size()))
  {
    error_ = true;
    writeKeyToResponse(constants::error_constant_string);
    beginResponseObject();
    writeToResponse(constants::message_constant_string,constants::invalid_params_error_message);
    writeToResponse(constants::data_constant_string,constants::parameter_not_found_error_data);
    writeToResponse(constants::code_constant_string,constants::invalid_params_error_code);
    endResponseObject();
    parameter_index = -1;
  }
  return parameter_index;
}

int Server::findParameterIndex(const char *parameter_name)
{
  int parameter_index = -1;
  for (unsigned int i=0; i<parameter_array_.size(); ++i)
  {
    if (parameter_array_[i].compareName(parameter_name))
    {
      parameter_index = i;
      break;
    }
  }
  return parameter_index;
}

int Server::findParameterIndex(const ConstantString &parameter_name)
{
  int parameter_index = -1;
  for (unsigned int i=0; i<parameter_array_.size(); ++i)
  {
    if (parameter_array_[i].compareName(parameter_name))
    {
      parameter_index = i;
      break;
    }
  }
  return parameter_index;
}

int Server::findMethodParameterIndex(int method_index, const char *parameter_name)
{
  int parameter_index = -1;
  if (method_index >= 0)
  {
    Array<Parameter*,constants::METHOD_PARAMETER_COUNT_MAX>& parameter_ptr_array = method_array_[method_index].parameter_ptr_array_;
    for (unsigned int i=0; i<parameter_ptr_array.size(); ++i)
    {
      if (parameter_ptr_array[i]->compareName(parameter_name))
      {
        parameter_index = i;
        break;
      }
    }
  }
  return parameter_index;
}

int Server::findMethodParameterIndex(int method_index, const ConstantString &parameter_name)
{
  int parameter_index = -1;
  if (method_index >= 0)
  {
    Array<Parameter*,constants::METHOD_PARAMETER_COUNT_MAX>& parameter_ptr_array = method_array_[method_index].parameter_ptr_array_;
    for (unsigned int i=0; i<parameter_ptr_array.size(); ++i)
    {
      if (parameter_ptr_array[i]->compareName(parameter_name))
      {
        parameter_index = i;
        break;
      }
    }
  }
  return parameter_index;
}

void Server::parameterHelp(Parameter &parameter)
{
  beginResponseObject();
  const ConstantString& parameter_name = parameter.getName();
  writeToResponse(constants::name_constant_string,parameter_name);

  char parameter_units[constants::STRING_LENGTH_PARAMETER_UNITS];
  parameter_units[0] = 0;
  const ConstantString& units = parameter.getUnits();
  units.copy(parameter_units);
  if (strcmp(parameter_units,"") != 0)
  {
    writeToResponse(constants::units_constant_string,parameter_units);
  }
  JsonStream::JsonTypes type = parameter.getType();
  switch (type)
  {
    case JsonStream::LONG_TYPE:
      {
        writeToResponse(constants::type_constant_string,JsonStream::LONG_TYPE);
        if (parameter.rangeIsSet())
        {
          long min = parameter.getMin().l;
          long max = parameter.getMax().l;
          writeToResponse(constants::min_constant_string,min);
          writeToResponse(constants::max_constant_string,max);
        }
        break;
      }
    case JsonStream::DOUBLE_TYPE:
      {
        writeToResponse(constants::type_constant_string,JsonStream::DOUBLE_TYPE);
        if (parameter.rangeIsSet())
        {
          double min = parameter.getMin().d;
          double max = parameter.getMax().d;
          writeToResponse(constants::min_constant_string,min);
          writeToResponse(constants::max_constant_string,max);
        }
        break;
      }
    case JsonStream::BOOL_TYPE:
      {
        writeToResponse(constants::type_constant_string,JsonStream::BOOL_TYPE);
        break;
      }
    case JsonStream::NULL_TYPE:
      break;
    case JsonStream::STRING_TYPE:
      {
        writeToResponse(constants::type_constant_string,JsonStream::STRING_TYPE);
        break;
      }
    case JsonStream::OBJECT_TYPE:
      {
        writeToResponse(constants::type_constant_string,JsonStream::OBJECT_TYPE);
        break;
      }
    case JsonStream::ARRAY_TYPE:
      {
        writeToResponse(constants::type_constant_string,JsonStream::ARRAY_TYPE);
        JsonStream::JsonTypes array_element_type = parameter.getArrayElementType();
        switch (array_element_type)
        {
          case JsonStream::LONG_TYPE:
            {
              writeToResponse(constants::array_element_type_constant_string,JsonStream::LONG_TYPE);
              if (parameter.rangeIsSet())
              {
                long min = parameter.getMin().l;
                long max = parameter.getMax().l;
                writeToResponse(constants::min_constant_string,min);
                writeToResponse(constants::max_constant_string,max);
              }
              break;
            }
          case JsonStream::DOUBLE_TYPE:
            {
              writeToResponse(constants::array_element_type_constant_string,JsonStream::DOUBLE_TYPE);
              if (parameter.rangeIsSet())
              {
                double min = parameter.getMin().d;
                double max = parameter.getMax().d;
                writeToResponse(constants::min_constant_string,min);
                writeToResponse(constants::max_constant_string,max);
              }
              break;
            }
          case JsonStream::BOOL_TYPE:
            {
              writeToResponse(constants::array_element_type_constant_string,JsonStream::BOOL_TYPE);
              break;
            }
          case JsonStream::NULL_TYPE:
            break;
          case JsonStream::STRING_TYPE:
            {
              writeToResponse(constants::array_element_type_constant_string,JsonStream::STRING_TYPE);
              break;
            }
          case JsonStream::OBJECT_TYPE:
            {
              writeToResponse(constants::array_element_type_constant_string,JsonStream::OBJECT_TYPE);
              break;
            }
          case JsonStream::ARRAY_TYPE:
            {
              writeToResponse(constants::array_element_type_constant_string,JsonStream::ARRAY_TYPE);
              break;
            }
        }
        break;
      }
  }
  endResponseObject();
}

bool Server::checkParameters()
{
  int parameter_index = 0;
  for (ArduinoJson::JsonArray::iterator it=request_json_array_ptr_->begin();
       it!=request_json_array_ptr_->end();
       ++it)
  {
    if (it!=request_json_array_ptr_->begin())
    {
      if (checkParameter(parameter_index,*it))
      {
        parameter_index++;
      }
      else
      {
        return false;
      }
    }
  }
  parameter_count_ = parameter_index;
  return true;
}

bool Server::checkParameter(int parameter_index, ArduinoJson::JsonVariant &json_value)
{
  bool out_of_range = false;
  bool object_parse_unsuccessful = false;
  bool array_parse_unsuccessful = false;
  Parameter& parameter = *(method_array_[request_method_index_].parameter_ptr_array_[parameter_index]);
  JsonStream::JsonTypes type = parameter.getType();
  char min_str[JsonStream::STRING_LENGTH_DOUBLE];
  min_str[0] = 0;
  char max_str[JsonStream::STRING_LENGTH_DOUBLE];
  max_str[0] = 0;
  switch (type)
  {
    case JsonStream::LONG_TYPE:
      {
        if (parameter.rangeIsSet())
        {
          long value = (long)json_value;
          long min = parameter.getMin().l;
          long max = parameter.getMax().l;
          if ((value < min) || (value > max))
          {
            out_of_range = true;
            dtostrf(min,0,0,min_str);
            dtostrf(max,0,0,max_str);
          }
        }
        break;
      }
    case JsonStream::DOUBLE_TYPE:
      {
        if (parameter.rangeIsSet())
        {
          double value = (double)json_value;
          double min = parameter.getMin().d;
          double max = parameter.getMax().d;
          if ((value < min) || (value > max))
          {
            out_of_range = true;
            dtostrf(min,0,JsonStream::DOUBLE_DIGITS_DEFAULT,min_str);
            dtostrf(max,0,JsonStream::DOUBLE_DIGITS_DEFAULT,max_str);
          }
        }
        break;
      }
    case JsonStream::BOOL_TYPE:
      break;
    case JsonStream::NULL_TYPE:
      break;
    case JsonStream::STRING_TYPE:
      break;
    case JsonStream::OBJECT_TYPE:
      {
        ArduinoJson::JsonObject& json_object = json_value;
        if (!json_object.success())
        {
          object_parse_unsuccessful = true;
        }
        break;
      }
    case JsonStream::ARRAY_TYPE:
      {
        ArduinoJson::JsonArray& json_array = json_value;
        if (!json_array.success())
        {
          array_parse_unsuccessful = true;
        }
        else
        {
          JsonStream::JsonTypes array_element_type = parameter.getArrayElementType();
          switch (array_element_type)
          {
            case JsonStream::LONG_TYPE:
              {
                if (parameter.rangeIsSet())
                {
                  long value;
                  long min = parameter.getMin().l;
                  long max = parameter.getMax().l;
                  for (ArduinoJson::JsonArray::iterator it=json_array.begin();
                       it!=json_array.end();
                       ++it)
                  {
                    value = (long)*it;
                    if ((value < min) || (value > max))
                    {
                      out_of_range = true;
                      dtostrf(min,0,0,min_str);
                      dtostrf(max,0,0,max_str);
                      break;
                    }
                  }
                }
                break;
              }
            case JsonStream::DOUBLE_TYPE:
              {
                if (parameter.rangeIsSet())
                {
                  double value;
                  double min = parameter.getMin().d;
                  double max = parameter.getMax().d;
                  for (ArduinoJson::JsonArray::iterator it=json_array.begin();
                       it!=json_array.end();
                       ++it)
                  {
                    value = (double)*it;
                    if ((value < min) || (value > max))
                    {
                      out_of_range = true;
                      dtostrf(min,0,JsonStream::DOUBLE_DIGITS_DEFAULT,min_str);
                      dtostrf(max,0,JsonStream::DOUBLE_DIGITS_DEFAULT,max_str);
                      break;
                    }
                  }
                }
                break;
              }
            case JsonStream::BOOL_TYPE:
              break;
            case JsonStream::NULL_TYPE:
              break;
            case JsonStream::STRING_TYPE:
              break;
            case JsonStream::OBJECT_TYPE:
              break;
            case JsonStream::ARRAY_TYPE:
              break;
          }
        }
        break;
      }
  }
  if (out_of_range)
  {
    error_ = true;
    writeKeyToResponse(constants::error_constant_string);
    beginResponseObject();
    writeToResponse(constants::message_constant_string,constants::invalid_params_error_message);
    char error_str[constants::STRING_LENGTH_ERROR];
    error_str[0] = 0;
    if (type != JsonStream::ARRAY_TYPE)
    {
      constants::parameter_error_error_data.copy(error_str);
    }
    else
    {
      constants::array_parameter_error_error_data.copy(error_str);
    }
    strcat(error_str,min_str);
    strcat(error_str," <= ");
    char parameter_name[constants::STRING_LENGTH_PARAMETER_NAME];
    parameter_name[0] = 0;
    const ConstantString& name = parameter.getName();
    name.copy(parameter_name);
    strcat(error_str,parameter_name);
    if (type == JsonStream::ARRAY_TYPE)
    {
      strcat(error_str," element");
    }
    strcat(error_str," <= ");
    strcat(error_str,max_str);
    writeToResponse(constants::data_constant_string,error_str);
    writeToResponse(constants::code_constant_string,constants::invalid_params_error_code);
    endResponseObject();
  }
  else if (object_parse_unsuccessful)
  {
    error_ = true;
    writeKeyToResponse(constants::error_constant_string);
    beginResponseObject();
    writeToResponse(constants::message_constant_string,constants::invalid_params_error_message);
    char parameter_name[constants::STRING_LENGTH_PARAMETER_NAME];
    parameter_name[0] = 0;
    const ConstantString& name = parameter.getName();
    name.copy(parameter_name);
    char error_str[constants::STRING_LENGTH_ERROR];
    error_str[0] = 0;
    strcat(error_str,parameter_name);
    char invalid_json_object[constants::invalid_json_object_error_data.length()+1];
    constants::invalid_json_object_error_data.copy(invalid_json_object);
    strcat(error_str,invalid_json_object);
    writeToResponse(constants::data_constant_string,error_str);
    writeToResponse(constants::code_constant_string,constants::invalid_params_error_code);
    endResponseObject();
  }
  else if (array_parse_unsuccessful)
  {
    error_ = true;
    writeKeyToResponse(constants::error_constant_string);
    beginResponseObject();
    writeToResponse(constants::message_constant_string,constants::invalid_params_error_message);
    char parameter_name[constants::STRING_LENGTH_PARAMETER_NAME];
    parameter_name[0] = 0;
    const ConstantString& name = parameter.getName();
    name.copy(parameter_name);
    char error_str[constants::STRING_LENGTH_ERROR];
    error_str[0] = 0;
    strcat(error_str,parameter_name);
    char invalid_json_array[constants::invalid_json_array_error_data.length()+1];
    constants::invalid_json_array_error_data.copy(invalid_json_array);
    strcat(error_str,invalid_json_array);
    writeToResponse(constants::data_constant_string,error_str);
    writeToResponse(constants::code_constant_string,constants::invalid_params_error_code);
    endResponseObject();
  }
  bool parameter_ok = (!out_of_range) && (!object_parse_unsuccessful) && (!array_parse_unsuccessful);
  return parameter_ok;
}

int Server::findSavedVariableIndex(const ConstantString &saved_variable_name)
{
  int saved_variable_index = -1;
  for (unsigned int i=0; i<saved_variable_array_.size(); ++i)
  {
    if (saved_variable_array_[i].compareName(saved_variable_name))
    {
      saved_variable_index = i;
      break;
    }
  }
  return saved_variable_index;
}

unsigned int Server::getSerialNumber()
{
  unsigned int serial_number;
  getSavedVariableValue(constants::serial_number_constant_string,serial_number);
  return serial_number;
}

void Server::initializeEeprom()
{
  saved_variable_array_[eeprom_initialized_index_].setValue(constants::eeprom_initialized_value);
  eeprom_uninitialized_ = false;
}

void Server::incrementServerStream()
{
  server_stream_index_ = (server_stream_index_ + 1) % server_stream_ptr_array_.size();
  json_stream_.setStream(*server_stream_ptr_array_[server_stream_index_]);
}

void Server::writeDeviceInfoToResponse()
{
  beginResponseObject();
  writeToResponse(constants::name_constant_string,name_ptr_);
  writeToResponse(constants::model_number_constant_string,model_number_);
  writeToResponse(constants::serial_number_constant_string,getSerialNumber());
  writeKeyToResponse(constants::firmware_version_constant_string);
  beginResponseObject();
  writeToResponse(constants::major_constant_string,firmware_major_);
  writeToResponse(constants::minor_constant_string,firmware_minor_);
  writeToResponse(constants::patch_constant_string,firmware_patch_);
  endResponseObject();
  endResponseObject();
}

void Server::getDeviceInfoCallback()
{
  writeResultKeyToResponse();
  writeDeviceInfoToResponse();
}

void Server::getMethodIdsCallback()
{
  writeResultKeyToResponse();
  beginResponseObject();
  for (unsigned int method_index=0; method_index<method_array_.size(); ++method_index)
  {
    if (!method_array_[method_index].isReserved())
    {
      const ConstantString& method_name = method_array_[method_index].getName();
      writeToResponse(method_name,method_index);
    }
  }
  endResponseObject();
}

void Server::getParametersCallback()
{
  writeResultKeyToResponse();
  beginResponseObject();
  writeKeyToResponse(constants::parameters_constant_string);
  json_stream_.beginArray();
  for (unsigned int parameter_index=0; parameter_index<parameter_array_.size(); ++parameter_index)
  {
    parameterHelp(parameter_array_[parameter_index]);
  }
  json_stream_.endArray();
  endResponseObject();
}

void Server::help(bool verbose)
{
  int array_elements_count = countJsonArrayElements((*request_json_array_ptr_));
  int parameter_count = array_elements_count - 1;
  bool param_error = true;
  // ?
  // ??
  if (parameter_count == 0)
  {
    param_error = false;
    writeResultKeyToResponse();
    beginResponseObject();
    writeKeyToResponse(constants::device_info_constant_string);
    writeDeviceInfoToResponse();

    writeKeyToResponse(constants::methods_constant_string);
    beginResponseArray();
    // ?
    if (!verbose)
    {
      for (unsigned int method_index=0; method_index<method_array_.size(); ++method_index)
      {
        if (!method_array_[method_index].isReserved())
        {
          const ConstantString& method_name = method_array_[method_index].getName();
          writeToResponse(method_name);
        }
      }
    }
    // ??
    else
    {
      for (unsigned int method_index=0; method_index<method_array_.size(); ++method_index)
      {
        if (!method_array_[method_index].isReserved())
        {
          methodHelp(false,method_index);
        }
      }
      endResponseArray();

      writeKeyToResponse(constants::parameters_constant_string);
      beginResponseArray();
      for (unsigned int parameter_index=0; parameter_index<parameter_array_.size(); ++parameter_index)
      {
        parameterHelp(parameter_array_[parameter_index]);
      }
    }
    endResponseArray();
    endResponseObject();
  }
  // ? method
  // ? parameter
  // ?? method
  // ?? parameter
  else if (parameter_count == 1)
  {
    const char* param_string = (*request_json_array_ptr_)[1];
    int method_index = findMethodIndex(param_string);
    bool method = false;
    // ? method
    if (method_index >= 0)
    {
      param_error = false;
      method = true;
      writeResultKeyToResponse();
      beginResponseObject();
      writeKeyToResponse(constants::method_info_constant_string);
      methodHelp(verbose,method_index);
      endResponseObject();
    }
    if (!method)
    {
      // ? parameter
      // ?? parameter
      int parameter_index = findParameterIndex(param_string);
      if (parameter_index >= 0)
      {
        param_error = false;
        writeResultKeyToResponse();
        beginResponseObject();
        writeKeyToResponse(constants::parameter_info_constant_string);
        Parameter& parameter = parameter_array_[parameter_index];
        parameterHelp(parameter);
        endResponseObject();
      }
    }
  }
  // ? method parameter
  // ?? method parameter
  else if (parameter_count == 2)
  {
    const char* method_string = (*request_json_array_ptr_)[1];
    int method_index = findMethodIndex(method_string);
    int parameter_index = findMethodParameterIndex(method_index,(const char *)(*request_json_array_ptr_)[2]);
    if (parameter_index >= 0)
    {
      param_error = false;
      Parameter& parameter = *(method_array_[method_index].parameter_ptr_array_[parameter_index]);
      writeResultKeyToResponse();
      beginResponseObject();
      writeKeyToResponse(constants::parameter_info_constant_string);
      parameterHelp(parameter);
      endResponseObject();
    }
  }
  // ? unknown
  // ?? unknown
  // ? method unknown
  // ?? method unknown
  if (param_error)
  {
    error_ = true;
    writeKeyToResponse(constants::error_constant_string);
    beginResponseObject();
    writeToResponse(constants::message_constant_string,constants::invalid_params_error_message);
    writeToResponse(constants::code_constant_string,constants::invalid_params_error_code);
    endResponseObject();
  }
}

void Server::help()
{
  help(false);
}

void Server::verboseHelp()
{
  help(true);
}
}
