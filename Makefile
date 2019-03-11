PIO_CMD = pio
ROOT_DIR = $(shell git rev-parse --show-toplevel)
SHARED_LIB_DIR = $(ROOT_DIR)/shared_libs

init: $(PIO_CMD) $(SHARED_LIB_DIR)

$(PIO_CMD):
	@test -x $(which $(PIO_CMD)) \
		&& echo "platformio CLI installed." \
		|| echo "platformio CLI not installed. Visit https://platformio.org/install/cli for instructions."
.PHONY: pio

$(PIO_CMD)-upgrade: $(PIO_CMD)
	$(PIO_CMD) upgrade

$(PIO_CMD)-update: $(PIO_CMD)
	$(PIO_CMD) update

$(PIO_CMD)-all: $(PIO_CMD)-upgrade $(PIO_CMD)-update

build:
	$(PIO_CMD) run

upload:
	$(PIO_CMD) run -t upload

monitor:
	$(PIO_CMD) run -t monitor

pclean:
	$(PIO_CMD) run -t clean

$(SHARED_LIB_DIR):
	mkdir -p $@

# projects quick builds

## CNB

cnb:
	cd fivebeans-cnb && $(MAKE) build

cnb-upload:
	cd fivebeans-cnb && $(MAKE) upload
