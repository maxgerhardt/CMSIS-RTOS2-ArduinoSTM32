#include <Arduino.h>
#include <cmsis_os2.h>

void PrintWithoutRace(const String& toPrint);

void Thread_MsgQueue1(void *argument); // thread function 1
void Thread_MsgQueue2(void *argument); // thread function 2
osThreadId_t tid_Thread_MsgQueue1;	   // thread id 1
osThreadId_t tid_Thread_MsgQueue2;	   // thread id 2

#define MSGQUEUE_OBJECTS 4 // number of Message Queue Objects

typedef struct
{ // object data type
	uint8_t Buf[32];
	uint8_t Idx;
} MEM_BLOCK_t;

typedef struct
{ // object data type
	uint8_t Buf[32];
	uint8_t Idx;
} MSGQUEUE_OBJ_t;

osMemoryPoolId_t mpid_MemPool2; // memory pool id

osMessageQueueId_t mid_MsgQueue; // message queue id

int Init_MsgQueue(void)
{

	mpid_MemPool2 = osMemoryPoolNew(MSGQUEUE_OBJECTS, sizeof(MSGQUEUE_OBJ_t), NULL);
	if (!mpid_MemPool2)
	{
		; // MemPool object not created, handle failure
	}

	mid_MsgQueue = osMessageQueueNew(MSGQUEUE_OBJECTS, sizeof(MSGQUEUE_OBJ_t), NULL);
	if (!mid_MsgQueue)
	{
		; // Message Queue object not created, handle failure
	}

	   osThreadAttr_t threadAttr;
  
	unsigned stackSize = 100; // 100 stack elements (400 bytes)
	osPriority_t priority = osPriorityNormal;

	//Set thread attributes
	threadAttr.attr_bits = 0;
	threadAttr.cb_mem = NULL;
	threadAttr.cb_size = 0;
	threadAttr.stack_mem = NULL;
	threadAttr.stack_size = stackSize * sizeof(unsigned);
	threadAttr.priority = (osPriority_t) priority;
	threadAttr.tz_module = 0;
	threadAttr.reserved = 0;

	threadAttr.name = "Thread 1";
	tid_Thread_MsgQueue1 = osThreadNew(Thread_MsgQueue1, NULL, &threadAttr);
	if (!tid_Thread_MsgQueue1)
	{
		return (-1);
	}
	threadAttr.name = "Thread 2";
	tid_Thread_MsgQueue2 = osThreadNew(Thread_MsgQueue2, NULL, &threadAttr);
	if (!tid_Thread_MsgQueue2)
	{
		return (-1);
	}

	return (0);
}

void Thread_MsgQueue1(void *argument)
{
	MEM_BLOCK_t *pMsg = 0;

	while (1)
	{
		pMsg = (MEM_BLOCK_t *)osMemoryPoolAlloc(mpid_MemPool2, 0); // get Mem Block, no timeout
		if (pMsg)
		{						 // Mem Block was available
			pMsg->Buf[0] = (uint8_t) random(255); // do some work...
			pMsg->Idx = 0;
			PrintWithoutRace(
				"[" + String(millis()) + "]"
				"[" + String(osThreadGetName(osThreadGetId())) +  "] "
				"Sending message with content 0x" + String(pMsg->Buf[0], HEX) + " Id: " + String(pMsg->Idx));
			osMessageQueuePut(mid_MsgQueue, &pMsg, 0, 0);
		}

		//wait until next queue put
		osDelay(500);
	}
}

void Thread_MsgQueue2(void *argument)
{
	osStatus_t status;
	MEM_BLOCK_t *pMsg = 0;
	
	while (1)
	{
		status = osMessageQueueGet(mid_MsgQueue, &pMsg, NULL, 500); // wait for message for 500 ticks
		if (status == osOK)
		{
			if (pMsg)
			{
				PrintWithoutRace(
					"[" + String(millis()) + "]"
					"[" + String(osThreadGetName(osThreadGetId())) +  "] "
					"Received message with content 0x" + String(pMsg->Buf[0], HEX) + " Id: " + String(pMsg->Idx));
				osMemoryPoolFree(mpid_MemPool2, pMsg); // free memory allocated for message
			}
		}
	}
}

/* Mutex-protected serial printing. Serial will crash otherwise! */
osMutexId_t print_mutex;
void Init_RaceFree_Print() {
	print_mutex = osMutexNew(NULL);
}

void PrintWithoutRace(const String& toPrint) {
	osMutexAcquire(print_mutex, osWaitForever);
	Serial.println(toPrint);
	Serial.flush();
	osMutexRelease(print_mutex);
}

void setup()
{
	Serial.begin(115200);
	pinMode(LED_BUILTIN, OUTPUT);

	osKernelInitialize();

	//initialize semaphore
	Init_RaceFree_Print();
	int init_ok = Init_MsgQueue();
	if(init_ok != 0) {
		Serial.println("Message queue / thread allocation has failed!");
	}

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
