.PHONY: all
all: button mcc mcp

.PHONY: button
button:
	@echo "Generating TheButton.mcc..."
	@$(MAKE) -C mcc/button --no-print-directory
	@echo "Done."

.PHONY: mcc
mcc:
	@ echo "Generating TheBar.mcc and TheBarVirt.mcc..."
	@$(MAKE) -C mcc --no-print-directory
	@echo "Done."

.PHONY: mcp
mcp:
	@ echo "Generating TheBar.mcp..."
	@$(MAKE) -C mcp --no-print-directory
	@echo "Done."

.PHONY: clean
clean:
	@echo "Cleaning..."
	@$(MAKE) -C mcc/button --no-print-directory clean
	@$(MAKE) -C mcc --no-print-directory clean
	@$(MAKE) -C mcp --no-print-directory clean
	@echo "Done."

.PHONY: cleanall
cleanall:
	@echo "Cleaning all..."
	@$(MAKE) -C mcc/button --no-print-directory cleanall
	@$(MAKE) -C mcc --no-print-directory cleanall
	@$(MAKE) -C mcp --no-print-directory cleanall
	@echo "Done."

.PHONY: install
install:
	@echo "Installing all..."
	@$(MAKE) -C mcc/button --no-print-directory install
	@$(MAKE) -C mcc --no-print-directory install
	@$(MAKE) -C mcp --no-print-directory install
	@avail flush
	@echo "Done."

