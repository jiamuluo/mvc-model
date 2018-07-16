
SOURCE_DIR	:= 	$(CURDIR)/sys/src \
				$(CURDIR)/user/src 

SOURCE_C = $(foreach dir, $(SOURCE_DIR), $(wildcard $(dir)/*.c))
OBJECES  = $(SOURCE_C:.c=.o)
		
all: $(OBJECES)
	@echo "-----------------driver lib start-----------------------"
	@mv $(OBJECES) $(BUILD_DIR)/driver
	$(LD) -shared -o $(INSTALL_DIR)/lib$(TARGET_LIB).so $(BUILD_DIR)/driver/*.o  $(LDFLAGS)
	$(STRIP) $(INSTALL_DIR)/libmuos.so
	@echo "-----------------driver lib end-----------------------"
$(OBJECES):%.o:%.c   
	$(CC) $(CFLAGS) -fPIC $(INCLUDE_DIR) -c $< -o $@
clean:
	@echo $(shell pwd)
