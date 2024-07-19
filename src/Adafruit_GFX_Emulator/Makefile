EXE_NAME := Matrix_Display

.PHONY: all clean print
all: $(EXE_NAME)

EMULATOR_LIBS := Adafruit_GFX_dummy_display
EMULATOR_LIBS += Adafruit-GFX-Library
EMULATOR_LIBS += Arduino

LIBRARIES := BaseGame
LIBRARIES += Menu
LIBRARIES += MyDisplay
LIBRARIES += MyInputs
LIBRARIES += SnakeGame
LIBRARIES += Tetris
LIBRARIES += Utility

BUILD_DIR := build
EMULATOR_ROOT := $(realpath .)
PROJECT_ROOT := $(realpath ../../)

# recur_wildcard=$(wildcard $1$2)$(foreach dir,$(wildcard $1*),$(call recur_wildcard,$(dir)/,$2))

# SUBDIRS = $(wildcard */)
# $(info SUBDIRS="$(SUBDIRS)")
# PROJECT_SUBDIRS := $(wildcard $(PROJECT_ROOT)/$(SUBDIRS))
# $(info PROJECT_SUBDIRS="$(PROJECT_SUBDIRS)")
# NEW_SRCS := $(wildcard $(addsuffix *.cpp,$(PROJECT_SUBDIRS)))
# $(info NEW_SRCS="$(NEW_SRCS)")
# NEWNEW_SRCS := $(call recur_wildcard,$(PROJECT_ROOT)/,*.cpp)
# $(info NEWNEW_SRCS="$(NEWNEW_SRCS)")

$(shell mkdir -p $(BUILD_DIR))

CFLAGS = -Wall -Wextra -Wno-unused-parameter -g -std=gnu17 -lm $(shell sdl2-config --cflags) -DARDUINO=123 -DPC_BUILD
CXXFLAGS = -Wall -Wextra -Wno-unused-parameter -g -std=gnu++20 $(shell sdl2-config --cflags) -DARDUINO=123 -DPC_BUILD
LDFLAGS = -Wl,--gc-sections $(shell sdl2-config --libs) -mconsole

DEPDIR := $(BUILD_DIR)
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.Td
POSTCOMPILE = @mv -f $(DEPDIR)/$*.Td $(DEPDIR)/$*.d && touch $@

INCLUDES := -I$(EMULATOR_ROOT)
INCLUDES += -I$(PROJECT_ROOT)/src
INCLUDES += -I$(PROJECT_ROOT)/.pio/libdeps/esp32doit-devkit-v1/LinkedList

SRC_CXX := $(wildcard $(EMULATOR_ROOT)/*.cpp)
SRC_CXX += $(wildcard $(PROJECT_ROOT)/src/*.cpp)

define add_lib
SRC_C    += $(shell find $1 -name "*.c")
SRC_CXX  += $(shell find $1 -name "*.cpp")
INCLUDES += -I$1
endef

$(foreach lib, $(EMULATOR_LIBS), $(eval $(call add_lib,$(EMULATOR_ROOT)/libs/$(lib))))
$(foreach lib, $(LIBRARIES), $(eval $(call add_lib,$(PROJECT_ROOT)/lib/$(lib))))

# CXXFLAGS += $(INCLUDES)

OBJECTS := $(foreach item, $(SRC_C:$(PROJECT_ROOT)/%.c=%.o),$(BUILD_DIR)/$(item))
OBJECTS += $(foreach item, $(SRC_CXX:$(PROJECT_ROOT)/%.cpp=%.o),$(BUILD_DIR)/$(item))
# $(info OBJECTS="$(OBJECTS)")
SRCS := $(foreach item, $(SRC_C),$(shell echo "$(item)" | sed 's#\:#\\\:#g'))
SRCS += $(foreach item, $(SRC_CXX),$(shell echo "$(item)" | sed 's#\:#\\\:#g'))
# $(info SRCS="$(SRCS)")

# $(info INCLUDES="$(INCLUDES)")

$(EXE_NAME): $(OBJECTS)
	$(CXX) $(INCLUDES) $(OBJECTS) $(LDFLAGS) -o $@
	@size $@.exe

# $(BUILD_DIR)/%.o : $(EMULATOR_ROOT)/%.c $(DEPDIR)/%.d
$(BUILD_DIR)/%.o : $(PROJECT_ROOT)/%.c $(DEPDIR)/%.d
	@mkdir -p `dirname $@`
	$(CC) $(DEPFLAGS) $(CFLAGS) $(INCLUDES) -c $< -o $@
	$(POSTCOMPILE)

# $(BUILD_DIR)/%.o : $(EMULATOR_ROOT)/%.cpp $(DEPDIR)/%.d
$(BUILD_DIR)/%.o : $(PROJECT_ROOT)/%.cpp $(DEPDIR)/%.d
	@mkdir -p `dirname $@`
	$(CXX) $(DEPFLAGS) $(CXXFLAGS) $(INCLUDES) -c $< -o $@
	$(POSTCOMPILE)

$(DEPDIR)/%.d: ;
.PRECIOUS: $(DEPDIR)/%.d

include $(wildcard $(patsubst %,$(DEPDIR)/%.d,$(basename $(SRCS))))

clean:
	rm -r $(BUILD_DIR)
	rm $(EXE_NAME)

print:
	$(info MAKEFILE_LIST="$(MAKEFILE_LIST)")
	$(info EMULATOR_ROOT="$(EMULATOR_ROOT)")
	$(info PROJECT_ROOT="$(PROJECT_ROOT)")
	$(info BUILD_DIR="$(BUILD_DIR)")
	$(info EMULATOR_LIBS="$(EMULATOR_LIBS)")
	$(info LIBRARIES="$(LIBRARIES)")
	$(info SRC_C="$(SRC_C)")
	$(info SRC_CXX="$(SRC_CXX)")
	$(info SRC_CXX_ROOT="$(SRC_CXX_ROOT)")
	$(info INCLUDES="$(INCLUDES)")
	$(info OBJECTS="$(OBJECTS)")
	$(info SRCS="$(SRCS)")
	$(info INCLUDES="$(INCLUDES)")