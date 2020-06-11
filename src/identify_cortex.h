#ifndef IDENTIFY_CORTEX_H_
#define IDENTIFY_CORTEX_H_

/* figure out the type of ARM cortex CPU from the
 * compile-time defines fed-in by the compiler.
 * We deduce this from the STM32xx series here.
 * Each series uses one ARM core type.
 * See https://www.st.com/en/microcontrollers-microprocessors/stm32-mainstream-mcus.html
 * */

#define CORTEX_M_TYPE_M0_OR_M0PLUS 0
#define CORTEX_M_TYPE_M3 3
#define CORTEX_M_TYPE_M4F 4
#define CORTEX_M_TYPE_M7 7
#define CORTEX_M_TYPE_ARMV8 8
#define CORTEX_M_TYPE_CORTEX_A 9
#define CORTEX_M_TYPE_OTHER 10

#ifdef STM32F0xx
#define CORTEX_M_TYPE CORTEX_M_TYPE_M0_OR_M0PLUS
#elif defined(STM32F1xx)
#define CORTEX_M_TYPE CORTEX_M_TYPE_M3
#elif defined(STM32F2xx)
#define CORTEX_M_TYPE CORTEX_M_TYPE_M3
#elif defined(STM32F3xx)
#define CORTEX_M_TYPE CORTEX_M_TYPE_M4F
#elif defined(STM32F4xx)
#define CORTEX_M_TYPE CORTEX_M_TYPE_M4F
#elif defined(STM32F7xx)
#define CORTEX_M_TYPE CORTEX_M_TYPE_M7
#elif defined(STM32G0xx)
#define CORTEX_M_TYPE CORTEX_M_TYPE_M0_OR_M0PLUS
#elif defined(STM32G4xx)
#define CORTEX_M_TYPE CORTEX_M_TYPE_M4F
#elif defined(STM32H7xx)
/*
 * well those are either single-core M7 or
 * dual-core M7 *and* a M4.. treat as M7
 */
#define CORTEX_M_TYPE CORTEX_M_TYPE_M7
#elif defined(STM32L0xx)
#define CORTEX_M_TYPE CORTEX_M_TYPE_M0_OR_M0PLUS
#elif defined(STM32L1xx)
#define CORTEX_M_TYPE CORTEX_M_TYPE_M3
#elif defined(STM32L4xx)
#define CORTEX_M_TYPE CORTEX_M_TYPE_M4F
#elif defined(STM32MP1xx)
/*
 * contains dual-core ARM Cortex-A7 and Cortex M4.
 * Cortex-A and ARMv8 support was stripped - future todo.
 */
#warning "Unsupported architecture ARMv8 selected.."
#define CORTEX_M_TYPE CORTEX_M_TYPE_M4F
#elif defined(STM32WBxx)
#define CORTEX_M_TYPE CORTEX_M_TYPE_M4F
#else
#error "STM32YYxx chip series is not defined in boards.txt."
#endif

#endif /* IDENTIFY_CORTEX_H_ */
