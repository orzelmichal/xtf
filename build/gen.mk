# Architecture independent makefile for compiling tests

# Sanity checking of expected parameters
ifeq ($(NAME),)
$(error NAME should be specified)
endif

ifeq ($(TEST-ENVS),)
$(error TEST-ENVS should not be empty)
endif

# It might be that some tests are intended only for some specific architectures.
# If the architecture we are building the test for is not listed in TEST-ARCH,
# skip the test. If TEST-ARCH is not specified, it defaults to all the
# supported architectures.
TEST-ARCH ?= $(SUPPORTED_ARCH)
ifneq ($(filter-out $(TEST-ARCH),$(ARCH)),)
$(info Skipping test not supported by ARCH=$(ARCH))
SKIP_TEST := 1
endif

# It might be that TEST-ENVS contains environments for different architectures.
# In that case skip unrecognised environments and override TEST-ENVS to contain
# only the recognized ones.
ifneq ($(filter-out $(ALL_ENVIRONMENTS),$(TEST-ENVS)),)
$(info Skipping unrecognised environments '$(filter-out $(ALL_ENVIRONMENTS),$(TEST-ENVS))')
TEST-ENVS := $(filter-out $(filter-out $(ALL_ENVIRONMENTS),$(TEST-ENVS)),$(TEST-ENVS))
# At this point, skip test if TEST-ENVS is empty.
ifeq ($(TEST-ENVS),)
SKIP_TEST := 1
endif
endif

ifeq ($(CATEGORY),)
$(error CATEGORY should not be empty)
endif

ifneq ($(filter-out $(ALL_CATEGORIES),$(CATEGORY)),)
$(error Unrecognised category '$(filter-out $(ALL_CATEGORIES),$(CATEGORY))')
endif

ifeq ($(VCPUS),)
VCPUS := 1 # Default to 1 vcpu if not provided
endif

ifneq ($(VARY-CFG),)
TEST-CFGS := $(foreach env,$(TEST-ENVS),$(foreach vary,$(VARY-CFG),test-$(env)-$(NAME)~$(vary).cfg))
else
TEST-CFGS := $(foreach env,$(TEST-ENVS),test-$(env)-$(NAME).cfg)
endif

ifeq ($(SKIP_TEST),)
.PHONY: build
build: $(foreach env,$(TEST-ENVS),test-$(env)-$(NAME)) $(TEST-CFGS)
build: info.json

info.json: $(ROOT)/build/mkinfo.py FORCE
	@$(PYTHON) $< $@.tmp "$(NAME)" "$(CATEGORY)" "$(TEST-ENVS)" "$(VARY-CFG)"
	@$(call move-if-changed,$@.tmp,$@)

.PHONY: install install-each-env
install: install-each-env info.json
	@$(INSTALL_DIR) $(DESTDIR)$(xtftestdir)/$(NAME)
	$(INSTALL_DATA) info.json $(DESTDIR)$(xtftestdir)/$(NAME)
else
.PHONY: build install
build:
install:
endif

# Build a test for specified environment
define PERENV_build

# If any base architecture/environment needs a special compilation/linking
# recipe instead of the default one, a custom recipe called build-$(BASE_ARCH)
# or build-$(env) e.g. build-arm or build-hvm64 should be created in
# $(ROOT)/build/$(ARCH)/arch-common.mk

test-$(1)-$(NAME): $$(DEPS-$(1)) $$(link-$(1))
ifdef build-$(BASE_ARCH)
	@# Base-architecture specific compilation recipe
	$(call build-$(BASE_ARCH),$(1))
else ifdef build-$(1)
	@# Environment specific compilation recipe
	$(call build-$(1))
else
	@# Generic link line for most environments
	$(LD) $$(LDFLAGS_$(1)) $$(DEPS-$(1)) -o $$@
endif

cfg-$(1) ?= $(defcfg-$($(1)_guest))

cfg-default-deps := $(ROOT)/build/mkcfg.py $$(cfg-$(1)) $(TEST-EXTRA-CFG) FORCE

test-$(1)-$(NAME).cfg: $$(cfg-default-deps)
	$(PYTHON) $$< $$@.tmp "$$(cfg-$(1))" "$(VCPUS)" "$(TEST-EXTRA-CFG)" ""
	@$(call move-if-changed,$$@.tmp,$$@)

test-$(1)-$(NAME)~%.cfg: $$(cfg-default-deps) %.cfg.in
	$(PYTHON) $$< $$@.tmp "$$(cfg-$(1))" "$(VCPUS)" "$(TEST-EXTRA-CFG)" "$$*.cfg.in"
	@$(call move-if-changed,$$@.tmp,$$@)

test-$(1)-$(NAME)~%.cfg: $$(cfg-default-deps) $(ROOT)/config/%.cfg.in
	$(PYTHON) $$< $$@.tmp "$$(cfg-$(1))" "$(VCPUS)" "$(TEST-EXTRA-CFG)" "$(ROOT)/config/$$*.cfg.in"
	@$(call move-if-changed,$$@.tmp,$$@)

-include $$(link-$(1):%.lds=%.d)
-include $$(DEPS-$(1):%.o=%.d)

.PHONY: install-$(1) install-$(1).cfg
install-$(1): test-$(1)-$(NAME)
	@$(INSTALL_DIR) $(DESTDIR)$(xtftestdir)/$(NAME)
	$(INSTALL_PROGRAM) $$< $(DESTDIR)$(xtftestdir)/$(NAME)

install-$(1).cfg: $(filter test-$(1)-%,$(TEST-CFGS))
	@$(INSTALL_DIR) $(DESTDIR)$(xtftestdir)/$(NAME)
	$(INSTALL_DATA) $$^ $(DESTDIR)$(xtftestdir)/$(NAME)

install-each-env: install-$(1) install-$(1).cfg

endef

# Make a call to a function PERENV_build once per each test's environment
$(foreach env,$(TEST-ENVS),$(eval $(call PERENV_build,$(env))))

.PHONY: clean
clean:
	find $(ROOT) \( -name "*.o" -o -name "*.d" \) -delete
	rm -f $(foreach env,$(TEST-ENVS),test-$(env)-$(NAME) test-$(env)-$(NAME)*.cfg)

.PHONY: %var
%var:
	@echo "$* = $($*)"

.PHONY: FORCE
FORCE:
