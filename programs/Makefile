TO_ROOT = ..
include $(TO_ROOT)/build-config/programs.mk

PROGRAMS = $(wildcard */.)

all: build_info build-programs install-programs
	@echo -n ${COLOR_RESET}

build_info:
	@echo ${COLOR_CYAN}Building programs...

build-programs: $(PROGRAMS)

$(PROGRAMS):
	$(MAKE) -C $@

install-programs: $(addprefix $(BUILD_DIR)/, $(PROGRAMS))
	@mkdir -p $(BIN_DIR)
	cp -rf $^ $(BIN_DIR)

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all install-programs clean install-programs $(PROGRAMS)
