#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <math.h>
#include "as5600.h"
#include "i2c.h"

#define AS5600_ADDR_7BIT   0x36
#define AS5600_I2C_ADDR    (AS5600_ADDR_7BIT << 1) // HAL expects 8-bit address
#define AS5600_RAW_ANGLE_MASK   0x0FFFU  /* 12-bit */
#define PI 3.1415926535

const float   AS5600_RAW_TO_DEGREES     = 360.0 / 4096;
const float   AS5600_DEGREES_TO_RAW     = 4096 / 360.0;
const float   AS5600_RAW_TO_RADIANS     = PI * 2.0 / 4096;
const float   AS5600_RAW_TO_RPM         = 60.0 / 4096;

EncoderStruct *encoder = 0;

uint8_t AS5600_CheckConnection(void)
{
	HAL_StatusTypeDef ref;
	ref = HAL_I2C_IsDeviceReady(&hi2c1, AS5600_I2C_ADDR, 3, 100);

    if (ref == HAL_OK)
    {
        // ACK feedback device -> I2C works fine
        return 1;
    }
    else
    {
        // No response received -> I2C error or not connected
        return 0;
    }
}

void AS5600_ReadRawAngle(EncoderStruct *encoder)
{
	encoder -> angle12 = 0;
    uint8_t buf[2];

    // Read 2 bytes from register RAW_ANGLE_MSB (0x0C) & RAW_ANGLE_LSB (0x0D)
    HAL_I2C_Mem_Read(
		&hi2c1,
		AS5600_I2C_ADDR,   // 8-bit address (0x40 << 1)
		AS5600_RAWANGLE_H, // Starting address at 0x0C
		I2C_MEMADD_SIZE_8BIT,
		buf,
		2,
		500                        // timeout 100ms
);

    // Combines 12 bits (bits [11:0])
    uint16_t raw = (((uint16_t)buf[0] << 8) | buf[1]);
    encoder -> angle12 = raw;
}

void AS5600_WriteZPOS(EncoderStruct *encoder)
{
	// Limit to 12-bit values ​​(guaranteed not to exceed 4095)
    encoder -> angle12 &= 0x0FFF;

    // ZPOS consists of 2 bytes:
    // - MSB (0x01): contains 4 high bits (bits 11:8)
    // - LSB (0x02): contains 8 low bits (bits 7:0)
    uint8_t zp_data_msb = (uint8_t)((encoder -> angle12 >> 8) & 0x0F);
    uint8_t zp_data_lsb = (uint8_t)(encoder -> angle12 & 0xFF);

    // Write MSB first
    HAL_I2C_Mem_Write(
        &hi2c1,
        AS5600_I2C_ADDR,  // 8-bit address (0x40 << 1)
        AS5600_ZPOS_H,
        I2C_MEMADD_SIZE_8BIT,
        &zp_data_msb,
        1,
        100
    );

    // Write LSB later
    HAL_I2C_Mem_Write(
        &hi2c1,
        AS5600_I2C_ADDR,
        AS5600_ZPOS_L,
        I2C_MEMADD_SIZE_8BIT,
        &zp_data_lsb,
        1,
        100
    );
}

void AS5600_ReadZPOS(EncoderStruct *encoder)
{
	encoder -> zpos12 = 0;
    uint8_t buf1[2];

    /* Read 2 consecutive bytes starting from ZPOS_MSB (0x01) -> will read 0x01 and 0x02 */
    HAL_I2C_Mem_Read(
        &hi2c1,
        AS5600_I2C_ADDR,
        AS5600_ZPOS_H,    // Starting address at 0x01
        I2C_MEMADD_SIZE_8BIT,
        buf1,
        2,
        100
    );

    /* 12-bit combination */
    uint16_t zp_raw = ((uint16_t)buf1[0] << 8) | buf1[1];
    encoder -> zpos12 = zp_raw;
}

void AS5600_WriteMPOS(EncoderStruct *encoder)
{
	// Limit to 12-bit values ​​(guaranteed not to exceed 4095)
    encoder -> angle12 &= 0x0FFF;

    // MPOS consists of 2 bytes:
    // - MSB (0x03): contains 4 high bits (bits 11:8)
    // - LSB (0x04): contains 8 low bits (bits 7:0)
    uint8_t mp_data_msb = (uint8_t)((encoder -> angle12 >> 8) & 0x0F);
    uint8_t mp_data_lsb = (uint8_t)(encoder -> angle12 & 0xFF);

    // Write MSB first
    HAL_I2C_Mem_Write(
        &hi2c1,
        AS5600_I2C_ADDR,
        AS5600_MPOS_H,
        I2C_MEMADD_SIZE_8BIT,
        &mp_data_msb,
        1,
        100
    );

    // Write LSB later
    HAL_I2C_Mem_Write(
        &hi2c1,
        AS5600_I2C_ADDR,
        AS5600_MPOS_L,
        I2C_MEMADD_SIZE_8BIT,
        &mp_data_lsb,
        1,
        100
    );
}

void AS5600_ReadMPOS(EncoderStruct *encoder)
{
	encoder -> mpos12 = 0;
    uint8_t buf2[2];

    /* Read 2 consecutive bytes starting from MPOS_MSB (0x03) -> will read 0x03 and 0x04 */
    HAL_I2C_Mem_Read(
        &hi2c1,
        AS5600_I2C_ADDR,
        AS5600_MPOS_H,
        I2C_MEMADD_SIZE_8BIT,
        buf2,
        2,
        100
    );

    /* 12-bit combination */
    uint16_t mp_raw = ((uint16_t)buf2[0] << 8) | buf2[1];
    encoder -> mpos12 = mp_raw;
}

void AS5600_BurnAngle(EncoderStruct *encoder)
{

	/* Write command BURN_ANGLE = 0x80 to register 0xFF */
    encoder -> cmd = 0x80;
    HAL_I2C_Mem_Write(
        &hi2c1,
        AS5600_I2C_ADDR,
        AS5600_BURN,      // burn register OTP = 0xFF
        I2C_MEMADD_SIZE_8BIT,
        &(encoder -> cmd),
        1,
        500                   // timeout 500ms
    );
}

void AS5600_VerifyBurnAngle()
{
    uint8_t seq[3] = {0x01, 0x11, 0x10};

    /* Write 3 values ​​sequentially to load OTP data into the register */
    for (int i = 0; i < 3; ++i) {
        HAL_I2C_Mem_Write(
            &hi2c1,
            AS5600_I2C_ADDR,
            AS5600_BURN,
            I2C_MEMADD_SIZE_8BIT,
            &seq[i],
            1,
            100
        );
    }
}

void AS5600_MakeSmooth(EncoderStruct *encoder)
{
	encoder -> filter_mode  = 0;
	encoder -> filter_mode = (AS5600_SLOW_FILTER_8X | (AS5600_FF_THRESHOLD_SLOW_FILTER_ONLY << 2));
	HAL_I2C_Mem_Write(
			&hi2c1,
	        AS5600_I2C_ADDR,
	        AS5600_CONF_H,      // burn register OTP = 0xFF
	        I2C_MEMADD_SIZE_8BIT,
	        &(encoder -> filter_mode),
	        1,
	        500                   // timeout 500ms
	    );
}


void CalibrateOffsetAngle(EncoderStruct *encoder)
{
	 uint32_t sum = 0;
	 uint16_t start_position;
	 const uint16_t samples = 1000;

	 for (uint16_t i = 0; i < samples; i++)
	     {
	         AS5600_ReadRawAngle(encoder);
	         start_position = encoder -> angle12;

	         sum += start_position;
	         HAL_Delay(1);
	     }
	 encoder -> offset_pos = (uint16_t)(sum / samples);
}


void AS5600_CorrectedAngleRaw(EncoderStruct *encoder)
{
	uint16_t corrected;
	AS5600_ReadRawAngle(encoder);
	corrected = encoder -> angle12 - encoder -> offset_pos;
	encoder -> correct_angle = corrected;
}

void AS5600_CheckQuadrant(EncoderStruct *encoder)
{
	uint16_t deg_angle = encoder -> angle12 * AS5600_RAW_TO_DEGREES;

	 /*
	    //Quadrants:
	    4  |  1
	    ---|---
	    3  |  2
	  */

	  //Quadrant 1
	  if (deg_angle >= 0 && deg_angle <= 90)
	  {
	    encoder -> quadrant_Number = 1;
	  }

	  //Quadrant 2
	  if (deg_angle > 90 && deg_angle <= 180)
	  {
		 encoder -> quadrant_Number = 2;
	  }

	  //Quadrant 3
	  if (deg_angle > 180 && deg_angle <= 270)
	  {
	    encoder -> quadrant_Number = 3;
	  }

	  //Quadrant 4
	  if (deg_angle > 270 && deg_angle < 360)
	  {
	    encoder -> quadrant_Number = 4;
	  }


	  if (encoder -> quadrant_Number != encoder -> previous_quadrant_Number) //if we changed quadrant
	  {
	    if (encoder -> quadrant_Number == 1 && encoder -> previous_quadrant_Number == 4)
	    {
	    	(encoder -> No_of_turns)++; // 4 --> 1 transition: CW rotation
	    }

	    if (encoder -> quadrant_Number == 4 && encoder -> previous_quadrant_Number == 1)
	    {
	    	(encoder -> No_of_turns)--; // 1 --> 4 transition: CCW rotation
	    }

	    encoder -> previous_quadrant_Number = encoder -> quadrant_Number;  //update to the current quadrant

	  }

	  encoder -> total_angle = ((encoder -> No_of_turns) * 360) + deg_angle; //number of turns (+/-) plus the actual angle within the 0-360 range
}












