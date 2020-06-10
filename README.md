# CMSIS-RTOS2 with ArduinoSTM32

CMSIS-RTOS2 library integrated with the STM32 Arduino core and PlatformIO

## Implementation details

The CMSIS-RTOS2 implements the global `SVC_Handler` interrupt. However, it also needs to run in the `Systick_Handler` -- which is occupied by STM32Duino by default. Thankfully, the Arduino core creators have created a `weak` function `osSystickHandler()`, which will be called if such a function exists.

https://github.com/stm32duino/Arduino_Core_STM32/blob/master/libraries/SrcWrapper/src/stm32/clock.c#L90

Thus, the CMSIS-RTOS2 interrupt files (`irq_cm3.S` and friends) were re-written to expose the `osSystickHandler` symbol and thus get called in the Systick handler without further hacking. Awesome!

Also, the library contains a `stm32duino_extensions.c` which implements the optional `yield()` function. We use this to get a more multi-threaded friendly `delay()` implementation, see below.

## How to use in your sketch

In your `setup()` function, you must initial the kernel, add all the threads (and other objects like mutexes and queues) you want to add, and then start the kernel.

This can be achieved by using a `setup()` function like

```cpp
void example_thread_function(void* startArg) {
	while(true) {
		osDelay(1000);
	}
}

void setup()
{
	Serial.begin(115200);

	osKernelInitialize();

        //add threads
 	osThreadNew(example_thread_function, NULL, NULL);

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
```

## Caveats

### `delay()`

If you want to wait in a multi-threaded program, use `osDelay` instead with the number of milliseconds. Your thread will be put to sleep and other threads can execute. 

You can still use `delay()` though -- the weak function `yield()` was implemented, which STM32Duino automatically calls when delaying. The `yield()` function will call into the `osThreadYield()` function, which gives other threads a chance to execute

### `Serial.println()`

Printing from multiple threads at the same time **will crash your firmware**. 

Make sure you are using a mutually-exclusive lock (aka mutex) when printing from multiple threads. This is illustrated in the MessageQueue` example. 

Or, just make sure to ever only print from one thread if acquiring a mutex is too expensive.


## Arduino IDE support?

This library is only usable with PlatformIO, because a config file and Python script is needed to set up the include flags / folders **and** assemble the correct IRQ file for the Cortex M0, M3 or M4 processor. 

I have not bothered to make this Arduino IDE compatible (yet..). 

## Example project

Just use a `platformio.ini` like 

```
[platformio]
default_envs = nucleo_f103rb	

[env]
lib_deps = 
	CMSIS-RTOS2-ArduinoSTM32=https://github.com/maxgerhardt/CMSIS-RTOS2-ArduinoSTM32.git

; target a STM32F103RB chip
[env:nucleo_f103rb]
platform = ststm32
board = nucleo_f103rb
framework = arduino
```

and use the `.cpp` files from one of the examples. Change the `board` to your bidding -- all Cortex M0, M3 and M4 boards should work.
