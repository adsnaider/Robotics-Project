/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>
#include <string>

#include "HAL/Types.h"
#include "SensorBase.h"

namespace frc {

/**
 * PCM compressor
 */
class Compressor : public SensorBase {
 public:
  // Default PCM ID is 0
  explicit Compressor(int pcmID = GetDefaultSolenoidModule());
  virtual ~Compressor() = default;

  void Start();
  void Stop();
  bool Enabled() const;

  bool GetPressureSwitchValue() const;

  double GetCompressorCurrent() const;

  void SetClosedLoopControl(bool on);
  bool GetClosedLoopControl() const;

  bool GetCompressorCurrentTooHighFault() const;
  bool GetCompressorCurrentTooHighStickyFault() const;
  bool GetCompressorShortedStickyFault() const;
  bool GetCompressorShortedFault() const;
  bool GetCompressorNotConnectedStickyFault() const;
  bool GetCompressorNotConnectedFault() const;
  void ClearAllPCMStickyFaults();

 protected:
  HAL_CompressorHandle m_compressorHandle;

 private:
  void SetCompressor(bool on);
  int m_module;
};

}  // namespace frc
