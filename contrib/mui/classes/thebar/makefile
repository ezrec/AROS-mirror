
.PHONY: all
all: button mcc mcp

.PHONY: button
button:
	@echo "Generating TheButton.mcc..."
	@cd mcc/button && make --no-print-directory 
	@echo -e "Done.\n"

.PHONY: mcc
mcc:
	@ echo "Generating TheBar.mcc and TheBarVirt.mcc..."
	@cd mcc && make --no-print-directory 
	@echo -e "Done.\n"

.PHONY: mcp
mcp:
	@ echo "Generating TheBar.mcp..."
	@cd mcp && make --no-print-directory 
	@echo -e "Done.\n"

.PHONY: clean
clean:
	@echo "Cleaning..."
	@cd mcc/button && make --no-print-directory clean 
	@cd mcc && make --no-print-directory clean 
	@cd mcp && make --no-print-directory clean 
	@echo -e "Done.\n"

.PHONY: cleanall
cleanall:
	@echo "Cleaning all..."
	@cd mcc/button && make --no-print-directory cleanall
	@cd mcc && make --no-print-directory cleanall
	@cd mcp && make --no-print-directory cleanall
	@echo -e "Done.\n"

.PHONY: install
install:
	@echo "Installing all..."
	@cd mcc/button && make --no-print-directory install 
	@cd mcc && make --no-print-directory install
	@cd mcp && make --no-print-directory install
	@avail flush
	@echo -e "Done.\n"

