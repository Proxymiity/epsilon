PNG_SERIALIZER := ion/image/png_serializer.py

define requires_compressed_png
$(call object_for,$(1)): $(call object_for,$(2).png)
$(call object_for,$(1)): SFLAGS += $(foreach d,$(sort $(dir $(call object_for,$(1)))),-I$(d))
$(3) += $(2).png

.PRECIOUS: $(BUILD_DIR)/$(2).h $(BUILD_DIR)/$(2).cpp
$(eval $(call rule_for, \
  PNG_SERIALIZER, \
  $(2).h $(2).cpp, \
  $(2).png $(PNG_SERIALIZER), \
  python3 $(PNG_SERIALIZER) --png $$< --header $$(basename $$@).h --cimplementation $$(basename $$@).cpp, \
  global \
))

endef
