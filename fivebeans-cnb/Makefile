include ../Makefile

slack-cert-fingerprint:
	@openssl s_client -connect hooks.slack.com:443 < /dev/null 2>/dev/null | \
  openssl x509 -fingerprint -noout -in /dev/stdin | \
	tr ':' ' '
	@echo "Copy this fingerprint to src/slack_settings.h"
