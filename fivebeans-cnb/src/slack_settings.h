// config page: https://contentful.slack.com/services/299624817991

#define SLACK_WH_HOST "hooks.slack.com"
#ifndef SLACK_WH_PATH
#error You must set SLACK_WH_PATH env var (either in platformio.ini, .envrc, or in other means)
#endif

#define SLACK_SSL_FINGERPRINT "C1 0D 53 49 D2 3E E5 2B A2 61 D5 9E 6F 99 0D 3D FD 8B B2 B3"
// https://www.ssllabs.com/ssltest/analyze.html?d=hooks.slack.com
// Valid until: Fri, 12 Feb 2021 12:00:00 UTC (expires in 2 years)
