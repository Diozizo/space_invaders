TARGET_EXEC ?= breakout

BUILD_DIR ?= build
SRC_DIRS ?= src

SRCS := $(shell find $(SRC_DIRS) -name *.c)
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

CPPFLAGS ?= -MMD -MP -g

# Includes
INCLUDE_PATH ?= \
    -I3rdParty/SDL3-3.2.24/include \
    -I3rdParty/SDL3_image-3.2.4/include \
    -I3rdParty/SDL3_gfx-1.0.1/include \
    -I3rdParty/SDL3_ttf/include

# Lib search paths
LIBRATY_PATH ?= \
    -L3rdParty/SDL3-3.2.24/build \
    -L3rdParty/SDL3_image-3.2.4/build \
    -L3rdParty/SDL3_gfx-1.0.1/build \
    -L3rdParty/SDL3_ttf/build

# Linker flags
LDFLAGS ?= -lm -lSDL3 -lSDL3_image -lSDL3_gfx -lSDL3_ttf

$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	@echo Linking: "$@ ( $(foreach f,$^,$(subst $(BUILD_DIR)/$(SRC_DIRS),,$f)) )"
	@$(CC) $(OBJS) -o $@ $(LDFLAGS) $(LIBRATY_PATH) \
	    -Wl,-rpath=3rdParty/SDL3-3.2.24/build \
	    -Wl,-rpath=3rdParty/SDL3_image-3.2.4/build \
	    -Wl,-rpath=3rdParty/SDL3_gfx-1.0.1/build \
	    -Wl,-rpath=3rdParty/SDL3_ttf/build

$(BUILD_DIR)/%.c.o: %.c
	@echo Compiling: $<
	@$(MKDIR_P) $(dir $@)
	@$(CC) $(CPPFLAGS) $(CFLAGS) $(INCLUDE_PATH) -c $< -o $@

.PHONY: clean

clean:
	$(RM) -r $(BUILD_DIR)

-include $(DEPS)

MKDIR_P ?= mkdir -p

