#include <Arduino.h>
#include <cmsis_os2.h>

void blinky_led_thread(void *startArg)
{
	(void)startArg;
	while (true)
	{
		//use multi-thread friendly:
		//osDelay(500);
		//instead of delay. thanks to stm32duino_extensions.c, delay() will call into yield() which
		//is the RTOS yield(), but still yield()-ing extremely often when we just want to sleep is bad..
		digitalWrite(LED_BUILTIN, HIGH);
		osDelay(500);
		digitalWrite(LED_BUILTIN, LOW);
		osDelay(500);
	}
}

void print_millis_thread(void *startArg)
{
	(void)startArg;
	while (true)
	{
		Serial.println("The time is: " + String(millis()));
		osDelay(1000);
	}
}

void setup()
{
	Serial.begin(115200);
	pinMode(LED_BUILTIN, OUTPUT);

	osKernelInitialize();

	//start two thread: blink the main LED and print the millis() system time via serial.
	//start thread on function with NULL as argument and no thread attributes
	//thread attributes are useful for setting up a predefined thread stack, priorities and names etc.
	//default in RTX_Config.h: OS_STACK_SIZE = 256 bytes.
	//**if this is not enough, functions will crash. change at will**
	osThreadNew(blinky_led_thread, NULL, NULL);
	osThreadNew(print_millis_thread, NULL, NULL);

	//is kernel ready to run?
	if (osKernelGetState() == osKernelReady)
	{
		//then start it. will block eternally (or fatal error encountered)
		Serial.println("Starting CMSIS-RTOS2 kernel.");
		osKernelStart();
	}
	//fallback here if we get through..
	while (true)
	{
		Serial.println("Should never reach!");
		Serial.println("Kernel state is " + String((int)osKernelGetState()));
		Serial.flush();
		delay(500);
	}
}

void loop() { /* never reached */ }
