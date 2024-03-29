/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/


#include "Jaguar.h"
//#include "NetworkCommunication/UsageReporting.h"

/**
 * Common initialization code called by all constructors.
 */
void Jaguar::InitJaguar()
{
	/**
	 * Input profile defined by Luminary Micro.
	 *
	 * Full reverse ranges from 0.671325ms to 0.6972211ms
	 * Proportional reverse ranges from 0.6972211ms to 1.4482078ms
	 * Neutral ranges from 1.4482078ms to 1.5517922ms
	 * Proportional forward ranges from 1.5517922ms to 2.3027789ms
	 * Full forward ranges from 2.3027789ms to 2.328675ms
	 */
	SetBounds(2.31, 1.55, 1.507, 1.454, .697);
	SetPeriodMultiplier(kPeriodMultiplier_1X);
	SetRaw(m_centerPwm);
	SetZeroLatch();

	HALReport(HALUsageReporting::kResourceType_Jaguar, GetChannel());
}

/**
 * Constructor for a Jaguar connected via PWM
 * @param channel The PWM channel that the Jaguar is attached to. 0-9 are on-board, 10-19 are on the MXP port
 */
Jaguar::Jaguar(uint32_t channel) : SafePWM(channel)
{
	InitJaguar();
}

Jaguar::~Jaguar()
{
}

/**
 * Set the PWM value.
 *
 * The PWM value is set using a range of -1.0 to 1.0, appropriately
 * scaling the value for the FPGA.
 *
 * @param speed The speed value between -1.0 and 1.0 to set.
 * @param syncGroup Unused interface.
 */
void Jaguar::Set(float speed, uint8_t syncGroup)
{
	SetSpeed(speed);
}

/**
 * Get the recently set value of the PWM.
 *
 * @return The most recently set value for the PWM between -1.0 and 1.0.
 */
float Jaguar::Get()
{
	return GetSpeed();
}

/**
 * Common interface for disabling a motor.
 */
void Jaguar::Disable()
{
	SetRaw(kPwmDisabled);
}

/**
 * Write out the PID value as seen in the PIDOutput base object.
 *
 * @param output Write out the PWM value as was found in the PIDController
 */
void Jaguar::PIDWrite(float output)
{
	Set(output);
}
