/**
 * @file test_flight_replay.cpp
 * @brief Logged flights replayed through the flight step; its events must match the logged ones.
 *
 * @details
 * Three TeleMega logs from post-flight-analysis (test/replay/, CC BY 4.0, see ATTRIBUTION.md):
 * Prometheus (SAC 2022), Hyperion I (SAC 2024) and Hyperion II (LC 2025). Each is converted to what
 * this flight computer's sensors would read (LSM6DSOX 16 g, H3LIS331DL 100 g, MS5611, see
 * flight_harness.h) and replayed one loop pass at a time (replay_fixture.h). The flight computer
 * uses the TeleMega's own deployment settings for these flights: main at 425 m, apogee charge 3 s
 * after apogee; single-stage profile, apogee and main pyros.
 *
 * Each event is checked against a reference taken from the same log, with a stated tolerance:
 * - liftoff: the log's time 0 (the TeleMega's boost detection), within 0.15 s;
 * - burnout: the TeleMega's change from boost to fast or coast, within 0.3 s;
 * - apogee: the baro apogee (highest 5-sample median of logged height before the drogue charge);
 *   the flight computer detects it on velocity falling below zero (fusion) or -10 m/s (baro,
 *   accelerometer), so from 0 to 0.6 s after it;
 * - main: the logged height first below 425 m after apogee, within 0.2 s;
 * - touchdown: ground contact (logged height within 3 m of its resting value from then on), from
 *   4 s before to 1 s after. Under the main the TeleMega logged pressure at 10 Hz with about +-2 m
 *   of scatter; on Hyperion I that scatter meets the touchdown condition (barometric velocity
 *   under 1 m/s for 6 samples, below 46 m) about 3.7 s before contact, 8 m up.
 */
#include <unity.h>

#include "../replay/flight_hyperion1.h"
#include "../replay/flight_hyperion2.h"
#include "../replay/flight_prometheus.h"
#include "../flight_state.h"

static std::vector<uint8_t> initialState;

static ReplayResult replay(const ReplayFixture &fixture)
{
  restoreFlightState(initialState);
  replaySettings();
  const ReplayResult r = replayFlight(fixture);
  printf("%s\n", fixture.name);
  printf("  liftoff   %8.3f s (log %8.3f)\n", r.liftoff, fixture.liftoffMs * 1e-3);
  printf("  burnout   %8.3f s (log %8.3f)\n", r.burnout, fixture.burnoutMs * 1e-3);
  printf("  apogee    %8.3f s (baro apogee %8.3f, TeleMega %8.3f)%s%s%s\n", r.apogee, fixture.peakMs * 1e-3, fixture.apogeeMs * 1e-3, r.apogeeByAccel ? " accel" : "", r.apogeeByBaro ? " baro" : "",
         r.apogeeByFusion ? " fusion" : "");
  printf("  main      %8.3f s (below 425 m %8.3f, TeleMega %8.3f)\n", r.mainDeploy, fixture.belowMainMs * 1e-3, fixture.mainMs * 1e-3);
  printf("  touchdown %8.3f s (ground contact %8.3f, TeleMega landed %8.3f)\n", r.touchdown, fixture.groundContactMs * 1e-3, fixture.landedMs * 1e-3);
  TEST_ASSERT_TRUE(r.maxActions <= FLIGHT_ACTIONS_MAX);
  return r;
}

static void checkEvents(const ReplayFixture &f, const ReplayResult &r)
{
  TEST_ASSERT_FLOAT_WITHIN(0.15F, f.liftoffMs * 1e-3F, r.liftoff);
  TEST_ASSERT_FLOAT_WITHIN(0.3F, f.burnoutMs * 1e-3F, r.burnout);
  const float apogeeLag = r.apogee - f.peakMs * 1e-3F;
  TEST_ASSERT_TRUE_MESSAGE(apogeeLag >= 0.0F && apogeeLag <= 0.6F, "apogee not within 0 to 0.6 s after the baro apogee");
  TEST_ASSERT_FLOAT_WITHIN(0.01F, 3.0F, r.apogeeFire - r.apogee);
  TEST_ASSERT_FLOAT_WITHIN(0.2F, f.belowMainMs * 1e-3F, r.mainDeploy);
  const float touchdownLead = r.touchdown - f.groundContactMs * 1e-3F;
  TEST_ASSERT_TRUE_MESSAGE(touchdownLead >= -4.0F && touchdownLead <= 1.0F, "touchdown not within 4 s before to 1 s after ground contact");
}

void test_prometheus() { checkEvents(flight_prometheus::fixture, replay(flight_prometheus::fixture)); }
void test_hyperion1() { checkEvents(flight_hyperion1::fixture, replay(flight_hyperion1::fixture)); }
void test_hyperion2() { checkEvents(flight_hyperion2::fixture, replay(flight_hyperion2::fixture)); }

void setUp() {}
void tearDown() {}

int main()
{
  initialState = saveFlightState();
  UNITY_BEGIN();
  RUN_TEST(test_prometheus);
  RUN_TEST(test_hyperion1);
  RUN_TEST(test_hyperion2);
  return UNITY_END();
}
