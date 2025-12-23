INIT_PATH = $(PWD)
TARGET_EXEC ?= breakout

BUILD_DIR ?= build
SRC_DIRS ?= src

SRCS := $(shell find $(SRC_DIRS) -name *.c)
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

CPPFLAGS ?= -MMD -MP -g

# ---------------- Includes ----------------
# Updated to point to '3rdParty/SDL3'
INCLUDE_PATH ?= \
    -I3rdParty/SDL3/include \
    -I3rdParty/SDL3_image-3.2.4/include \
    -I3rdParty/SDL3_gfx-1.0.1/include \
    -I3rdParty/SDL3_ttf/include \
    -I3rdParty/SDL3_mixer/include


# ---------------- Library paths ----------------
# Updated to point to '3rdParty/SDL3'
LIBRATY_PATH ?= \
    -L3rdParty/SDL3/build \
    -L3rdParty/SDL3_image-3.2.4/build \
    -L3rdParty/SDL3_gfx-1.0.1/build \
    -L3rdParty/SDL3_ttf/build \
    -L3rdParty/SDL3_mixer/build


# ---------------- Linker flags ----------------
LDFLAGS ?= \
    -lm \
    -lSDL3 \
    -lSDL3_image \
    -lSDL3_gfx \
    -lSDL3_ttf \
    -lSDL3_mixer


$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	@echo Linking: "$@ ( $(foreach f,$^,$(subst $(BUILD_DIR)/$(SRC_DIRS),,$f)) )"
	@mkdir -p $(dir $@)
	@$(CC) $(OBJS) -o $@ $(LDFLAGS) $(LIBRATY_PATH) \
        -Wl,-rpath=3rdParty/SDL3/build \
        -Wl,-rpath=3rdParty/SDL3_image-3.2.4/build \
        -Wl,-rpath=3rdParty/SDL3_gfx-1.0.1/build \
        -Wl,-rpath=3rdParty/SDL3_ttf/build \
        -Wl,-rpath=3rdParty/SDL3_mixer/build


$(BUILD_DIR)/%.c.o: %.c
	@echo Compiling: $<
	@mkdir -p $(dir $@)
	@$(CC) $(CPPFLAGS) $(CFLAGS) $(INCLUDE_PATH) -c $< -o $@


.PHONY: clean

clean:
	$(RM) -r $(BUILD_DIR)

-include $(DEPS)
