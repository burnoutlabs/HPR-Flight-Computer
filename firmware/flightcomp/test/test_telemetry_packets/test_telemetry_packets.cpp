/**
 * @file test_telemetry_packets.cpp
 * @brief The flight computer's telemetry packets against the bytes it sent at V4.7.2.
 *
 * runTelemetryScenario() drives the real Telemetry.cpp through preflight, inflight and
 * postflight, with FHSS off and on, and records each packet and the frequency it went out
 * on. Every packet must match telemetry_golden.h: same count, same length, same bytes, and
 * with FHSS the same frequency.
 */
#include <unity.h>

#include <stdio.h>

#include "telemetry_golden.h"
#include "telemetry_scenarios.h"

void setUp() {}
void tearDown() {}

static void assertMatchesGolden(const GoldenPacket *golden, int goldenCount)
{
  char msg[96];
  TEST_ASSERT_EQUAL_INT_MESSAGE(goldenCount, sentCount, "number of packets sent");
  for (int i = 0; i < goldenCount; i++)
  {
    snprintf(msg, sizeof(msg), "packet %d length", i);
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(golden[i].len, sent[i].len, msg);
    snprintf(msg, sizeof(msg), "packet %d bytes", i);
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(golden[i].bytes, sent[i].bytes, golden[i].len, msg);
    snprintf(msg, sizeof(msg), "packet %d frequency", i);
    TEST_ASSERT_TRUE_MESSAGE(golden[i].freq == sent[i].freq, msg);
  }
}

void test_packets_match_v4_7_2_with_fhss_off()
{
  runTelemetryScenario(false, 433.250F);
  assertMatchesGolden(GOLDEN_FHSS_OFF, sizeof(GOLDEN_FHSS_OFF) / sizeof(GOLDEN_FHSS_OFF[0]));
}

void test_packets_match_v4_7_2_with_fhss_on()
{
  runTelemetryScenario(true, 915.100F);
  assertMatchesGolden(GOLDEN_FHSS_ON, sizeof(GOLDEN_FHSS_ON) / sizeof(GOLDEN_FHSS_ON[0]));
}

int main(int, char **)
{
  UNITY_BEGIN();
  RUN_TEST(test_packets_match_v4_7_2_with_fhss_off);
  RUN_TEST(test_packets_match_v4_7_2_with_fhss_on);
  return UNITY_END();
}
