// ----------------------------------------------------------------------------
// Property.cpp
//
//
// Authors:
// Peter Polidoro polidorop@janelia.hhmi.org
// ----------------------------------------------------------------------------
#include "Property.h"


namespace modular_server
{

namespace property
{
// Parameters
CONSTANT_STRING(value_parameter_name,"value");

// Array Parameters
CONSTANT_STRING(element_index_parameter_name,"element_index");

// Methods
CONSTANT_STRING(get_value_method_name,"getValue");
CONSTANT_STRING(set_value_method_name,"setValue");
CONSTANT_STRING(get_default_value_method_name,"getDefaultValue");
CONSTANT_STRING(set_value_to_default_method_name,"setValueToDefault");

// Array Methods
CONSTANT_STRING(get_element_value_method_name,"getElementValue");
CONSTANT_STRING(set_element_value_method_name,"setElementValue");
CONSTANT_STRING(get_default_element_value_method_name,"getDefaultElementValue");
CONSTANT_STRING(set_element_value_to_default_method_name,"setElementValueToDefault");
CONSTANT_STRING(set_all_element_values_method_name,"setAllElementValues");
}

Array<Parameter,Property::PARAMETER_COUNT_MAX> Property::parameters_;
Array<Parameter,Property::ARRAY_PARAMETER_COUNT_MAX> Property::array_parameters_;
Array<Method,Property::METHOD_COUNT_MAX> Property::methods_;
Array<Method,Property::ARRAY_METHOD_COUNT_MAX> Property::array_methods_;

int Property::findParameterIndex(const ConstantString & parameter_name)
{
  int parameter_index = -1;
  for (size_t i=0; i<parameters_.size(); ++i)
  {
    if (parameters_[i].compareName(parameter_name))
    {
      parameter_index = i;
      break;
    }
  }
  if (parameter_index < 0)
  {
    for (size_t i=0; i<array_parameters_.size(); ++i)
    {
      if (array_parameters_[i].compareName(parameter_name))
      {
        parameter_index = i + parameters_.size();
        break;
      }
    }
  }
  return parameter_index;
}

Parameter & Property::createParameter(const ConstantString & parameter_name)
{
  int parameter_index = findParameterIndex(parameter_name);
  if (parameter_index < 0)
  {
    parameters_.push_back(Parameter(parameter_name));
    parameters_.back().setFirmwareName(constants::firmware_name);
    return parameters_.back();
  }
}

Parameter & Property::createArrayParameter(const ConstantString & parameter_name)
{
  int parameter_index = findParameterIndex(parameter_name);
  if (parameter_index < 0)
  {
    array_parameters_.push_back(Parameter(parameter_name));
    array_parameters_.back().setFirmwareName(constants::firmware_name);
    return array_parameters_.back();
  }
}

Parameter & Property::parameter(const ConstantString & parameter_name)
{
  int parameter_index = findParameterIndex(parameter_name);
  if ((parameter_index >= 0) && (parameter_index < (int)parameters_.size()))
  {
    return parameters_[parameter_index];
  }
  else if (parameter_index >= (int)parameters_.size())
  {
    parameter_index -= parameters_.size();
    return array_parameters_[parameter_index];
  }
}

int Property::findMethodIndex(const ConstantString & method_name)
{
  int method_index = -1;
  for (size_t i=0; i<methods_.size(); ++i)
  {
    if (methods_[i].compareName(method_name))
    {
      method_index = i;
      break;
    }
  }
  if (method_index < 0)
  {
    for (size_t i=0; i<array_methods_.size(); ++i)
    {
      if (array_methods_[i].compareName(method_name))
      {
        method_index = i + methods_.size();
        break;
      }
    }
  }
  return method_index;
}

Method & Property::createMethod(const ConstantString & method_name)
{
  int method_index = findMethodIndex(method_name);
  if (method_index < 0)
  {
    methods_.push_back(Method(method_name));
    methods_.back().setFirmwareName(constants::firmware_name);
    return methods_.back();
  }
}

Method & Property::createArrayMethod(const ConstantString & method_name)
{
  int method_index = findMethodIndex(method_name);
  if (method_index < 0)
  {
    array_methods_.push_back(Method(method_name));
    array_methods_.back().setFirmwareName(constants::firmware_name);
    return array_methods_.back();
  }
}

Method & Property::method(const ConstantString & method_name)
{
  int method_index = findMethodIndex(method_name);
  if ((method_index >= 0) && (method_index < (int)methods_.size()))
  {
    return methods_[method_index];
  }
  else if (method_index >= (int)methods_.size())
  {
    method_index -= methods_.size();
    return array_methods_[method_index];
  }
}

// public
Property::Property()
{
}

void Property::setUnits(const ConstantString & name)
{
  parameter_.setUnits(name);
}

void Property::setRange(const long min, const long max)
{
  parameter_.setRange(min,max);
}

void Property::setRange(const double min, const double max)
{
  parameter_.setRange(min,max);
}

void Property::setSubset(constants::SubsetMemberType * subset, size_t max_size, size_t size)
{
  parameter_.setSubset(subset,max_size,size);
}

void Property::addValueToSubset(constants::SubsetMemberType & value)
{
  parameter_.addValueToSubset(value);
}

void Property::attachPreSetValueFunctor(const Functor0 & functor)
{
  pre_set_value_functor_ = functor;
}

void Property::attachPreSetElementValueFunctor(const Functor1<const size_t> & functor)
{
  pre_set_element_value_functor_ = functor;
}

void Property::attachPostSetValueFunctor(const Functor0 & functor)
{
  post_set_value_functor_ = functor;
}

void Property::attachPostSetElementValueFunctor(const Functor1<const size_t> & functor)
{
  post_set_element_value_functor_ = functor;
}

template <>
bool Property::getValue<long>(long & value)
{
  if (getType() != JsonStream::LONG_TYPE)
  {
    return false;
  }
  return saved_variable_.getValue(value);
}

template <>
bool Property::getValue<double>(double & value)
{
  if (getType() != JsonStream::DOUBLE_TYPE)
  {
    return false;
  }
  return saved_variable_.getValue(value);
}

template <>
bool Property::getValue<bool>(bool & value)
{
  if (getType() != JsonStream::BOOL_TYPE)
  {
    return false;
  }
  return saved_variable_.getValue(value);
}

bool Property::getValue(const ConstantString * & value)
{
  if ((getType() != JsonStream::STRING_TYPE) ||
      stringIsSavedAsCharArray())
  {
    value = NULL;
    return false;
  }
  return saved_variable_.getValue(value);
}

template <>
bool Property::getElementValue<long>(const size_t element_index, long & value)
{
  if ((getType() != JsonStream::ARRAY_TYPE) ||
      (getArrayElementType() != JsonStream::LONG_TYPE))
  {
    return false;
  }
  return saved_variable_.getElementValue(element_index,value);
}

template <>
bool Property::getElementValue<double>(const size_t element_index, double & value)
{
  if ((getType() != JsonStream::ARRAY_TYPE) ||
      (getArrayElementType() != JsonStream::DOUBLE_TYPE))
  {
    return false;
  }
  return saved_variable_.getElementValue(element_index,value);
}

template <>
bool Property::getElementValue<bool>(const size_t element_index, bool & value)
{
  if ((getType() != JsonStream::ARRAY_TYPE) ||
      (getArrayElementType() != JsonStream::BOOL_TYPE))
  {
    return false;
  }
  return saved_variable_.getElementValue(element_index,value);
}

template <>
bool Property::getElementValue<char>(const size_t element_index, char & value)
{
  if (getType() != JsonStream::STRING_TYPE)
  {
    return false;
  }
  if (!stringIsSavedAsCharArray())
  {
    const ConstantString * cs_ptr;
    getValue(cs_ptr);
    size_t str_len = cs_ptr->length();
    if (element_index <= str_len)
    {
      char str[str_len+1];
      cs_ptr->copy(str);
      value = str[element_index];
      return true;
    }
    else
    {
      return false;
    }
  }
  return saved_variable_.getElementValue(element_index,value);
}

template <>
bool Property::getDefaultValue<long>(long & value)
{
  if (getType() != JsonStream::LONG_TYPE)
  {
    return false;
  }
  return saved_variable_.getDefaultValue(value);
}

template <>
bool Property::getDefaultValue<double>(double & value)
{
  if (getType() != JsonStream::DOUBLE_TYPE)
  {
    return false;
  }
  return saved_variable_.getDefaultValue(value);
}

template <>
bool Property::getDefaultValue<bool>(bool & value)
{
  if (getType() != JsonStream::BOOL_TYPE)
  {
    return false;
  }
  return saved_variable_.getDefaultValue(value);
}

bool Property::getDefaultValue(const ConstantString * & value)
{
  if ((getType() != JsonStream::STRING_TYPE) ||
      stringIsSavedAsCharArray())
  {
    value = NULL;
    return false;
  }
  return saved_variable_.getDefaultValue(value);
}

template <>
bool Property::getDefaultElementValue<long>(const size_t element_index, long & value)
{
  if ((getType() != JsonStream::ARRAY_TYPE) ||
      (getArrayElementType() != JsonStream::LONG_TYPE))
  {
    return false;
  }
  return saved_variable_.getDefaultElementValue(element_index,value);
}

template <>
bool Property::getDefaultElementValue<double>(const size_t element_index, double & value)
{
  if ((getType() != JsonStream::ARRAY_TYPE) ||
      (getArrayElementType() != JsonStream::DOUBLE_TYPE))
  {
    return false;
  }
  return saved_variable_.getDefaultElementValue(element_index,value);
}

template <>
bool Property::getDefaultElementValue<bool>(const size_t element_index, bool & value)
{
  if ((getType() != JsonStream::ARRAY_TYPE) ||
      (getArrayElementType() != JsonStream::BOOL_TYPE))
  {
    return false;
  }
  return saved_variable_.getDefaultElementValue(element_index,value);
}

template <>
bool Property::getDefaultElementValue<char>(const size_t element_index, char & value)
{
  if (getType() != JsonStream::STRING_TYPE)
  {
    return false;
  }
  if (!stringIsSavedAsCharArray())
  {
    const ConstantString * cs_ptr;
    getDefaultValue(cs_ptr);
    size_t str_len = cs_ptr->length();
    if (element_index <= str_len)
    {
      char str[str_len+1];
      cs_ptr->copy(str);
      value = str[element_index];
      return true;
    }
    else
    {
      return false;
    }
  }
  return saved_variable_.getDefaultElementValue(element_index,value);
}

template <>
bool Property::setElementValue<long>(const size_t element_index, const long & value)
{
  bool success = false;
  preSetElementValueFunctor(element_index);
  if ((getType() == JsonStream::ARRAY_TYPE) &&
      (getArrayElementType() == JsonStream::LONG_TYPE))
  {
    if (parameter_.rangeIsSet())
    {
      long min = parameter_.getMin().l;
      long max = parameter_.getMax().l;
      if ((value >= min) && (value <= max))
      {
        success = saved_variable_.setElementValue(element_index,value);
      }
    }
    else
    {
      success = saved_variable_.setElementValue(element_index,value);
    }
  }
  postSetElementValueFunctor(element_index);
  return success;
}

template <>
bool Property::setElementValue<double>(const size_t element_index, const double & value)
{
  bool success = false;
  preSetElementValueFunctor(element_index);
  if ((getType() == JsonStream::ARRAY_TYPE) &&
      (getArrayElementType() == JsonStream::DOUBLE_TYPE))
  {
    if (parameter_.rangeIsSet())
    {
      double min = parameter_.getMin().d;
      double max = parameter_.getMax().d;
      if ((value >= min) && (value <= max))
      {
        success = saved_variable_.setElementValue(element_index,value);
      }
    }
    else
    {
      success = saved_variable_.setElementValue(element_index,value);
    }
  }
  postSetElementValueFunctor(element_index);
  return success;
}

template <>
bool Property::setElementValue<bool>(const size_t element_index, const bool & value)
{
  bool success = false;
  preSetElementValueFunctor(element_index);
  if ((getType() == JsonStream::ARRAY_TYPE) &&
      (getArrayElementType() == JsonStream::BOOL_TYPE))
  {
    success = saved_variable_.setElementValue(element_index,value);
  }
  postSetElementValueFunctor(element_index);
  return success;
}

template <>
bool Property::setElementValue<char>(const size_t element_index, const char & value)
{
  bool success = false;
  preSetElementValueFunctor(element_index);
  if ((getType() == JsonStream::STRING_TYPE) &&
      (stringIsSavedAsCharArray()))
  {
    success = saved_variable_.setElementValue(element_index,value);
  }
  postSetElementValueFunctor(element_index);
  return success;
}

template <>
bool Property::setValue<long>(const long & value)
{
  bool success = false;
  preSetValueFunctor();
  if (getType() == JsonStream::LONG_TYPE)
  {
    if (parameter_.rangeIsSet())
    {
      long min = parameter_.getMin().l;
      long max = parameter_.getMax().l;
      if ((value >= min) && (value <= max))
      {
        success = saved_variable_.setValue(value);
      }
    }
    else
    {
      success = saved_variable_.setValue(value);
    }
  }
  postSetValueFunctor();
  return success;
}

template <>
bool Property::setValue<double>(const double & value)
{
  bool success = false;
  preSetValueFunctor();
  if (getType() == JsonStream::DOUBLE_TYPE)
  {
    if (parameter_.rangeIsSet())
    {
      double min = parameter_.getMin().d;
      double max = parameter_.getMax().d;
      if ((value >= min) && (value <= max))
      {
        success = saved_variable_.setValue(value);
      }
    }
    else
    {
      success = saved_variable_.setValue(value);
    }
  }
  postSetValueFunctor();
  return success;
}

template <>
bool Property::setValue<bool>(const bool & value)
{
  bool success = false;
  preSetValueFunctor();
  if (getType() == JsonStream::BOOL_TYPE)
  {
    success = saved_variable_.setValue(value);
  }
  postSetValueFunctor();
  return success;
}

template <>
bool Property::setValue<const ConstantString *>(const ConstantString * const & value)
{
  bool success = false;
  preSetValueFunctor();
  if ((getType() == JsonStream::STRING_TYPE) &&
      !stringIsSavedAsCharArray())
  {
    success = saved_variable_.setValue(value);
  }
  postSetValueFunctor();
  return success;
}

template <>
bool Property::setValue<ConstantString *>(ConstantString * const & value)
{
  bool success = false;
  preSetValueFunctor();
  if ((getType() == JsonStream::STRING_TYPE) &&
      !stringIsSavedAsCharArray())
  {
    success = saved_variable_.setValue(value);
  }
  postSetValueFunctor();
  return success;
}

bool Property::setValue(ArduinoJson::JsonArray & value)
{
  bool success = false;
  size_t array_length = getArrayLength();
  JsonStream::JsonTypes property_type = getType();
  if (property_type == JsonStream::ARRAY_TYPE)
  {
    size_t N = value.size();
    size_t array_length_min = min(array_length,N);
    JsonStream::JsonTypes array_element_type = getArrayElementType();
    preSetValueFunctor();
    switch (array_element_type)
    {
      case JsonStream::LONG_TYPE:
      {
        for (size_t i=0; i<array_length_min; ++i)
        {
          long v = value[i];
          success = setElementValue<long>(i,v);
          if (!success)
          {
            break;
          }
        }
        break;
      }
      case JsonStream::DOUBLE_TYPE:
      {
        for (size_t i=0; i<array_length_min; ++i)
        {
          double v = value[i];
          success = setElementValue<double>(i,v);
          if (!success)
          {
            break;
          }
        }
        break;
      }
      case JsonStream::BOOL_TYPE:
      {
        for (size_t i=0; i<array_length_min; ++i)
        {
          bool v = value[i];
          success = setElementValue<bool>(i,v);
          if (!success)
          {
            break;
          }
        }
        break;
      }
      case JsonStream::NULL_TYPE:
      {
        break;
      }
      case JsonStream::STRING_TYPE:
      {
        break;
      }
      case JsonStream::OBJECT_TYPE:
      {
        break;
      }
      case JsonStream::ARRAY_TYPE:
      {
        break;
      }
      case JsonStream::ANY_TYPE:
      {
        break;
      }
    }
    postSetValueFunctor();
  }
  return success;
}

void Property::setValueToDefault()
{
  preSetValueFunctor();
  saved_variable_.setValueToDefault();
  postSetValueFunctor();
}

void Property::setElementValueToDefault(const size_t element_index)
{
  preSetElementValueFunctor(element_index);
  saved_variable_.setElementValueToDefault(element_index);
  postSetElementValueFunctor(element_index);
}

bool Property::valueIsDefault()
{
  return saved_variable_.valueIsDefault();
}

size_t Property::getArrayLength()
{
  if ((getType() == JsonStream::STRING_TYPE) &&
      (!stringIsSavedAsCharArray()))
  {
    const ConstantString * cs_ptr;
    getValue(cs_ptr);
    return cs_ptr->length() + 1;
  }
  return saved_variable_.getArrayLength();
}

size_t Property::getStringLength()
{
  size_t length = 0;
  JsonStream::JsonTypes property_type = getType();
  if (property_type == JsonStream::STRING_TYPE)
  {
    size_t array_length_max = getArrayLength();
    size_t array_length = 1;
    char value;
    while (array_length < array_length_max)
    {
      getElementValue(array_length-1,value);
      if (value == 0)
      {
        return array_length - 1;
      }
      ++array_length;
    }
    return array_length_max;
  }
  return length;
}

// private
template <>
Property::Property<long>(const ConstantString & name,
                         const long & default_value) :
parameter_(name),
  saved_variable_(default_value)
{
  parameter_.setTypeLong();
}

template <>
Property::Property<double>(const ConstantString & name,
                           const double & default_value) :
parameter_(name),
  saved_variable_(default_value)
{
  parameter_.setTypeDouble();
}

template <>
Property::Property<bool>(const ConstantString & name,
                         const bool & default_value) :
parameter_(name),
  saved_variable_(default_value)
{
  parameter_.setTypeBool();
}

template <>
Property::Property<const ConstantString *>(const ConstantString & name,
                                           const ConstantString * const & default_value) :
parameter_(name),
  saved_variable_(default_value)
{
  parameter_.setTypeString();
  string_saved_as_char_array_ = false;
}

Parameter & Property::parameter()
{
  return parameter_;
}

bool Property::compareName(const char * name_to_compare)
{
  return parameter_.compareName(name_to_compare);
}

bool Property::compareName(const ConstantString & name_to_compare)
{
  return parameter_.compareName(name_to_compare);
}

const ConstantString & Property::getName()
{
  return parameter_.getName();
}

const ConstantString & Property::getFirmwareName()
{
  return parameter_.getFirmwareName();
}

bool Property::firmwareNameInArray(ArduinoJson::JsonArray & firmware_name_array)
{
  return parameter_.firmwareNameInArray(firmware_name_array);
}

JsonStream::JsonTypes Property::getType()
{
  return parameter_.getType();
}

JsonStream::JsonTypes Property::getArrayElementType()
{
  return parameter_.getArrayElementType();
}

bool Property::stringIsSavedAsCharArray()
{
  return string_saved_as_char_array_;
}

int Property::findSubsetValueIndex(const long value)
{
  return parameter_.findSubsetValueIndex(value);
}

int Property::findSubsetValueIndex(const char * value)
{
  return parameter_.findSubsetValueIndex(value);
}

Vector<constants::SubsetMemberType> & Property::getSubset()
{
  return parameter_.getSubset();
}

void Property::preSetValueFunctor()
{
  if (pre_set_value_functor_)
  {
    pre_set_value_functor_();
  }
}

void Property::preSetElementValueFunctor(const size_t element_index)
{
  if (pre_set_element_value_functor_)
  {
    pre_set_element_value_functor_(element_index);
  }
}

void Property::postSetValueFunctor()
{
  if (post_set_value_functor_)
  {
    post_set_value_functor_();
  }
}

void Property::postSetElementValueFunctor(const size_t element_index)
{
  if (post_set_element_value_functor_)
  {
    post_set_element_value_functor_(element_index);
  }
}

}
