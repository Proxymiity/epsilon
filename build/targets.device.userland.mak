HANDY_TARGETS += $(userland_target_variants)

userland_src += $(ion_device_userland_src) $(liba_src) $(kandinsky_src) $(escher_src) $(libaxx_src) $(poincare_src) $(python_src) $(apps_src)

userland_test_src += $(ion_device_userland_src) $(liba_src) $(kandinsky_src) $(escher_src) $(libaxx_src) $(poincare_src) $(python_src) $(apps_tests_src) $(tests_src) $(runner_src)

userland_targets = $(addprefix $(BUILD_DIR)/,$(addsuffix .$(EXE),$(userland_target_variants)))

USERLAND_LDFLAGS += -Lion/src/$(PLATFORM)/$(MODEL)/userland -Lion/src/$(PLATFORM)/userland
USERLAND_LDDEPS += ion/src/$(PLATFORM)/userland/shared_userland_flash.ld

define rule_for_flavored_userland
$(1): $$(call flavored_object_for, \
	$(if $(findstring test,$(1)),$$(userland_test_src),$$(userland_src)), \
	leaveuserland consoledisplay $(patsubst $(BUILD_DIR)/userland%.$(EXE), $(subst test,,$(subst ., ,%)),$(1)))
endef

$(foreach target,$(userland_targets),$(eval $(call rule_for_flavored_userland,$(target))))

$(userland_targets): LDFLAGS += $(USERLAND_LDFLAGS)

