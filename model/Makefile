SOURCE_DIR := $(shell find ./ -maxdepth 16 -type d | grep "src")
SOURCE_C = $(foreach dir, $(SOURCE_DIR), $(wildcard $(dir)/*.c))

OBJECES     =$(SOURCE_C:.c=.o)
		
all: $(OBJECES)
	@echo "-----------------build model start-----------------------"
	@mv $(OBJECES) $(BUILD_DIR)/model
	@echo "-----------------build model end-----------------------"
$(OBJECES):%.o:%.c   
	$(CC) $(CFLAGS) $(INCLUDE_DIR) -c $< -o $@
clean:
	@echo $(shell pwd)
