SAMPLES = $(wildcard *-input.txt)
RESULTS = $(SAMPLES:%-input.txt=%-actual.txt)

all: $(RESULTS)

.PHONY: all .FORCE

.FORCE:

$(RESULTS): .FORCE
	@$(EXE) < $(@:%-actual.txt=%-input.txt) > $@
	@diff $(@:%-actual.txt=%-expected.txt) $@
