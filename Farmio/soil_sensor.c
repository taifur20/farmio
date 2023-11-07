/*
 * soil_sensor.c
 *
 *  Created on: Nov 5, 2023
 *  Author: Md. Khairul Alam
 */


#include "cyhal.h"
#include "cybsp.h"
#include "soil_sensor.h"
#include "FreeRTOS.h"
#include "task.h"
#include "cy_retarget_io.h"

/*******************************************************************************
* Global Variables
*******************************************************************************/

/* Variable for storing character read from terminal */
uint8_t soil_uart_read_value;


cyhal_uart_t soil_sensor_uart_obj;
uint8_t soil_rx_buf[SOIL_RX_BUF_SIZE];

uint8_t rx_buf[11];
size_t rx_length = 11;

uint8_t values[11];


const cyhal_uart_cfg_t soil_uart_config =
{
    .data_bits = SOIL_DATA_BITS_8,
    .stop_bits = SOIL_STOP_BITS_1,
    .parity = CYHAL_UART_PARITY_NONE,
    .rx_buffer = soil_rx_buf,
    .rx_buffer_size = SOIL_RX_BUF_SIZE
};




void init_soil_sensor(void){

	cy_rslt_t soil_result;

	soil_result = cyhal_uart_init(&soil_sensor_uart_obj, P13_5, P13_4, NC, NC, NULL, &soil_uart_config); //IO6 -> P13_5 -> TX -> R0, IO5 -> P13_4 -> RX -> DI
	//result = cyhal_uart_init(&uart_obj, P0_3, P0_2, NC, NC, NULL, &uart_config); //IO1 - P0_3, IO0 - P0_2
	if (soil_result != CY_RSLT_SUCCESS)
	{
		CY_ASSERT(0);

	}

	/* The UART callback handler registration */
	//cyhal_uart_register_callback(&soil_sensor_uart_obj, uart_event_handler, NULL);
	/* Enable required UART events */
	//cyhal_uart_enable_event(&soil_sensor_uart_obj, (cyhal_uart_event_t)(CYHAL_UART_IRQ_TX_DONE | CYHAL_UART_IRQ_TX_ERROR | CYHAL_UART_IRQ_RX_DONE), INT_PRIORITY, true);


	soil_result = cyhal_uart_set_baud(&soil_sensor_uart_obj, SOIL_BAUD_RATE, NULL);
	if (soil_result != CY_RSLT_SUCCESS)
	{
	    CY_ASSERT(0);

	}

	/* Initialize the DE pin of the RS-485 module */
	soil_result = cyhal_gpio_init(DE, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, false);

	/* GPIO init failed. Stop program execution */
    if (soil_result != CY_RSLT_SUCCESS)
	{
	    CY_ASSERT(0);
	}

	/* Initialize RE pin of the RS-485 module */
    soil_result = cyhal_gpio_init(RE, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, false);

	/* GPIO init failed. Stop program execution */
	if (soil_result != CY_RSLT_SUCCESS)
    {
	    CY_ASSERT(0);
	}
}

//for testing the sensor reading
void read_soil_sensor(void){

	uint8_t query_buf[8] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x07, 0x04, 0x08};
	size_t query_length = 8;
	// switch RS-485 to transmit mode
	cyhal_gpio_write(DE, true);
	cyhal_gpio_write(RE, true);
	//cyhal_system_delay_ms(2);
	vTaskDelay(2);

	// Send the query data to the NPK sensor
	cyhal_uart_write(&soil_sensor_uart_obj, (void*)query_buf, &query_length);

	// switching RS485 to receive mode
	cyhal_gpio_write(DE, false);
	cyhal_gpio_write(RE, false);

	//delay to allow response bytes to be received!
	//cyhal_system_delay_ms(500);
	vTaskDelay(500);

	if (1) {  // Check if there are enough bytes available to read
		cyhal_uart_read(&soil_sensor_uart_obj, (void*)rx_buf, &rx_length);  // Read the received data into the receivedData array

	    // Parse and print the received data in decimal format
	    unsigned int soilHumidity = (rx_buf[3] << 8) | rx_buf[4];
	    unsigned int soilTemperature = (rx_buf[5] << 8) | rx_buf[6];
	    unsigned int soilConductivity = (rx_buf[7] << 8) | rx_buf[8];
	    unsigned int soilPH = (rx_buf[9] << 8) | rx_buf[10];
	    unsigned int nitrogen = (rx_buf[11] << 8) | rx_buf[12];
	    unsigned int phosphorus = (rx_buf[13] << 8) | rx_buf[14];
	    unsigned int potassium = (rx_buf[15] << 8) | rx_buf[16];

	    printf("Soil Humidity: %.2f\n", (float)soilHumidity / 10.0);
	    printf("Soil Temperature: %.2f\n", (float)soilTemperature / 10.0);
	    printf("Soil Conductivity: %.2f\n", (float)soilConductivity);
	    printf("Soil pH: %.2f\n", (float)soilPH / 10.0);
	    printf("Nitrogen: %.2f\n", (float)nitrogen);
	    printf("Phosphorus: %.2f\n", (float)phosphorus);
	    printf("Potassium: %.2f\n", (float)potassium);
	  }

}


float read_ph(void){

	uint8_t query_buf[8] = {0x01, 0x03, 0x00, 0x06, 0x00, 0x01, 0x64, 0x0B};
	size_t query_length = 8;
	// switch RS-485 to transmit mode
	cyhal_gpio_write(DE, true);
	cyhal_gpio_write(RE, true);
	//cyhal_system_delay_ms(2);
	vTaskDelay(2);

	// Send the query data to the NPK sensor
	cyhal_uart_write(&soil_sensor_uart_obj, (void*)query_buf, &query_length);

	// switching RS485 to receive mode
	cyhal_gpio_write(DE, false);
	cyhal_gpio_write(RE, false);

	//delay to allow response bytes to be received!
	//cyhal_system_delay_ms(500);
	vTaskDelay(500);

	uint8_t read_data;
	for (uint8_t i = 0; i < 8; i++) {
	    cyhal_uart_getc(&soil_sensor_uart_obj, &read_data, 0);
	    values[i] = read_data;
	  }
	cyhal_uart_clear(&soil_sensor_uart_obj); //clear if any unread uart buffers

	uint8_t ph = values[3]<<8|values[4];
	float soil_ph = ph/100.0;
	//printf("Soil pH: %.2f\n", ph);
	return soil_ph;

}

float read_temperature(void){

	uint8_t query_buf[8] = { 0x01, 0x03, 0x00, 0x13, 0x00, 0x01, 0x75, 0xCF };
	size_t query_length = 8;

	cyhal_gpio_write(DE, true);
	cyhal_gpio_write(RE, true);

	vTaskDelay(2);

	cyhal_uart_write(&soil_sensor_uart_obj, (void*)query_buf, &query_length);

	cyhal_gpio_write(DE, false);
	cyhal_gpio_write(RE, false);

	vTaskDelay(500);

	uint8_t read_data;
	for (uint8_t i = 0; i < 8; i++) {
	    cyhal_uart_getc(&soil_sensor_uart_obj, &read_data, 0);
	    values[i] = read_data;
	  }
	cyhal_uart_clear(&soil_sensor_uart_obj); //clear if any unread uart buffers

	uint8_t temperature = values[3]<<8|values[4];
    float soil_temp = temperature/10.0;
	//printf("Soil temperature: %.2fC\n", soil_temp);
	return soil_temp;

}

float read_moisture(void){

	uint8_t query_buf[8] = { 0x01, 0x03, 0x00, 0x12, 0x00, 0x01, 0x24, 0x0F };
	size_t query_length = 8;

	cyhal_gpio_write(DE, true);
	cyhal_gpio_write(RE, true);

	vTaskDelay(2);

	cyhal_uart_write(&soil_sensor_uart_obj, (void*)query_buf, &query_length);

	cyhal_gpio_write(DE, false);
	cyhal_gpio_write(RE, false);

	vTaskDelay(500);

	uint8_t read_data;
	for (uint8_t i = 0; i < 8; i++) {
	    cyhal_uart_getc(&soil_sensor_uart_obj, &read_data, 0);
	    values[i] = read_data;
	  }
	cyhal_uart_clear(&soil_sensor_uart_obj); //clear if any unread uart buffers

	uint8_t moisture = values[3]<<8|values[4];
    float soil_mois = moisture/10;
	//printf("Soil moisture: %.2f%%\n", soil_mois);
	return soil_mois;

}

int read_conductivity(void){

	uint8_t query_buf[8] = { 0x01, 0x03, 0x00, 0x15, 0x00, 0x01, 0x95, 0xCE };
	size_t query_length = 8;

	cyhal_gpio_write(DE, true);
	cyhal_gpio_write(RE, true);

	vTaskDelay(2);

	cyhal_uart_write(&soil_sensor_uart_obj, (void*)query_buf, &query_length);

	cyhal_gpio_write(DE, false);
	cyhal_gpio_write(RE, false);

	vTaskDelay(500);

	uint8_t read_data;
	for (uint8_t i = 0; i < 8; i++) {
	    cyhal_uart_getc(&soil_sensor_uart_obj, &read_data, 0);
	    values[i] = read_data;
	  }
	cyhal_uart_clear(&soil_sensor_uart_obj); //clear if any unread uart buffers

	uint8_t conductivity = values[3]<<8|values[4];
    int soil_condc = conductivity;
	//printf("Soil conductivity: %dus/cm\n", soil_condc);
	return soil_condc;

}

int read_nitrogen(void){

	uint8_t query_buf[8] = { 0x01, 0x03, 0x00, 0x1E, 0x00, 0x01, 0xE4, 0x0C };
	size_t query_length = 8;

	cyhal_gpio_write(DE, true);
	cyhal_gpio_write(RE, true);

	vTaskDelay(2);

	cyhal_uart_write(&soil_sensor_uart_obj, (void*)query_buf, &query_length);

	cyhal_gpio_write(DE, false);
	cyhal_gpio_write(RE, false);

	vTaskDelay(500);

	uint8_t read_data;
	for (uint8_t i = 0; i < 8; i++) {
	    cyhal_uart_getc(&soil_sensor_uart_obj, &read_data, 0);
	    values[i] = read_data;
	  }
	cyhal_uart_clear(&soil_sensor_uart_obj); //clear if any unread uart buffers

	uint8_t nitrogen = values[3]<<8|values[4];
    int soil_nitrogen = nitrogen;
	//printf("Soil temperature: %dmg/kg\n", soil_nitrogen);
	return soil_nitrogen;

}

int read_phosphorous(void){

	uint8_t query_buf[8] = { 0x01, 0x03, 0x00, 0x1F, 0x00, 0x01, 0xB5, 0xCC };
	size_t query_length = 8;

	cyhal_gpio_write(DE, true);
	cyhal_gpio_write(RE, true);

	vTaskDelay(2);

	cyhal_uart_write(&soil_sensor_uart_obj, (void*)query_buf, &query_length);

	cyhal_gpio_write(DE, false);
	cyhal_gpio_write(RE, false);

	vTaskDelay(500);

	uint8_t read_data;
	for (uint8_t i = 0; i < 8; i++) {
	    cyhal_uart_getc(&soil_sensor_uart_obj, &read_data, 0);
	    values[i] = read_data;
	  }
	cyhal_uart_clear(&soil_sensor_uart_obj); //clear if any unread uart buffers

	uint8_t phosphorous = values[3]<<8|values[4];
    int soil_phosphorous = phosphorous;
	//printf("Soil temperature: %dmg/kg\n", soil_phosphorous);
	return soil_phosphorous;

}


int read_potassium(void){

	uint8_t query_buf[8] = { 0x01, 0x03, 0x00, 0x20, 0x00, 0x01, 0x85, 0xC0 };
	size_t query_length = 8;
	cyhal_gpio_write(DE, true);
	cyhal_gpio_write(RE, true);
	vTaskDelay(2);

	cyhal_uart_write(&soil_sensor_uart_obj, (void*)query_buf, &query_length);

	cyhal_gpio_write(DE, false);
	cyhal_gpio_write(RE, false);

	vTaskDelay(500);

	uint8_t read_data;
	for (uint8_t i = 0; i < 8; i++) {
	    cyhal_uart_getc(&soil_sensor_uart_obj, &read_data, 0);
	    values[i] = read_data;
	  }
	cyhal_uart_clear(&soil_sensor_uart_obj); //clear if any unread uart buffers

	uint8_t potassium = values[3]<<8|values[4];
    int soil_potassium = potassium;
	return soil_potassium;

}
/* [] END OF FILE */




