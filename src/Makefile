.PHONY: all
all:
	cd emulator && make all
	cd assembler && make all

OUT_DIR := ../out/


.PHONY: cleangit a
clean:
	cd assembler && make clean
	cd emulator && make clean
	$(RM) -r $(OUT_DIR)
