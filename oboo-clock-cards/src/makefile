SUBDIRS = oboo-runtime

all:	subdirs

.PHONY: subdirs $(SUBDIRS)

subdirs: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@

env:
	mkdir -p $(AUTO_DIR)/bin
	mkdir -p $(AUTO_DIR)/inc
	mkdir -p $(AUTO_DIR)/out

clean:
	rm -rf $(AUTO_DIR)