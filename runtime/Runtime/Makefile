BUILD_DIR = build

CMAKE = cmake
MAKE = make

all: clean_build
	@echo "Running cmake..."
	cd $(BUILD_DIR) && $(CMAKE) ..
	@echo "Building the project..."
	$(MAKE) -C $(BUILD_DIR)
	$(BUILD_DIR)/Runtime

clean_build:
	@echo "Cleaning build directory..."
	rm -rf $(BUILD_DIR)
	mkdir $(BUILD_DIR)

.PHONY: clean_build all

