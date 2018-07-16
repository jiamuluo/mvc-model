
include Makefile.mk

BUILD_DIR   :=$(CURDIR)/build_dir
INSTALL_DIR :=$(CURDIR)/_install

SOURCE_DIR	:= 	$(CURDIR)/driver/\
				$(CURDIR)/model/ \
				$(CURDIR)/view/

INCLUDE_PATH = $(shell find $(CURDIR)  -maxdepth 16 -type d | grep "include")
INCLUDE_DIR  += $(foreach dir, $(INCLUDE_PATH), -I$(dir))

export BUILD_DIR INCLUDE_DIR INSTALL_DIR CC AR LD STRIP CFLAGS LDFLAGS TARGET_LIB TARGET_APP

.PHONY : all clean install
all:
	@echo ---------------------------start build----------------------
	@rm -rf $(BUILD_DIR)
	@rm -rf $(INSTALL_DIR)
	@mkdir -p $(BUILD_DIR)/driver
	@mkdir -p $(BUILD_DIR)/model
	@mkdir -p $(BUILD_DIR)/view
	@mkdir -p $(INSTALL_DIR)

	@for dir in $(SOURCE_DIR);\
	do $(MAKE) -C $$dir all;\
	done	
	@echo ---------------------------end build-------------------------

clean:
	@-rm -rf $(BUILD_DIR)

install:
	@chmod +x $(INSTALL_DIR)/$(TARGET_APP)
	cp -f $(INSTALL_DIR)/$(TARGET_APP) /bin
	cp -f $(INSTALL_DIR)/lib$(TARGET_LIB)* /lib/
 