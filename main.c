/*----------------------------------------------------------------------------
 * Name:    main.c
 *----------------------------------------------------------------------------*/

#include "RTE_Components.h"
#include  CMSIS_device_header
#include "unity.h"
#include <stdio.h>
#include "main.h"

#ifdef MIMXRT1052_SERIES
#include "clock_config.h"
#include "board.h"
#include "pin_mux.h"
#include "fsl_iomuxc.h"

// Callbacks for LPUART1 Driver
uint32_t LPUART1_GetFreq   (void) { return BOARD_BOOTCLOCKRUN_UART_CLK_ROOT; }
void     LPUART1_InitPins  (void) { /* Done in BOARD_InitDEBUG_UART function */ }
void     LPUART1_DeinitPins(void) { /* Not implemented */ }

// Callbacks for LPUART3 Driver
uint32_t LPUART3_GetFreq   (void) { return BOARD_BOOTCLOCKRUN_UART_CLK_ROOT; }
void     LPUART3_InitPins  (void) { /* Done in BOARD_InitARDUINO_UART function */ }
void     LPUART3_DeinitPins(void) { /* Not implemented */ }
#endif

extern void stdio_init (void);

/* Application function to test */
static int my_sum(int a, int b) {
  return a + b;
}

/*============= UNIT TESTS ============== */

/* Called in RUN_TEST before executing test function */
void setUp(void) {
  // set stuff up here
}

/* Called in RUN_TEST after executing test function */
void tearDown(void) {
  // clean stuff up here
}

/* Testing summation of positive integers */
static void test_my_sum_pos(void) {
  const int sum = my_sum(1, 1);
  TEST_ASSERT_EQUAL_INT(2, sum);
}

/* Testing summation of negative integers */
static void test_my_sum_neg(void) {
  const int sum = my_sum(-1, -1);
  TEST_ASSERT_EQUAL_INT(-2, sum);
}

/* Testing summation of zeros */
static void test_my_sum_zero(void) {
  const int sum = my_sum(0, 0);
  TEST_ASSERT_EQUAL_INT(0, sum);
}

/* Failing test with incorrect summation value */
static void test_my_sum_fail(void) {
  const int sum = my_sum(1, -1);
  TEST_ASSERT_EQUAL_INT(2, sum);
}

/*  Main: Run tests */
int main(void) {
	
#ifdef MIMXRT1052_SERIES
	BOARD_InitBootPins();
  BOARD_InitBootClocks();
  BOARD_InitDebugConsole();

  // Enable ENET_REF_CLK output mode
  IOMUXC_EnableMode(IOMUXC_GPR, kIOMUXC_GPR_ENET1TxClkOutputDir, true);

  NVIC_SetPriority(ENET_IRQn,    8U);
  NVIC_SetPriority(USDHC1_IRQn,  8U);
  NVIC_SetPriority(LPUART3_IRQn, 8U);

  SystemCoreClockUpdate();
#else
	stdio_init();
#endif
  printf("---[ UNITY BEGIN ]---\n");
  UNITY_BEGIN();
  RUN_TEST(test_my_sum_pos);
  RUN_TEST(test_my_sum_neg);
  RUN_TEST(test_my_sum_fail);
  RUN_TEST(test_my_sum_zero);
  const int result = UNITY_END();
  printf("---[ UNITY END ]---\n");
  return result;
}
