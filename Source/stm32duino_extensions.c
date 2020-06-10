#include <cmsis_os2.h> 

/* implement actual thread yielding, e.g. used in verbatim delay */
void yield() {
    osThreadYield();
}