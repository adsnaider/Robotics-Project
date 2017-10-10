/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>
#include <string>

#include "HAL/Types.h"
#include "SolenoidBase.h"

namespace frc {

/**
 * Solenoid class for running high voltage Digital Output (PCM).
 *
 * The Solenoid class is typically used for pneumatics solenoids, but could be
 * used for any device within the current spec of the PCM.
 */
class Solenoid : public SolenoidBase {
 public:
  explicit Solenoid(int channel);
  Solenoid(int moduleNumber, int channel);
  virtual ~Solenoid();
  virtual void Set(bool on);
  virtual bool Get() const;
  bool IsBlackListed() const;

 private:
  HAL_SolenoidHandle m_solenoidHandle = HAL_kInvalidHandle;
  int m_channel;  ///< The channel on the module to control.
};

}  // namespace frc
