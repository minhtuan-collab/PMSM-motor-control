#ifndef AS5600_H
#define AS5600_H

#ifdef __cplusplus
extern "C"
{
#endif // defined (__cplusplus)

#include "main.h"
#include "stdint.h"
#include "stddef.h"

/*!
 * @brief AS5600 Register map
 *
 * | Addr | Access| Acronym       |
 * |------|-------|---------------|
 * | 0x00 | R     | ZMCO          |
 * | 0x01 | R/W/P | ZPOS (HI)     |
 * | 0x02 | R/W/P | ZPOS (LO)     |
 * | 0x03 | R/W/P | MPOS (HI)     |
 * | 0x04 | R/W/P | MPOS (LO)     |
 * | 0x05 | R/W/P | MANG (HI)     |
 * | 0x06 | R/W/P | MANG (LO)     |
 * | 0x07 | R/W/P | CONF (HI)     |
 * | 0x08 | R/W/P | CONF (LO)     |
 * | -    | -     | -             |
 * | 0x0B | R     | STATUS        |
 * | 0x0C | R     | RAWANGLE (HI) |
 * | 0x0D | R     | RAWANGLE (LO) |
 * | 0x0E | R     | ANGLE (HI)    |
 * | 0x0F | R     | ANGLE (LO)    |
 * | -    | -     | -             |
 * | 0x1A | R     | AGC           |
 * | 0x1B | R     | MAGNITUDE (HI)|
 * | 0x1C | R     | MAGNITUDE (LO)|
 * | -    | -     | -             |
 * | 0xFF | W     | BURN          |
 */

#define AS5600_ZMCO 0x00
#define AS5600_ZPOS_H 0x01
#define AS5600_ZPOS_L 0x02
#define AS5600_MPOS_H 0x03
#define AS5600_MPOS_L 0x04
#define AS5600_MANG_H 0x05
#define AS5600_MANG_L 0x06
#define AS5600_CONF_H 0x07
#define AS5600_CONF_L 0x08
#define AS5600_STATUS 0x0B
#define AS5600_RAWANGLE_H 0x0C
#define AS5600_RAWANGLE_L 0X0D
#define AS5600_ANGLE_H 0X0E
#define AS5600_ANGLE_L 0X0F
#define AS5600_AGC = 0x1A
#define AS5600_MAGNITUDE_H 0x1B
#define AS5600_MAGNITUDE_L 0x1C
#define AS5600_I2CADDR 0x20
#define AS5600_I2CUPDT 0x21
#define AS5600_BURN 0xFF

/*! 
* @brief CONF Register
* - Power Mode
* - Hysteresis
* - Output Stage
* - PWM Frequency
* - Slow Filter
* - Fast Filter Threshold
* - Watchdog
*/

/* Power Mode */
#define AS5600_POWER_MODE_NOM 0
#define AS5600_POWER_MODE_LPM 0x01
#define AS5600_POWER_MODE_LPM2 0x02
#define AS5600_POWER_MODE_LPM3 0x03
#define AS5600_POWER_MODE_COUNT 0x04


/* Hysteresis */

// No hysteresis
#define AS5600_HYSTERESIS_OFF 0
// Least significant bit hysteresis
#define AS5600_HYSTERESIS_1LSB 0x01
// Two least significant bits hysteresis
#define AS5600_HYSTERESIS_2LSB 0x02
// Three least significant bits hysteresis
#define AS5600_HYSTERESIS_3LSB 0x03
// Fence member
#define AS5600_HYSTERESIS_COUNT 0x04


/* Output Stage */

// Output stage analog full range 0 - 100%
#define AS5600_OUTPUT_STAGE_ANALOG_FR 0
// Output stage analog digital range 10 - 90%
#define AS5600_OUTPUT_STAGE_ANALOG_RR 0x01
// Output stage digital PWM
#define AS5600_OUTPUT_STAGE_DIGITAL_PWM 0x02
// Fence member
#define AS5600_OUTPUT_STAGE_COUNT 0x03


/* PWM Output Frequency */

// PWM Frequency of 115 Hz
#define AS5600_PWM_FREQUENCY_115HZ 0
// PWM Frequency of 230 Hz
#define AS5600_PWM_FREQUENCY_230HZ 0x01
// PWM Frequency of 460 Hz
#define AS5600_PWM_FREQUENCY_460HZ 0x02
// PWM Frequency of 920 Hz
#define AS5600_PWM_FREQUENCY_920HZ 0x03
// Fence member
#define AS5600_PWM_FREQUENCY_COUNT 0x04


/* Slow Filter step response delays */

// Slow filter with 16x step response delay
#define S5600_SLOW_FILTER_16X 0
// Slow filter with 8x step response delay
#define AS5600_SLOW_FILTER_8X 0x01
// Slow filter with 4x step response delay
#define AS5600_SLOW_FILTER_4X 0x02
// Slow filter with 2x step response delay
#define AS5600_SLOW_FILTER_2X 0x03
// Fence member
#define AS5600_SLOW_FILTER_COUNT 0x04


/* Fast filter threshold options */

// Use slow filter only
#define AS5600_FF_THRESHOLD_SLOW_FILTER_ONLY 0
// Fast filter threshold of 6 LSB
#define AS5600_FF_THRESHOLD_6LSB 0x01
// Fast filter threshold of 7 LSB
#define AS5600_FF_THRESHOLD_7LSB 0x02
// Fast filter threshold of 9 LSB
#define AS5600_FF_THRESHOLD_9LSB 0x03
// Fast filter threshold of 18 LSB
#define AS5600_FF_THRESHOLD_18LSB 0x04
// Fast filter threshold of 21 LSB
#define AS5600_FF_THRESHOLD_21LSB 0x05
// Fast filter threshold of 24 LSB
#define AS5600_FF_THRESHOLD_24LSB 0x06
// Fast filter threshold of 10 LSB
#define AS5600_FF_THRESHOLD_10LSB 0x07
// Fence member
#define AS5600_FF_THRESHOLD_COUNT 0x08



/* Status register (STATUS) elements */

// No magnet was detected
#define AS5600_STATUS_NO_MANGET 0x0U
// AGC minimum gain overflow, magnet too strong
#define AS5600_STATUS_MH 0x08U
// AGC maximum gain overflow, magnet too weak
#define AS5600_STATUS_ML 0x10U
// Magnet was detected
#define AS5600_STATUS_MD 0x20U
// Magnet detected with AGC minimum gain overflow, magnet too strong
#define AS5600_STATUS_MH_MD 0x28U
// Magnet detected with AGC maximum gain overflow, magnet too weak
#define AS5600_STATUS_ML_MD 0x30U


/* BURN register commands */

// Command for burning a setting configuration
#define AS5600_BURN_MODE_BURN_SETTING 0x40U
// Command for burning start and end angles
#define AS5600_BURN_MODE_BURN_ANGLE 0x80U
// Fence member
#define AS5600_BURN_MODE_COUNT 0

typedef struct {
    uint16_t angle12;  // 12-bit Angle  (0-4095)
    uint16_t offset_pos;
    uint16_t correct_angle;
    uint16_t zpos12;
    uint16_t mpos12;
    uint8_t cmd;
    uint8_t filter_mode;
    uint16_t No_of_turns;
	uint16_t total_angle;
	uint16_t quadrant_Number;
	uint16_t previous_quadrant_Number;
} EncoderStruct;

extern EncoderStruct *encoder;

/* --------------------------- Programming Function --------------------------- */

/* Check i2c Connection */
uint8_t AS5600_CheckConnection();

/* Angle programming through I2C interface */
void AS5600_ReadRawAngle(EncoderStruct *encoder);
void AS5600_WriteZPOS(EncoderStruct *encoder);
void AS5600_ReadZPOS(EncoderStruct *encoder);
void AS5600_WriteMPOS(EncoderStruct *encoder);
void AS5600_ReadMPOS(EncoderStruct *encoder);
void AS5600_BurnAngle(EncoderStruct *encoder);
void AS5600_VerifyBurnAngle();

/* Smooth motion (Set Slow Filter) */
void AS5600_MakeSmooth(EncoderStruct *encoder);

/* Calibration */
void AS5600_CalibrateOffsetAngle(EncoderStruct *encoder);
void AS5600_CorrectedAngleRaw(EncoderStruct *encoder);

/* Check Quadrant and Calculate Revolution */
void AS5600_CheckQuadrant(EncoderStruct *encoder);

#ifdef __cplusplus
}
#endif // defined (__cplusplus)

#endif //AS5600_H
