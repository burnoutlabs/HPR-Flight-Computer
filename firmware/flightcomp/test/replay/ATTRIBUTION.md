# Flight data attribution

The replay fixtures in this directory (`flight_prometheus.h`, `flight_hyperion1.h`,
`flight_hyperion2.h`) are derived from flight logs in
[burnoutlabs/post-flight-analysis](https://github.com/burnoutlabs/post-flight-analysis),
at commit `0887cdd`, which licenses its data under the
[Creative Commons Attribution 4.0 International License (CC BY 4.0)](https://creativecommons.org/licenses/by/4.0/).

| Fixture | Flight | Source file |
| --- | --- | --- |
| `flight_prometheus.h` | Prometheus, Spaceport America Cup 2022, 2022-06-24; TeleMega s/n 6583 flight 0003 | `data/2022-06-24_prometheus_sac22/interim/telemega_std.csv` |
| `flight_hyperion1.h` | Hyperion I, Spaceport America Cup 2024, 2024-06-21; TeleMega s/n 6583 flight 0004 | `data/2024-06-21_hyperion1_sac24/interim/telemega_std.csv` |
| `flight_hyperion2.h` | Hyperion II, Launch Canada 2025, 2025-08-21; TeleMega s/n 6583 flight 0005 | `data/2025-08-21_hyperion2_lc25/interim/telemega_std.csv` |

Changes made: only the time, acceleration and pressure columns are kept, rows that repeat the
previous sample's acceleration and pressure are dropped, values are rounded (0.01 s, 0.01 m/s²,
1 Pa) and delta-encoded, and reference event times are computed from the same log. Each header
records the sha256 of its source file. `make_fixtures.py` regenerates them:

```sh
python3 firmware/flightcomp/test/replay/make_fixtures.py /path/to/post-flight-analysis
```
