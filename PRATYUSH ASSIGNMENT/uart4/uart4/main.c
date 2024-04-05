/*
 * uart4.c
 *
 * Created: 30-03-2024 21:36:02
 * Author : DELL
 */ 
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/eeprom.h>
#include <stdio.h> 


#define BAUD_RATE 2400
#define UBRR_VALUE ((F_CPU / (16UL * BAUD_RATE)) - 1)
int byte_count = 0;

void USART_Init() {
	UBRR0H = (uint8_t)(UBRR_VALUE >> 8);
	UBRR0L = (uint8_t)(UBRR_VALUE);
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
    UCSR0C = (3 << UCSZ00);
}

void USART_Transmit(uint8_t data) {
	
	while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = data;
}

char USART_Receive() {
	
	while (!(UCSR0A & (1 << RXC0)));
    return UDR0;
}

void reverse(char* str, int len) {
	int i = 0, j = len - 1, temp;
	while (i < j) {
		temp = str[i];
		str[i] = str[j];
		str[j] = temp;
		i++;
		j--;
	}
}


int intToStr(int x, char str[], int d) {
	int i = 0;
	while (x) {
		str[i++] = (x % 10) + '0';
		x = x / 10;
	}

	while (i < d)
	str[i++] = '0';

	reverse(str, i);
	str[i] = '\0';
	return i;
}


void ftoa(float n, char* res, int afterpoint) {
	
	int ipart = (int)n;
    float fpart = n - (float)ipart;

	int i = intToStr(ipart, res, 0);
    
	if (afterpoint != 0) {
		res[i] = '.';

		
		fpart = fpart * pow(10, afterpoint);

		intToStr((int)fpart, res + i + 1, afterpoint);
	}
}

void USART_TransmitString(const char *str) {
	while (*str) {
		USART_Transmit(*str++);
	}
}


void USART_ReceiveString(char *buffer, uint8_t max_length) {
	uint8_t index = 0;
	char received_char;

	while (index < max_length - 1) {  
		received_char = USART_Receive();

		if (received_char == '\n' || received_char == '\r') {
			break;  
		}

		buffer[index++] = received_char;
	}

	buffer[index] = '\0';  
}

void EEPROM_WriteString(uint16_t address, const char *str) {
	while (*str) {
		eeprom_write_byte((uint8_t *)address++, *str++);
		byte_count++;
	}
}

void EEPROM_ReadString(uint16_t address, char *buffer, uint8_t max_length) {
	uint8_t index = 0;
	uint8_t byte;

	while (index < max_length - 1) {  
		byte = eeprom_read_byte((uint8_t *)address++);

		if (byte == '\0') {
			break;  
		}

		buffer[index++] = byte;
	}

	buffer[index] = '\0';  
}

void start_clock() {
	TCCR1B = (1 << CS10); 
}

uint32_t stop_clock() {
	TCCR1B = 0; 
	return TCNT1; 
}

void int_to_str(int num, char *str, size_t size) {
	snprintf(str, size, "%d", num);
}

void uint32_to_str(uint32_t num, char *str, size_t size) {
	snprintf(str, size, "%lu", (unsigned long)num);
}


float ticks_to_seconds(uint32_t ticks) {
	float time_per_tick = 1.0 / F_CPU; 
	return ticks * time_per_tick;
}


void float_to_str(float value, char *str, size_t size) {
	snprintf(str, size, "%.2f", value);  
}

int main(void) {
	char input_string[1000];
	char eeprom_string[1000];  
	char read_time_seconds_str[4];
	USART_Init();  

	while (1) {
		
		start_clock();
		USART_ReceiveString(input_string, sizeof(input_string));

		
		EEPROM_WriteString(0, input_string);

		
		EEPROM_ReadString(0, eeprom_string, sizeof(input_string));
		

		
		USART_TransmitString("string from eeprom:");
		USART_TransmitString(eeprom_string);
		USART_TransmitString("\n");
		uint32_t read_time_cycles = stop_clock();
		_delay_ms(1000);
		
		
		float read_time_secondss = ticks_to_seconds(read_time_cycles);
		float read_time_seconds = byte_count/read_time_secondss;
		ftoa(read_time_seconds, read_time_seconds_str, 4);
		USART_TransmitString("Time Taken:");
		USART_TransmitString(read_time_seconds_str);
		USART_TransmitString("\n");
		read_time_secondss=0;
		byte_count=0;
		

		
		_delay_ms(1000);
	}

	return 0;
}

