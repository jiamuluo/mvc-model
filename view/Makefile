
SOURCE_DIR := $(shell find ./ -maxdepth 16 -type d | grep "src")
SOURCE_C = $(foreach dir, $(SOURCE_DIR), $(wildcard $(dir)/*.c))

OBJECES     =$(SOURCE_C:.c=.o)
OBJECES     += main.o

all: $(OBJECES)
	@echo -----------------start build view-----------------------
	@mv $(OBJECES) $(BUILD_DIR)/view
	$(CC) $(CFLAGS) $(LDFLAGS) $(BUILD_DIR)/view/*.o $(BUILD_DIR)/model/*.o -L$(INSTALL_DIR) -l$(TARGET_LIB)  -o $(INSTALL_DIR)/$(TARGET_APP)
	$(STRIP) $(INSTALL_DIR)/$(TARGET_APP)	 
	@echo -----------------end build view-----------------------
$(OBJECES):%.o:%.c   
	$(CC) $(CFLAGS) $(INCLUDE_DIR) -c $< -o $@
clean:
	@echo $(shell pwd)