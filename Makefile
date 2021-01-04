# Set project directory one level above of Makefile directory.
SOURCEDIR := src
BUILDDIR := build

# Name of the final executable file and of the used cpmîler
ifeq ($(OS),win)
	TARGET = rail_network_database.exe
	COMPILER = x86_64-w64-mingw32-g++-win32
else
	TARGET = rail_network_database
	COMPILER = g++
endif

ifeq ($(DEBUG),TRUE)
	DEBUG = -g
else
	DEBUG = -g
endif

# Create the list of directories
DIRS = .
SOURCEDIRS = $(foreach dir, $(DIRS), $(addprefix $(SOURCEDIR)/, $(dir)))
TARGETDIRS = $(foreach dir, $(DIRS), $(addprefix $(BUILDDIR)/, $(dir)))

# Generate the GCC includes parameters by adding -I before each source folder
INCLUDES = $(foreach dir, $(SOURCEDIRS), $(addprefix -I, $(dir)))

# Add this list to VPATH, the place make will look for the source files
VPATH = $(SOURCEDIRS)

# Create a list of *.c sources in DIRS
SOURCES = $(foreach dir,$(SOURCEDIRS),$(wildcard $(dir)/*.c++))

# Define objects for all sources
OBJS := $(subst $(SOURCEDIR),$(BUILDDIR),$(SOURCES:.c++=.o))

# Define the libraries
LIBRARIES := -lmysqlcppconn

# Define dependencies files for all objects
DEPS = $(OBJS:.o=.d)

RM = rm -rf 
RMDIR = rm -rf 
MKDIR = mkdir -p
ERRIGNORE = 2>/dev/null
SEP=/

# Remove space after separator
PSEP = $(strip $(SEP))

#Hide or not the calls depending of VERBOSE
ifeq ($(VERBOSE),TRUE)
	HIDE =  
else
	HIDE = @
endif

# Define the function that will generate each rule
define generateRules
$(1)/%.o: %.c++
	@echo Building $$@
	$(HIDE)$(COMPILER) $(DEBUG) -c $$(INCLUDES) -std=c++11 $(LIBRARIES) -o  $$(subst /,$$(PSEP),$$@) $$(subst /,$$(PSEP),$$<) -MMD
endef

.PHONY: all clean directories 

all: directories $(TARGET)

$(TARGET): $(OBJS)
	$(HIDE)echo Linking $@
	$(HIDE)$(COMPILER) $(OBJS) -std=c++11 $(LIBRARIES) -o  $(TARGET)

# Include dependencies
-include $(DEPS)

# Generate rules
$(foreach targetdir, $(TARGETDIRS), $(eval $(call generateRules, $(targetdir))))

directories: 
	$(HIDE)$(MKDIR) $(subst /,$(PSEP),$(TARGETDIRS)) $(ERRIGNORE)

# Remove all objects, dependencies and executable files generated during the build
clean:
	$(HIDE)$(RMDIR) $(subst /,$(PSEP),$(TARGETDIRS)) $(ERRIGNORE)
	$(HIDE)$(RM) $(TARGET) $(ERRIGNORE)
	@echo Cleaning done !