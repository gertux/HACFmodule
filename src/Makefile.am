lib_LTLIBRARIES = libmodhacf.la
libmodhacf_la_SOURCES = hacf_apache_module.c hacf.c hacf_param.c hacf.h hacf_param.h
install-libLTLIBRARIES: $(lib_LTLIBRARIES)
	$(APXS) -i -a -n hacf $(lib_LTLIBRARIES)
uninstall-libLTLIBRARIES:
	@echo "The module should be manualy uninstalled from `$(APXS) -q LIBEXECDIR`"
