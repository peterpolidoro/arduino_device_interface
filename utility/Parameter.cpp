// ----------------------------------------------------------------------------
// Parameter.cpp
//
//
// Authors:
// Peter Polidoro polidorop@janelia.hhmi.org
// ----------------------------------------------------------------------------
#include "Parameter.h"


namespace ModularDevice
{
Parameter::Parameter()
{
  setName(constants::empty_constant_string);
  setUnits(constants::empty_constant_string);
  type_ = JsonStream::LONG_TYPE;
  array_element_type_ = JsonStream::LONG_TYPE;
  range_is_set_ = false;
  array_length_range_is_set_ = false;
}

Parameter::Parameter(const ConstantString &name)
{
  setName(name);
  setUnits(constants::empty_constant_string);
  type_ = JsonStream::LONG_TYPE;
  array_element_type_ = JsonStream::LONG_TYPE;
  range_is_set_ = false;
  array_length_range_is_set_ = false;
}

void Parameter::setName(const ConstantString &name)
{
  name_ptr_ = &name;
}

void Parameter::setUnits(const ConstantString &units)
{
  units_ptr_ = &units;
}

void Parameter::setTypeLong()
{
  if (type_ != JsonStream::ARRAY_TYPE)
  {
    type_ = JsonStream::LONG_TYPE;
  }
  else
  {
    array_element_type_ = JsonStream::LONG_TYPE;
  }
}

void Parameter::setTypeDouble()
{
  if (type_ != JsonStream::ARRAY_TYPE)
  {
    type_ = JsonStream::DOUBLE_TYPE;
  }
  else
  {
    array_element_type_ = JsonStream::DOUBLE_TYPE;
  }
}

void Parameter::setTypeBool()
{
  if (type_ != JsonStream::ARRAY_TYPE)
  {
    type_ = JsonStream::BOOL_TYPE;
  }
  else
  {
    array_element_type_ = JsonStream::BOOL_TYPE;
  }
}

void Parameter::setTypeString()
{
  if (type_ != JsonStream::ARRAY_TYPE)
  {
    type_ = JsonStream::STRING_TYPE;
  }
  else
  {
    array_element_type_ = JsonStream::STRING_TYPE;
  }
}

void Parameter::setTypeObject()
{
  type_ = JsonStream::OBJECT_TYPE;
}

void Parameter::setTypeArray()
{
  if (type_ != JsonStream::ARRAY_TYPE)
  {
    array_element_type_ = type_;
    type_ = JsonStream::ARRAY_TYPE;
  }
}

void Parameter::setTypeValue()
{
  if (type_ != JsonStream::ARRAY_TYPE)
  {
    type_ = JsonStream::VALUE_TYPE;
  }
  else
  {
    array_element_type_ = JsonStream::VALUE_TYPE;
  }
}

void Parameter::setRange(const double min, const double max)
{
  min_.d = min;
  max_.d = max;
  setTypeDouble();
  range_is_set_ = true;
}

void Parameter::setRange(const float min, const float max)
{
  min_.d = (double)min;
  max_.d = (double)max;
  setTypeDouble();
  range_is_set_ = true;
}

void Parameter::removeRange()
{
  range_is_set_ = false;
}

void Parameter::setArrayLengthRange(const unsigned char array_length_min,
                                    const unsigned char array_length_max)
{
  setTypeArray();
  array_length_min_ = array_length_min;
  array_length_max_ = array_length_max;
  array_length_range_is_set_ = true;
}

void Parameter::removeArrayLengthRange()
{
  array_length_range_is_set_ = false;
}

bool Parameter::compareName(const char *name_to_compare)
{
  char name[constants::STRING_LENGTH_PARAMETER_NAME] = {0};
  name_ptr_->copy(name);
  return String(name).equalsIgnoreCase(name_to_compare);
}

bool Parameter::compareName(const ConstantString &name_to_compare)
{
  return (&name_to_compare == name_ptr_);
}

const ConstantString& Parameter::getName()
{
  return *name_ptr_;
}

const ConstantString& Parameter::getUnits()
{
  return *units_ptr_;
}

JsonStream::JsonTypes Parameter::getType()
{
  return type_;
}

JsonStream::JsonTypes Parameter::getArrayElementType()
{
  return array_element_type_;
}

bool Parameter::rangeIsSet()
{
  return range_is_set_;
}

constants::NumberType Parameter::getMin()
{
  return min_;
}

constants::NumberType Parameter::getMax()
{
  return max_;
}

unsigned char Parameter::getArrayLengthMin()
{
  return array_length_min_;
}

unsigned char Parameter::getArrayLengthMax()
{
  return array_length_max_;
}

bool Parameter::arrayLengthRangeIsSet()
{
  return array_length_range_is_set_;
}

}
