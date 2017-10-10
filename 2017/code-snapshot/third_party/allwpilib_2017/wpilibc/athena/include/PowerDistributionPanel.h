/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>
#include <string>

#include "SensorBase.h"

namespace frc {

/**
 * Class for getting voltage, current, temperature, power and energy from the
 * CAN PDP.
 */
class PowerDistributionPanel : public SensorBase {
 public:
  PowerDistributionPanel();
  explicit PowerDistributionPanel(int module);

  double GetVoltage() const;
  double GetTemperature() const;
  double GetCurrent(int channel) const;
  double GetTotalCurrent() const;
  double GetTotalPower() const;
  double GetTotalEnergy() const;
  void ResetTotalEnergy();
  void ClearStickyFaults();

 private:
  int m_module;
};

}  // namespace frc
