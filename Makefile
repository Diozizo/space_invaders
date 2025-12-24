INIT_PATH = $(PWD)
TARGET_EXEC ?= breakout

BUILD_DIR ?= build
SRC_DIRS ?= src

SRCS := $(shell find $(SRC_DIRS) -name *.c)
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

CPPFLAGS ?= -MMD -MP -g

# ---------------- Includes ----------------
INCLUDE_PATH ?= \
    -I3rdParty/SDL3/include \
    -I3rdParty/SDL3_image-3.2.4/include \
    -I3rdParty/SDL3_gfx-1.0.1/include \
    -I3rdParty/SDL3_ttf/include \
    -I3rdParty/SDL3_mixer/include \
    -I3rdParty/ncurses/build/include \
    -I3rdParty/ncurses/build/include/ncursesw

# ---------------- Library paths ----------------
LIBRATY_PATH ?= \
    -L3rdParty/SDL3/build \
    -L3rdParty/SDL3_image-3.2.4/build \
    -L3rdParty/SDL3_gfx-1.0.1/build \
    -L3rdParty/SDL3_ttf/build \
    -L3rdParty/SDL3_mixer/build \
    -L3rdParty/ncurses/build/lib

# ---------------- Linker flags ----------------
# Added -lncursesw (Wide-char version is standard)
LDFLAGS ?= \
    -lm \
    -lSDL3 \
    -lSDL3_image \
    -lSDL3_gfx \
    -lSDL3_ttf \
    -lSDL3_mixer \
    -lncursesw

# ---------------- Build Target ----------------
$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	@echo Linking: "$@ ( $(foreach f,$^,$(subst $(BUILD_DIR)/$(SRC_DIRS),,$f)) )"
	@mkdir -p $(dir $@)
	@$(CC) $(OBJS) -o $@ $(LDFLAGS) $(LIBRATY_PATH) \
        -Wl,-rpath=3rdParty/SDL3/build \
        -Wl,-rpath=3rdParty/SDL3_image-3.2.4/build \
        -Wl,-rpath=3rdParty/SDL3_gfx-1.0.1/build \
        -Wl,-rpath=3rdParty/SDL3_ttf/build \
        -Wl,-rpath=3rdParty/SDL3_mixer/build \
        -Wl,-rpath=3rdParty/ncurses/build/lib

$(BUILD_DIR)/%.c.o: %.c
	@echo Compiling: $<
	@mkdir -p $(dir $@)
	@$(CC) $(CPPFLAGS) $(CFLAGS) $(INCLUDE_PATH) -c $< -o $@

.PHONY: clean

clean:
	$(RM) -r $(BUILD_DIR)

-include $(DEPS)
