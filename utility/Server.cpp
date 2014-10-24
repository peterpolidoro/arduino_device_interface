// ----------------------------------------------------------------------------
// Server.cpp
//
//
// Authors:
// Peter Polidoro polidorop@janelia.hhmi.org
// ----------------------------------------------------------------------------
#include "Server.h"


using namespace ArduinoJson;

namespace RemoteDevice
{

FLASH_STRING(default_device_name,"");
FLASH_STRING(get_device_info_method_name,"getDeviceInfo");
FLASH_STRING(get_method_ids_method_name,"getMethodIds");
FLASH_STRING(get_response_codes_method_name,"getResponseCodes");
FLASH_STRING(help_method_name,"?");
FLASH_STRING(object_request_error_message,"JSON object requests not supported. Must use compact JSON array format for requests.");
FLASH_STRING(array_parse_error_message,"Parsing JSON array request failed! Could be invalid JSON or too many tokens.");

Server::Server(Stream &stream)
{
  setStream(stream);
  setName(default_device_name);
  model_number_ = 0;
  serial_number_ = 0;
  firmware_number_ = 0;
  request_method_index_ = -1;
  parameter_count_ = 0;
  response_ = JsonPrinter(stream);
  error_ = false;

  Method& get_device_info_method = createMethod(get_device_info_method_name);
  get_device_info_method.attachReservedCallback(&Server::getDeviceInfoCallback);

  Method& get_method_ids_method = createMethod(get_method_ids_method_name);
  get_method_ids_method.attachReservedCallback(&Server::getMethodIdsCallback);

  Method& get_response_codes_method = createMethod(get_response_codes_method_name);
  get_response_codes_method.attachReservedCallback(&Server::getResponseCodesCallback);

  Method& help_method = createMethod(help_method_name);
  help_method.attachReservedCallback(&Server::help);
}

void Server::setStream(Stream &stream)
{
  stream_ptr_ = &stream;
}

void Server::handleRequest()
{
  while (stream_ptr_->available() > 0)
  {
    int request_length = stream_ptr_->readBytesUntil(EOL,request_,STRING_LENGTH_REQUEST);
    if (request_length == 0)
    {
      continue;
    }
    request_[request_length] = '\0';
    error_ = false;
    response_.startObject();
    response_.setCompactPrint();
    if (request_[0] == START_CHAR_JSON_OBJECT)
    {
        response_.add("status",ERROR);
        char error_message[STRING_LENGTH_ERROR] = {0};
        object_request_error_message.copy(error_message);
        response_.add("error_message",error_message);
        error_ = true;
    }
    else
    {
      if (request_[0] != START_CHAR_JSON_ARRAY)
      {
        response_.setPrettyPrint();
        String request_string = String("[") + String(request_) + String("]");
        request_string.toCharArray(request_,STRING_LENGTH_REQUEST);
      }
      request_json_array_ = parser_.parse(request_);
      if (request_json_array_.success())
      {
        processRequestArray();
      }
      else
      {
        response_.add("status",ERROR);
        char error_message[STRING_LENGTH_ERROR] = {0};
        array_parse_error_message.copy(error_message);
        response_.add("error_message",error_message);
        response_.add("received_request",request_);
        error_ = true;
      }
      if (!error_)
      {
        response_.add("status",SUCCESS);
      }
      // if (request_type_ == COMMAND_LINE_REQUEST)
      // {
      //   int status = response["status"];
      //   if (response.containsKey("method"))
      //   {
      //     // Make help response a little less cluttered
      //     if (strcmp(response["method"],"?") == 0)
      //     {
      //       response.remove("method");
      //       response.remove("status");
      //     }
      //   }
      //   response.prettyPrintTo(*stream_ptr_);
      // }
      // else
      // {
      //   // response.printTo(*stream_ptr_);
      // }
    }
    response_.stopObject();
    *stream_ptr_ << endl;
  }
}

void Server::setName(_FLASH_STRING &name)
{
  name_ptr_ = &name;
}

void Server::setModelNumber(int model_number)
{
  model_number_ = model_number;
}

void Server::setFirmwareNumber(int firmware_number)
{
  firmware_number_ = firmware_number;
}

Method& Server::createMethod(_FLASH_STRING &method_name)
{
  int method_index = findMethodIndex(method_name);
  if (method_index < 0)
  {
    method_vector_.push_back(Method(method_name));
    return method_vector_.back();
  }
  else
  {
    method_vector_[method_index] = Method(method_name);
    return method_vector_[method_index];
  }
}

Method& Server::copyMethod(Method method)
{
  method_vector_.push_back(method);
  return method_vector_.back();
}

Parameter& Server::createParameter(_FLASH_STRING &parameter_name)
{
  int parameter_index = findParameterIndex(parameter_name);
  if (parameter_index < 0)
  {
    parameter_vector_.push_back(Parameter(parameter_name));
    return parameter_vector_.back();
  }
  else
  {
    parameter_vector_[parameter_index] = Parameter(parameter_name);
    return parameter_vector_[parameter_index];
  }
}

Parameter& Server::copyParameter(Parameter parameter)
{
  parameter_vector_.push_back(parameter);
  return parameter_vector_.back();
}

Parser::JsonValue Server::getParameterValue(_FLASH_STRING &name)
{
  int parameter_index = findParameterIndex(name);
  return request_json_array_[parameter_index+1];
}

void Server::processRequestArray()
{
  char* method_string = request_json_array_[0];
  request_method_index_ = processMethodString(method_string);
  if (!(request_method_index_ < 0))
  {
    int array_elements_count = countJsonArrayElements(request_json_array_);
    int parameter_count = array_elements_count - 1;
    if ((parameter_count == 1) && (String((char*)request_json_array_[1]).equals("?")))
    {
      methodHelp();
    }
    else if ((parameter_count == 2) && (String((char*)request_json_array_[2]).equals("?")))
    {
      int parameter_index = processParameterString(request_json_array_[1]);
      parameterHelp(parameter_index);
    }
    else if (parameter_count != method_vector_[request_method_index_].parameter_count_)
    {
      response_.add("status",ERROR);
      String error_request = "Incorrect number of parameters. ";
      error_request += String(parameter_count) + String(" given. ");
      error_request += String(method_vector_[request_method_index_].parameter_count_);
      error_request += String(" needed.");
      char error_str[STRING_LENGTH_ERROR];
      error_request.toCharArray(error_str,STRING_LENGTH_ERROR);
      response_.add("error_message",error_str);
      error_ = true;
    }
    else
    {
      boolean parameters_ok = checkParameters();
      if (parameters_ok)
      {
        executeMethod();
      }
    }
  }
}

boolean Server::checkParameters()
{
  int parameter_index = 0;
  for (Parser::JsonArrayIterator it=request_json_array_.begin(); it!=request_json_array_.end(); ++it)
  {
    if (it!=request_json_array_.begin())
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

void Server::executeMethod()
{
  if (method_vector_[request_method_index_].isReserved())
  {
    method_vector_[request_method_index_].reservedCallback(this);
  }
  else
  {
    method_vector_[request_method_index_].callback();
  }
}

void Server::methodHelp()
{
  int parameter_index = 0;
  response_.addKey("parameters");
  response_.startArray();
  std::vector<Parameter*>& parameter_ptr_vector = method_vector_[request_method_index_].parameter_ptr_vector_;
  _FLASH_STRING* parameter_name_ptr;
  char parameter_name_char_array[STRING_LENGTH_PARAMETER_NAME];
  for (std::vector<Parameter*>::iterator it = parameter_ptr_vector.begin();
       it != parameter_ptr_vector.end();
       ++it)
  {
    if (parameter_index < PARAMETER_COUNT_MAX)
    {
      parameter_name_ptr = (*it)->getNamePointer();
      parameter_name_ptr->copy(parameter_name_char_array);
      // Serial << "parameter_name_array_[parameter_index]: " << parameter_name_array_[parameter_index] << endl;
      // Serial << "parameter_name_ptr.length(): " << parameter_name_ptr->length() << endl;
      response_.add(parameter_name_char_array);
      parameter_index++;
    }
  }
  response_.stopArray();
}

void Server::parameterHelp(int parameter_index)
{
  // char method_name[STRING_LENGTH_METHOD_NAME] = {0};
  // _FLASH_STRING* method_name_ptr = method_vector_[request_method_index_].getNamePointer();
  // method_name_ptr->copy(method_name);

  // parameter_help_object_ = Generator::JsonObject<JSON_OBJECT_SIZE_PARAMETER_HELP>();
  // Parameter& parameter = *(method_vector_[request_method_index_].parameter_ptr_vector_[parameter_index]);
  // char parameter_name[STRING_LENGTH_PARAMETER_NAME] = {0};
  // _FLASH_STRING* parameter_name_ptr = parameter.getNamePointer();
  // parameter_name_ptr->copy(parameter_name);
  // char parameter_units[STRING_LENGTH_PARAMETER_UNITS] = {0};
  // _FLASH_STRING* parameter_units_ptr = parameter.getUnitsPointer();
  // parameter_units_ptr->copy(parameter_units);

  // parameter_help_object_["name"] = parameter_name;
  // parameter_help_object_["position"] = parameter_index;
  // parameter_help_object_["method"] = method_name;
  // if (strcmp(parameter_units,"") != 0)
  // {
  //   parameter_help_object_["units"] = parameter_units;
  // }
  // ParameterType type = parameter.getType();
  // switch (type)
  // {
  //   case LONG_PARAMETER:
  //     parameter_help_object_["type"] = "long";
  //     if (parameter.rangeIsSet())
  //     {
  //       long min = parameter.getMin().l;
  //       long max = parameter.getMax().l;
  //       parameter_help_object_["min"] = min;
  //       parameter_help_object_["max"] = max;
  //     }
  //     break;
  //   case DOUBLE_PARAMETER:
  //     parameter_help_object_["type"] = "double";
  //     if (parameter.rangeIsSet())
  //     {
  //       double min = parameter.getMin().d;
  //       double max = parameter.getMax().d;
  //       parameter_help_object_["min"].set<DOUBLE_DIGITS>(min);
  //       parameter_help_object_["max"].set<DOUBLE_DIGITS>(max);
  //     }
  //     break;
  //   case STRING_PARAMETER:
  //     parameter_help_object_["type"] = "string";
  //     break;
  // }
  // response_.add("parameter",parameter_help_object_);
}

boolean Server::checkParameter(int parameter_index, Parser::JsonValue json_value)
{
  boolean parameter_ok = true;
  Parameter& parameter = *(method_vector_[request_method_index_].parameter_ptr_vector_[parameter_index]);
  ParameterType type = parameter.getType();
  String min_string = "";
  String max_string = "";
  switch (type)
  {
    case LONG_PARAMETER:
      if (parameter.rangeIsSet())
      {
        long value = (long)json_value;
        long min = parameter.getMin().l;
        long max = parameter.getMax().l;
        if ((value < min) || (value > max))
        {
          parameter_ok = false;
          min_string = String(min);
          max_string = String(max);
        }
      }
      break;
    case DOUBLE_PARAMETER:
      if (parameter.rangeIsSet())
      {
        double value = (double)json_value;
        double min = parameter.getMin().d;
        double max = parameter.getMax().d;
        if ((value < min) || (value > max))
        {
          parameter_ok = false;
          char temp_string[10];
          dtostrf(min,DOUBLE_DIGITS,DOUBLE_DIGITS,temp_string);
          min_string = String(temp_string);
          dtostrf(max,DOUBLE_DIGITS,DOUBLE_DIGITS,temp_string);
          max_string = String(temp_string);
        }
      }
      break;
    case STRING_PARAMETER:
      break;
  }
  if (!parameter_ok)
  {
    response_.add("status",ERROR);
    String error = String("Parameter value out of range: ");
    error += min_string;
    error += String(" <= ");
    char parameter_name[STRING_LENGTH_PARAMETER_NAME] = {0};
    _FLASH_STRING* parameter_name_ptr = parameter.getNamePointer();
    parameter_name_ptr->copy(parameter_name);
    error += String(parameter_name);
    error += String(" <= ");
    error += max_string;
    char error_str[STRING_LENGTH_ERROR];
    error.toCharArray(error_str,STRING_LENGTH_ERROR);
    response_.add("error_message",error_str);
    error_ = true;
  }
  return parameter_ok;
}

int Server::processMethodString(char *method_string)
{
  int method_index = -1;
  int method_id = String(method_string).toInt();
  if (String(method_string).compareTo("0") == 0)
  {
    method_index = 0;
    response_.add("method_id",0);
  }
  else if (method_id > 0)
  {
    method_index = method_id;
    response_.add("method_id",method_id);
  }
  else
  {
    method_index = findMethodIndex(method_string);
    response_.add("method",method_string);
  }
  if ((method_index < 0) || (method_index >= method_vector_.size()))
  {
    response_.add("status",ERROR);
    response_.add("error_message","Unknown method.");
    error_ = true;
    method_index = -1;
  }
  return method_index;
}

int Server::findMethodIndex(char *method_name)
{
  int method_index = -1;
  for (std::vector<Method>::iterator it = method_vector_.begin();
       it != method_vector_.end();
       ++it)
  {
    if (it->compareName(method_name))
    {
      method_index = std::distance(method_vector_.begin(),it);
      break;
    }
  }
  return method_index;
}

int Server::findMethodIndex(_FLASH_STRING &method_name)
{
  int method_index = -1;
  for (std::vector<Method>::iterator it = method_vector_.begin();
       it != method_vector_.end();
       ++it)
  {
    if (it->compareName(method_name))
    {
      method_index = std::distance(method_vector_.begin(),it);
      break;
    }
  }
  return method_index;
}

int Server::processParameterString(char *parameter_string)
{
  int parameter_index = -1;
  int parameter_id = String(parameter_string).toInt();
  if (String(parameter_string).compareTo("0") == 0)
  {
    parameter_index = 0;
  }
  else if (parameter_id > 0)
  {
    parameter_index = parameter_id;
  }
  else
  {
    parameter_index = findParameterIndex(parameter_string);
  }
  std::vector<Parameter*>& parameter_ptr_vector = method_vector_[request_method_index_].parameter_ptr_vector_;
  if ((parameter_index < 0) || (parameter_index >= parameter_ptr_vector.size()))
  {
    response_.add("status",ERROR);
    response_.add("error_message","Unknown parameter.");
    error_ = true;
    parameter_index = -1;
  }
  return parameter_index;
}

int Server::findParameterIndex(const char *parameter_name)
{
  int parameter_index = -1;
  if (request_method_index_ >= 0)
  {
    std::vector<Parameter*>& parameter_ptr_vector = method_vector_[request_method_index_].parameter_ptr_vector_;
    for (std::vector<Parameter*>::iterator it = parameter_ptr_vector.begin();
         it != parameter_ptr_vector.end();
         ++it)
    {
      if ((*it)->compareName(parameter_name))
      {
        parameter_index = std::distance(parameter_ptr_vector.begin(),it);
        break;
      }
    }
  }
  return parameter_index;
}

int Server::findParameterIndex(_FLASH_STRING &parameter_name)
{
  int parameter_index = -1;
  if (request_method_index_ >= 0)
  {
    std::vector<Parameter*>& parameter_ptr_vector = method_vector_[request_method_index_].parameter_ptr_vector_;
    for (std::vector<Parameter*>::iterator it = parameter_ptr_vector.begin();
         it != parameter_ptr_vector.end();
         ++it)
    {
      if ((*it)->compareName(parameter_name))
      {
        parameter_index = std::distance(parameter_ptr_vector.begin(),it);
        break;
      }
    }
  }
  return parameter_index;
}

int Server::countJsonArrayElements(Parser::JsonArray &json_array)
{
  int elements_count = 0;
  for (Parser::JsonArrayIterator i=json_array.begin(); i!=json_array.end(); ++i)
  {
    elements_count++;
  }
  return elements_count;
}

void Server::getDeviceInfoCallback()
{
  char device_name[STRING_LENGTH_DEVICE_NAME] = {0};
  name_ptr_->copy(device_name);
  response_.add("name",device_name);
  response_.add("model_number",model_number_);
  response_.add("serial_number",serial_number_);
  response_.add("firmware_number",firmware_number_);
}

void Server::getMethodIdsCallback()
{
  char method_name[METHOD_COUNT_MAX][STRING_LENGTH_METHOD_NAME] = {0};
  _FLASH_STRING* method_name_ptr;
  for (std::vector<Method>::iterator it = method_vector_.begin();
       it != method_vector_.end();
       ++it)
  {
    int method_index = std::distance(method_vector_.begin(),it);
    if (!method_vector_[method_index].isReserved())
    {
      method_name_ptr = it->getNamePointer();
      method_name_ptr->copy(method_name[method_index]);
      int method_id = method_index;
      response_.add(method_name[method_index],method_id);
    }
  }
}

void Server::getResponseCodesCallback()
{
  response_.add("response_success",SUCCESS);
  response_.add("response_error",ERROR);
}

void Server::help()
{
  // Generator::JsonObject<JSON_OBJECT_SIZE_DEVICE_INFO> device_info;
  // char device_name[STRING_LENGTH_DEVICE_NAME] = {0};
  // name_ptr_->copy(device_name);
  // device_info["name"] = device_name;
  // device_info["model_number"] = model_number_;
  // device_info["serial_number"] = serial_number_;
  // device_info["firmware_number"] = firmware_number_;
  // response_.add("device_info",device_info);
  // Generator::JsonArray<METHOD_COUNT_MAX> methods;
  // char method_name[METHOD_COUNT_MAX][STRING_LENGTH_METHOD_NAME] = {0};
  // _FLASH_STRING* method_name_ptr;
  // for (std::vector<Method>::iterator it = method_vector_.begin();
  //      it != method_vector_.end();
  //      ++it)
  // {
  //   int method_index = std::distance(method_vector_.begin(),it);
  //   if (!method_vector_[method_index].isReserved())
  //   {
  //     method_name_ptr = it->getNamePointer();
  //     method_name_ptr->copy(method_name[method_index]);
  //     methods.add(method_name[method_index]);
  //   }
  // }
  // response_.add("methods",methods);
}
}