ifndef ARCH
HANDY_TARGETS_EXTENSIONS += apk
endif

.PHONY: %_run
%_run: $(BUILD_DIR)/%.apk
	$(call rule_label,ADB)
	$(Q) adb install $<
