#!/usr/bin/env python3
"""Build the replay fixtures (flight_*.h) from the post-flight-analysis repository.

Usage: python3 make_fixtures.py PATH_TO_POST_FLIGHT_ANALYSIS [OUT_DIR]

Reads each flight's TeleMega log in the standard interim format
(data/<flight>/interim/telemega_std.csv, schema gfr-0.2) and writes one header per flight with:
- the samples the replay needs: time (centiseconds from liftoff), axial acceleration (cm/s^2,
  the log's `acceleration`, which is the axial accelerometer reading less 1 g) and pressure (Pa),
  delta-encoded to keep the file small, every row of the log kept;
- the reference event times from the same log: the TeleMega flight-state changes (boost, the
  first of fast/coast, drogue, main, landed); the baro apogee (highest 5-sample median of height
  before the drogue charge fired); the time the height first drops below the main deployment
  altitude; and ground contact (the height stays within 3 m of its resting value from then on).
GNSS rows that repeat the previous sensor values are dropped.

Standard library only. The output depends only on the input files, so rerunning it on the same
post-flight-analysis commit reproduces the committed headers byte for byte.
"""
import csv
import hashlib
import os
import sys

# The TeleMega's settings on all three flights (each .eeprom config): "apogee_delay": 3,
# "main_deploy": 425
APOGEE_DELAY_S = 3.0
MAIN_DEPLOY_M = 425.0
# Ground contact: the logged height stays within this of its resting value
GROUND_BAND_M = 3.0

FLIGHTS = [
    # (fixture name, data directory, description)
    ("prometheus", "2022-06-24_prometheus_sac22", "Prometheus, SAC 2022, 2022-06-24, TeleMega s/n 6583 flight 0003"),
    ("hyperion1", "2024-06-21_hyperion1_sac24", "Hyperion I, SAC 2024, 2024-06-21, TeleMega s/n 6583 flight 0004"),
    ("hyperion2", "2025-08-21_hyperion2_lc25", "Hyperion II, LC 2025, 2025-08-21, TeleMega s/n 6583 flight 0005"),
]


def flight_rows(path):
    rows = []
    with open(path, newline="") as f:
        for r in csv.DictReader(f):
            if not r["a_up_mps2"] or not r["pressure_pa"]:
                continue
            rows.append(r)
    return rows


def build(pfa, name, directory, description):
    path = os.path.join(pfa, "data", directory, "interim", "telemega_std.csv")
    raw = open(path, "rb").read()
    rows = flight_rows(path)
    samples = []
    for r in rows:
        t = round(float(r["t_flight_s"]) * 100)
        a = round(float(r["a_up_mps2"]) * 100)
        p = round(float(r["pressure_pa"]))
        # a row that repeats the previous sample's acceleration and pressure is a GNSS row
        # carrying the last sensor values, not a new sample
        if samples and (t <= samples[-1][0] or (a, p) == samples[-1][1:]):
            continue
        samples.append((t, a, p))
    # reference events from the TeleMega state column
    events = {}
    prev = None
    for r in rows:
        s = r["state"]
        if s != prev:
            t = float(r["t_flight_s"])
            if s == "boost":
                events.setdefault("liftoff", 0.0)
            elif s in ("fast", "coast"):
                events.setdefault("burnout", t)
            elif s == "drogue":
                events.setdefault("apogee", t)
            elif s == "main":
                events.setdefault("main", t)
            elif s == "landed":
                events.setdefault("landed", t)
            prev = s
    # apogee from the baro: the highest 5-sample median of height before the drogue charge. The
    # TeleMega fired the charge APOGEE_DELAY_S after its apogee detection, and the ejection's
    # pressure transient reads as a higher altitude; the median drops single-sample outliers.
    before_drogue = [r for r in rows if float(r["t_flight_s"]) < events["apogee"] + APOGEE_DELAY_S]
    heights = [float(r["alt_agl_m"]) for r in before_drogue]
    medians = [sorted(heights[max(0, i - 2):i + 3])[len(heights[max(0, i - 2):i + 3]) // 2] for i in range(len(heights))]
    i_peak = max(range(len(medians)), key=lambda i: (medians[i], -i))
    peak = dict(before_drogue[i_peak], alt_agl_m=medians[i_peak])
    # height first below the main deployment altitude after apogee, and ground contact: the
    # first time after which the height stays within GROUND_BAND_M of its median over the last 5 s
    t_rows = [float(r["t_flight_s"]) for r in rows]
    h_rows = [float(r["alt_agl_m"]) for r in rows]
    t_apogee = float(peak["t_flight_s"])
    below_main = next(t for t, h in zip(t_rows, h_rows) if t > t_apogee and h < MAIN_DEPLOY_M)
    rest = sorted(h for t, h in zip(t_rows, h_rows) if t > t_rows[-1] - 5.0)
    rest = rest[len(rest) // 2]
    contact = t_rows[-1]
    for t, h in reversed(list(zip(t_rows, h_rows))):
        if abs(h - rest) > GROUND_BAND_M:
            break
        contact = t
    pad_msl = float(rows[0]["alt_baro_m_msl"]) - float(rows[0]["alt_agl_m"])
    flat = []
    last = (0, 0, 0)
    for s in samples:
        flat.extend(v - w for v, w in zip(s, last))
        last = s
    lines = []
    for i in range(0, len(flat), 24):
        lines.append("    " + ", ".join(str(v) for v in flat[i:i + 24]) + ",")
    ms = lambda key: round(events[key] * 1000)
    out = f"""/**
 * @file flight_{name}.h
 * @brief Replay fixture: {description}.
 *
 * Generated by make_fixtures.py from post-flight-analysis
 * data/{directory}/interim/telemega_std.csv (sha256 {hashlib.sha256(raw).hexdigest()}).
 * Flight data (c) the post-flight-analysis contributors, CC BY 4.0; see ATTRIBUTION.md.
 * Do not edit by hand.
 */
#pragma once

#include "replay_fixture.h"

namespace flight_{name}
{{
// {len(samples)} samples of (time cs, axial acceleration less 1 g cm/s^2, pressure Pa), each
// stored as the difference from the previous sample
static const int32_t deltas[] = {{
{chr(10).join(lines)}
}};

static const ReplayFixture fixture = {{
    "{description}",
    deltas,
    {len(samples)},
    // TeleMega flight states: boost (log time 0), first of fast/coast, drogue, main, landed (ms)
    {ms("liftoff")}, {ms("burnout")}, {ms("apogee")}, {ms("main")}, {ms("landed")},
    // baro apogee (highest 5-sample median height before the drogue charge): time (ms), height (cm)
    {round(float(peak["t_flight_s"]) * 1000)}, {round(float(peak["alt_agl_m"]) * 100)},
    // height first below {MAIN_DEPLOY_M:.0f} m after apogee, and ground contact (height within {GROUND_BAND_M:.0f} m of rest) (ms)
    {round(below_main * 1000)}, {round(contact * 1000)},
    // pad altitude above sea level from the log (cm)
    {round(pad_msl * 100)},
}};
}} // namespace flight_{name}
"""
    return out


def main():
    pfa = sys.argv[1]
    out_dir = sys.argv[2] if len(sys.argv) > 2 else os.path.dirname(os.path.abspath(__file__))
    for name, directory, description in FLIGHTS:
        text = build(pfa, name, directory, description)
        with open(os.path.join(out_dir, f"flight_{name}.h"), "w", newline="\n") as f:
            f.write(text)
        print(f"flight_{name}.h: {len(text)} bytes")


if __name__ == "__main__":
    main()
