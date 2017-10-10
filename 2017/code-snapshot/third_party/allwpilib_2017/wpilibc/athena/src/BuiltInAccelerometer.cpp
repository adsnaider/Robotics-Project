/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "BuiltInAccelerometer.h"

#include "HAL/Accelerometer.h"
#include "HAL/HAL.h"
#include "WPIErrors.h"

using namespace frc;

/**
 * Constructor.
 *
 * @param range The range the accelerometer will measure
 */
BuiltInAccelerometer::BuiltInAccelerometer(Range range) {
  SetRange(range);

  HAL_Report(HALUsageReporting::kResourceType_Accelerometer, 0, 0,
             "Built-in accelerometer");
}

void BuiltInAccelerometer::SetRange(Range range) {
  HAL_SetAccelerometerActive(false);
  HAL_SetAccelerometerRange((HAL_AccelerometerRange)range);
  HAL_SetAccelerometerActive(true);
}

/**
 * @return The acceleration of the roboRIO along the X axis in g-forces
 */
double BuiltInAccelerometer::GetX() { return HAL_GetAccelerometerX(); }

/**
 * @return The acceleration of the roboRIO along the Y axis in g-forces
 */
double BuiltInAccelerometer::GetY() { return HAL_GetAccelerometerY(); }

/**
 * @return The acceleration of the roboRIO along the Z axis in g-forces
 */
double BuiltInAccelerometer::GetZ() { return HAL_GetAccelerometerZ(); }
