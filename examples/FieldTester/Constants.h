// ----------------------------------------------------------------------------
// Constants.h
//
//
// Authors:
// Peter Polidoro polidorop@janelia.hhmi.org
// ----------------------------------------------------------------------------
#ifndef FIELD_TESTER_CONSTANTS_H
#define FIELD_TESTER_CONSTANTS_H
#include "ConstantVariable.h"
#include "ModularServer.h"


namespace constants
{
//MAX values must be >= 1, >= created/copied count, < RAM limit
enum{FIELD_COUNT_MAX=9};
enum{PARAMETER_COUNT_MAX=6};
enum{METHOD_COUNT_MAX=15};
enum{CALLBACK_COUNT_MAX=1};

extern const size_t baudrate;

extern ConstantString device_name;

extern ConstantString firmware_name;
extern const modular_server::FirmwareInfo firmware_info;

extern ConstantString form_factor;
extern ConstantString hardware_name;
extern const modular_server::HardwareInfo hardware_info;

// Units

// Fields
// Field values must be long, double, bool, long[], double[], bool[], char[], ConstantString *
extern ConstantString double_field_name;
extern const double double_default;

extern ConstantString bool_field_name;
extern const bool bool_default;

extern ConstantString long_array_field_name;
enum{LONG_ARRAY_LENGTH=4};
extern const long long_array_default[LONG_ARRAY_LENGTH];
extern const long long_array_element_min;
extern const long long_array_element_max;

extern ConstantString double_array_field_name;
enum{DOUBLE_ARRAY_LENGTH=3};
extern const double double_array_default[DOUBLE_ARRAY_LENGTH];
extern const double double_array_element_min;
extern const double double_array_element_max;

extern ConstantString bool_array_field_name;
enum{BOOL_ARRAY_LENGTH=2};
extern const bool bool_array_default[BOOL_ARRAY_LENGTH];

extern ConstantString string_field_name;
enum{STRING_LENGTH_MAX=10};
extern const char string_default[STRING_LENGTH_MAX];

extern ConstantString odd_field_name;
enum{ODD_SUBSET_LENGTH=5};
extern modular_server::SubsetMemberType odd_subset[ODD_SUBSET_LENGTH];
extern const long & odd_default;

extern ConstantString mode_field_name;
enum{MODE_SUBSET_LENGTH=3};
extern ConstantString mode_rising;
extern ConstantString mode_falling;
extern ConstantString mode_change;
extern modular_server::SubsetMemberType mode_ptr_subset[MODE_SUBSET_LENGTH];
extern const ConstantString * const mode_ptr_default;

extern ConstantString odd_array_field_name;
enum{ODD_ARRAY_LENGTH=2};
extern const long odd_array_default[ODD_ARRAY_LENGTH];

// Parameters
extern ConstantString long_array_parameter_name;

extern ConstantString length_parameter_name;
extern const long length_min;
extern const long length_max;

extern ConstantString count_parameter_name;
enum{COUNT_SUBSET_LENGTH=7};
extern modular_server::SubsetMemberType count_subset[COUNT_SUBSET_LENGTH];

extern ConstantString count_array_parameter_name;
extern const size_t count_array_element_min;
extern const size_t count_array_element_max;
extern const size_t count_array_length_min;
extern const size_t count_array_length_max;

extern ConstantString direction_parameter_name;
enum{DIRECTION_SUBSET_LENGTH=4};
extern ConstantString direction_up;
extern ConstantString direction_down;
extern ConstantString direction_left;
extern ConstantString direction_right;
extern modular_server::SubsetMemberType direction_ptr_subset[DIRECTION_SUBSET_LENGTH];

extern ConstantString direction_array_parameter_name;
extern const size_t direction_array_length_min;
extern const size_t direction_array_length_max;

// Methods
extern ConstantString get_doubled_method_name;
extern ConstantString get_bool_method_name;
extern ConstantString get_long_array_fixed_method_name;
extern ConstantString get_long_array_variable_method_name;
extern ConstantString set_long_array_fixed_method_name;
extern ConstantString set_long_array_variable_method_name;
extern ConstantString set_long_array_parameter_method_name;
extern ConstantString get_string_all_method_name;
extern ConstantString get_string_some_method_name;
extern ConstantString get_count_method_name;
extern ConstantString get_count_array_method_name;
extern ConstantString get_direction_method_name;
extern ConstantString get_direction_array_method_name;
extern ConstantString check_mode_method_name;
extern ConstantString increment_mode_method_name;

// Callbacks

// Errors
extern ConstantString index_error;

}
#endif
