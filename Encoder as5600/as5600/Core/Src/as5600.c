#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <math.h>
#include "as5600.h"

#define AS5600_ADDR_7BIT   0x40
#define AS5600_I2C_ADDR    (AS5600_ADDR_7BIT << 1) // HAL expects 8-bit addr
#define AS5600_RAW_ANGLE_MASK   0x0FFFU  /* 12-bit */
#define PI 3.1415926535

const float   AS5600_RAW_TO_DEGREES     = 360.0 / 4096;
const float   AS5600_DEGREES_TO_RAW     = 4096 / 360.0;
const float   AS5600_RAW_TO_RADIANS     = PI * 2.0 / 4096;
const float   AS5600_RAW_TO_RPM         = 60.0 / 4096;

static void AS5600_ReadRawAngle(I2C_HandleTypeDef *hi2c1, uint16_t *angle12)
{

    uint8_t buf[2];


    // Read 2 bytes from register RAW_ANGLE_MSB (0x0C) & RAW_ANGLE_LSB (0x0D)
    HAL_I2C_Mem_Read(
		hi2c1,
		AS5600_I2C_ADDR,   // 8-bit address (0x40 << 1)
		AS5600_RAWANGLE_H, // Starting address at 0x0C
		I2C_MEMADD_SIZE_8BIT,
		buf,
		2,
		100                        // timeout 100ms
    );

    // Combines 12 bits (bits [11:0])
    uint16_t raw = ((uint16_t)buf[0] << 8) | buf[1];
    *angle12 = raw;
}

static void AS5600_WriteZPOS(I2C_HandleTypeDef *hi2c1, uint16_t zpos12)
{
	// Limit to 12-bit values ​​(guaranteed not to exceed 4095)
    zpos12 &= 0x0FFF;

    // ZPOS consists of 2 bytes:
    // - MSB (0x01): contains 4 high bits (bits 11:8)
    // - LSB (0x02): contains 8 low bits (bits 7:0)
    uint8_t zp_data_msb = (uint8_t)((zpos12 >> 8) & 0x0F);
    uint8_t zp_data_lsb = (uint8_t)(zpos12 & 0xFF);

    // Write MSB first
    HAL_I2C_Mem_Write(
        hi2c1,
        AS5600_I2C_ADDR,  // 8-bit address (0x40 << 1)
        AS5600_ZPOS_H,
        I2C_MEMADD_SIZE_8BIT,
        &zp_data_msb,
        1,
        100
    );

    // Write LSB later
    HAL_I2C_Mem_Write(
        hi2c1,
        AS5600_I2C_ADDR,
        AS5600_ZPOS_L,
        I2C_MEMADD_SIZE_8BIT,
        &zp_data_lsb,
        1,
        100
    );
}

static void AS5600_ReadZPOS(I2C_HandleTypeDef *hi2c1, uint16_t *zpos12)
{
    uint8_t buf1[2];

    /* Read 2 consecutive bytes starting from ZPOS_MSB (0x01) -> will read 0x01 and 0x02 */
    HAL_I2C_Mem_Read(
        hi2c1,
        AS5600_I2C_ADDR,
        AS5600_ZPOS_H,    // Starting address at 0x01
        I2C_MEMADD_SIZE_8BIT,
        buf1,
        2,
        100
    );

    /* 12-bit combination */
    uint16_t zp_raw = ((uint16_t)buf1[0] << 8) | buf1[1];
    *zpos12 = zp_raw;
}

static void AS5600_WriteMPOS(I2C_HandleTypeDef *hi2c1, uint16_t mpos12)
{
	// Limit to 12-bit values ​​(guaranteed not to exceed 4095)
    mpos12 &= 0x0FFF;

    // MPOS consists of 2 bytes:
    // - MSB (0x03): contains 4 high bits (bits 11:8)
    // - LSB (0x04): contains 8 low bits (bits 7:0)
    uint8_t mp_data_msb = (uint8_t)((mpos12 >> 8) & 0x0F);
    uint8_t mp_data_lsb = (uint8_t)(mpos12 & 0xFF);

    // Write MSB first
    HAL_I2C_Mem_Write(
        hi2c1,
        AS5600_I2C_ADDR,
        AS5600_MPOS_H,
        I2C_MEMADD_SIZE_8BIT,
        &mp_data_msb,
        1,
        100
    );

    // Write LSB later
    HAL_I2C_Mem_Write(
        hi2c1,
        AS5600_I2C_ADDR,
        AS5600_MPOS_L,
        I2C_MEMADD_SIZE_8BIT,
        &mp_data_lsb,
        1,
        100
    );
}

static void AS5600_ReadMPOS(I2C_HandleTypeDef *hi2c1, uint16_t *mpos12)
{
    uint8_t buf2[2];

    /* Read 2 consecutive bytes starting from MPOS_MSB (0x03) -> will read 0x03 and 0x04 */
    HAL_I2C_Mem_Read(
        hi2c1,
        AS5600_I2C_ADDR,
        AS5600_MPOS_H,
        I2C_MEMADD_SIZE_8BIT,
        buf2,
        2,
        100
    );

    /* 12-bit combination */
    uint16_t mp_raw = ((uint16_t)buf2[0] << 8) | buf2[1];
    *mpos12 = mp_raw;
}

static void AS5600_BurnAngle(I2C_HandleTypeDef *hi2c1)
{

	/* Write command BURN_ANGLE = 0x80 to register 0xFF */
    uint8_t cmd = 0x80;
    HAL_I2C_Mem_Write(
        hi2c1,
        AS5600_I2C_ADDR,
        AS5600_BURN,      // burn register OTP = 0xFF
        I2C_MEMADD_SIZE_8BIT,
        &cmd,
        1,
        500                   // timeout 500ms
    );
}

static void AS5600_VerifyBurnAngle(I2C_HandleTypeDef *hi2c1)
{
    uint8_t seq[3] = {0x01, 0x11, 0x10};

    /* Write 3 values ​​sequentially to load OTP data into the register */
    for (int i = 0; i < 3; ++i) {
        HAL_I2C_Mem_Write(
            hi2c1,
            AS5600_I2C_ADDR,
            AS5600_BURN,
            I2C_MEMADD_SIZE_8BIT,
            &seq[i],
            1,
            100
        );
    }
}

static void AS5600_MakeSmooth(I2C_HandleTypeDef *hi2c1)
{
	uint8_t filter_mode = 0;
	filter_mode = (AS5600_SLOW_FILTER_8X | (AS5600_FF_THRESHOLD_SLOW_FILTER_ONLY << 2));
	HAL_I2C_Mem_Write(
			hi2c1,
	        AS5600_I2C_ADDR,
	        AS5600_CONF_H,      // burn register OTP = 0xFF
	        I2C_MEMADD_SIZE_8BIT,
	        &filter_mode,
	        1,
	        500                   // timeout 500ms
	    );
}


static void CalibrateOffsetAngle(I2C_HandleTypeDef *hi2c1, uint16_t *offset_pos)
{
	 uint32_t sum = 0;
	 uint16_t start_position;
	 const uint16_t samples = 1000;

	 for (uint16_t i = 0; i < samples; i++)
	     {
	         AS5600_ReadRawAngle(hi2c, &start_position);

	         sum += start_position;
	         HAL_Delay(1);
	     }
	 *offset = (uint16_t)(sum / samples);
}

static void AS5600_CorrectedAngleRaw(I2C_HandleTypeDef *hi2c,
                                            uint16_t offset_pos,
                                            uint16_t *correct_angle)
{
	uint16_t raw_angle;
	AS5600_ReadRawAngle(hi2c, &raw_angle);
	uint16_t corrected;
	corrected = raw_angle - offset_pos;
	*correct_angle = corrected;
}


/* 	uint16_t start_angle = 0;
;
	float deg_angle = raw_angle * AS5600_RAW_TO_DEGREES;



	if (*correct_angle < 0){
		*correct_angle = *correct_angle + 360;
	} */






