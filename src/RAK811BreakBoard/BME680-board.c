#include "board.h"
#include "BME680.h"
#include "BME680-board.h"

struct bme680_dev gas_sensor;

static int8_t bme680_i2c_read(uint8_t dev_id, uint8_t reg_addr, uint8_t *data, uint16_t len){
    uint8_t res = 0;
    res = I2cReadBuffer(&I2c, dev_id, reg_addr, data, len);
    if( res != SUCCESS  ){
        printf("BME I2C READ ERROR\r\n");
        return BME680_E_COM_FAIL;
    }
    return BME680_OK;
}

static int8_t bme680_i2c_write(uint8_t dev_id, uint8_t reg_addr, uint8_t *data, uint16_t len){
    uint8_t res = 0;

    res = I2cWriteBuffer(&I2c, dev_id, reg_addr,  data, len);
    if( res != SUCCESS  ){
        printf("BME I2C WRITE ERROR\r\n");
        return BME680_E_COM_FAIL;
    }
    return BME680_OK;
}

int8_t BME680_configuation( void ){
    uint8_t rslt = 0;
    uint8_t set_required_settings;
    
    /* Set the temperature, pressure and humidity settings */
    gas_sensor.tph_sett.os_hum = BME680_OS_2X;
    gas_sensor.tph_sett.os_pres = BME680_OS_4X;
    gas_sensor.tph_sett.os_temp = BME680_OS_8X;
    gas_sensor.tph_sett.filter = BME680_FILTER_SIZE_3;

    /* Set the remaining gas sensor settings and link the heating profile */
    gas_sensor.gas_sett.run_gas = BME680_ENABLE_GAS_MEAS;
    /* Create a ramp heat waveform in 3 steps */
    gas_sensor.gas_sett.heatr_temp = 320; /* degree Celsius */
    gas_sensor.gas_sett.heatr_dur = 150; /* milliseconds */

    /* Select the power mode */
    /* Must be set before writing the sensor configuration */
    gas_sensor.power_mode = BME680_FORCED_MODE; 

    /* Set the required sensor settings needed */
    set_required_settings = BME680_OST_SEL | BME680_OSP_SEL | BME680_OSH_SEL | BME680_FILTER_SEL 
        | BME680_GAS_SENSOR_SEL;

    /* Set the desired sensor configuration */
    rslt = bme680_set_sensor_settings(set_required_settings,&gas_sensor);
    if( rslt != BME680_OK ){
        printf("BME680 set sensor settings ERROR\r\n");
    }
    
    return rslt;

}


 int8_t BME680_Init( void ){
    int8_t rslt = BME680_OK;
    int16_t t;
    uint32_t p;
    uint32_t h;
    uint32_t r;
    
    gas_sensor.dev_id = BME680_I2C_ADDR_PRIMARY;
    gas_sensor.intf = BME680_I2C_INTF;
    gas_sensor.read = bme680_i2c_read;
    gas_sensor.write = bme680_i2c_write;

    gas_sensor.amb_temp = 25;
    gas_sensor.delay_ms = DelayMs;
    
    rslt = bme680_init(&gas_sensor);
    if( rslt != BME680_OK ){
        printf("BME680 Init ERROR\r\n");
        return rslt;
    }
    
    printf("BME680 init success!\r\n");

    BME680_read(&t, &p, &h, &r);
    
    return rslt;
}



int8_t BME680_read( int16_t * temprature, uint32_t * pressure, uint32_t * humidity, uint32_t * resistance){
    uint16_t meas_period;
    uint8_t rslt;
    uint8_t set_required_settings;
    struct bme680_field_data data = {0};

    /* Set the temperature, pressure and humidity settings */
    gas_sensor.tph_sett.os_hum = BME680_OS_2X;
    gas_sensor.tph_sett.os_pres = BME680_OS_4X;
    gas_sensor.tph_sett.os_temp = BME680_OS_8X;
    gas_sensor.tph_sett.filter = BME680_FILTER_SIZE_3;

    /* Set the remaining gas sensor settings and link the heating profile */
    gas_sensor.gas_sett.run_gas = BME680_ENABLE_GAS_MEAS;
    /* Create a ramp heat waveform in 3 steps */
    gas_sensor.gas_sett.heatr_temp = 300; /* degree Celsius */
    gas_sensor.gas_sett.heatr_dur = 250; /* milliseconds */

    /* Select the power mode */
    /* Must be set before writing the sensor configuration */
    gas_sensor.power_mode = BME680_FORCED_MODE; 

    /* Set the required sensor settings needed */
    set_required_settings = BME680_OST_SEL | BME680_OSP_SEL | BME680_OSH_SEL | BME680_FILTER_SEL 
        | BME680_GAS_SENSOR_SEL;

    /* Set the desired sensor configuration */
    rslt = bme680_set_sensor_settings(set_required_settings,&gas_sensor);
    if( rslt != BME680_OK ){
        printf("BME680 set sensor settings ERROR\r\n");
    }
    
    
    rslt = bme680_set_sensor_mode(&gas_sensor);

    if( rslt != BME680_OK ){
        printf("BME680 set sensor mode ERROR\r\n");
    }
    
    bme680_get_profile_dur(&meas_period, &gas_sensor);

    DelayMs(meas_period); /* Delay till the measurement is ready */

    rslt = bme680_get_sensor_data(&data, &gas_sensor);
    if( rslt != BME680_OK ){
        printf("BME680 get sensor data ERROR\r\n");
    }

    *temprature = data.temperature;
    *pressure = data.pressure;
    *humidity = data.humidity;
    
    //printf("T: %.2f degC, P: %.2f hPa, H %.2f %%rH ", data.temperature / 100.0f,
    //    data.pressure / 100.0f, data.humidity / 1000.0f );
    printf("T: %d degC, P: %d hPa, H %d rH ", *temprature / 100,
        *pressure / 100, *humidity / 1000 );
    /* Avoid using measurements from an unstable heating setup */
    if(data.status & BME680_GASM_VALID_MSK){
        //e_printf(", G: %d ohms", data.gas_resistance);
        *resistance = data.gas_resistance;
        printf(", G: %d ohms", *resistance);
    }
    
    printf("\r\n");
    
    gas_sensor.power_mode = BME680_SLEEP_MODE;
    bme680_set_sensor_mode(&gas_sensor);
    
    return SUCCESS;
}
