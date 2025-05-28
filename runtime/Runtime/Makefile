BUILD_DIR = build

CMAKERELEASE = cmake
CMAKEDEBUG = cmake -DCMAKE_BUILD_TYPE=Debug
MAKE = make

debug: clean_build
	@echo "Debug build:"
	@echo "Running cmake..."
	cd $(BUILD_DIR) && $(CMAKEDEBUG) ..
	@echo "Building the project..."
	$(MAKE) -C $(BUILD_DIR)
	$(BUILD_DIR)/Runtime


release: clean_build
	@echo "Release build:"
	@echo "Running cmake..."
	cd $(BUILD_DIR) && $(CMAKERELEASE) ..
	@echo "Building the project..."
	$(MAKE) -C $(BUILD_DIR)
	$(BUILD_DIR)/Runtime

lol:
	nasm -felf64 -g -F dwarf main.nasm -o hi.o
	ld hi.o
	./a.out



clean_build:
	@echo "Cleaning build directory..."
	rm -rf $(BUILD_DIR)
	mkdir $(BUILD_DIR)
	rm -rf screens
	mkdir screens

.PHONY: clean_build release debug
