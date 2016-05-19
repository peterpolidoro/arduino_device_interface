// ----------------------------------------------------------------------------
// Constants.h
//
//
// Authors:
// Peter Polidoro polidorop@janelia.hhmi.org
// ----------------------------------------------------------------------------
#ifndef CONSTANTS_H
#define CONSTANTS_H
#include "ConstantVariable.h"

namespace constants
{
//MAX values must be >= 1, >= created/copied count, < RAM limit
enum{FIELD_COUNT_MAX=1};
enum{PARAMETER_COUNT_MAX=5};
enum{METHOD_COUNT_MAX=8};

extern const size_t baudrate;

extern const size_t model_number;

extern const long firmware_major;
extern const long firmware_minor;
extern const long firmware_patch;

extern HardwareSerial& serial2;
extern const size_t serial2_rx_pin;

extern ConstantString device_name;

// Fields
// Field values must be long, bool, long[], or bool[]
extern ConstantString starting_chars_count_field_name;
extern const long starting_chars_count_min;
extern const long starting_chars_count_max;
extern const long starting_chars_count_default;

// Parameters
extern ConstantString string_parameter_name;

extern ConstantString string2_parameter_name;

extern ConstantString count_parameter_name;
extern const int count_min;
extern const int count_max;

extern ConstantString index_array_parameter_name;
extern const int index_array_element_min;
extern const int index_array_element_max;
extern const size_t index_array_length_min;
extern const size_t index_array_length_max;

extern ConstantString double_echo_parameter_name;

// Methods
extern ConstantString echo_method_name;
extern ConstantString length_method_name;
extern ConstantString starts_with_method_name;
extern ConstantString repeat_method_name;
extern ConstantString chars_at_method_name;
extern ConstantString starting_chars_method_name;
extern ConstantString set_stored_string_method_name;
extern ConstantString get_stored_string_method_name;

// Errors
extern ConstantString index_error;

}
#endif
