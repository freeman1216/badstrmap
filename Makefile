CC = arm-none-eabi-gcc
CFLAGS = -ggdb -Wall -Wextra  -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard -fno-strict-aliasing 
LDFLAGS = -Tstm32f411ceu6.ld -nolibc --specs=nosys.specs -nostartfiles  
INCLUDES = -Iinc/ -Iinc/driver/ 

SRC_DIR = src
BUILD_DIR = build

SOURCES = $(SRC_DIR)/startup_stm32f411ceu6.c 


MAIN_SRC = $(SOURCES) $(SRC_DIR)/main.c


MAIN_BIN = $(BUILD_DIR)/main.elf


###############
# Build rules #
###############

$(MAIN_BIN): $(BUILD_DIR)
	$(CC) $(CFLAGS) $(LDFLAGS) $(INCLUDES) $(MAIN_SRC) -o $@

all: clean $(MAIN_BIN)


.PHONY: debug
debug:

	@echo "Flashing $(MAIN_BIN)..."
	openocd -f /usr/share/openocd/scripts/interface/stlink.cfg \
	-f /usr/share/openocd/scripts/target/stm32f4x.cfg \
		-c "program $(MAIN_BIN) reset exit"

	@echo "Starting OpenOCD server..."
	openocd -f /usr/share/openocd/scripts/interface/stlink.cfg \
	-f /usr/share/openocd/scripts/target/stm32f4x.cfg & \
	gf2 $(MAIN_BIN) \
		-ex "target remote localhost:3333" \
		-ex "monitor reset halt" 
	
	pkill openocd




###############
# Clean       #
###############
.PHONY: clean
clean:
	rm -f $(BUILD_DIR)/*.elf

###############
# Build dir   #
###############
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)
