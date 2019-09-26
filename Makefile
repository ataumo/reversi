# Variables
EXE = reversi

# Special rules and targets
.PHONY: all build check clean help

# Rules and targets
all: build

build:
	@cd src && $(MAKE)
	@cp -f src/$(EXE) ./

clean:
	@cd src && $(MAKE) clean
	@cd test && $(MAKE) clean
	@rm -f $(EXE)

help:
	@echo "Usage:"
	@echo " make [all]\t\tBuild"
	@echo " make build\t\tBuild the software"
	@echo " make check\t\tRun all the tests"
	@echo " make clean\t\tRemove all files generated by make"
	@echo " make help\t\tDisplay this help"
