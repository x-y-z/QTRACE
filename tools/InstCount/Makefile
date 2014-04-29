include ../../config-host.mak

# INSTRUMENT_INCLUDE += $(GLIB_CFLAGS) $(QEMU_INCLUDES) -I/home/xtong/qemu-trace/x86_64-softmmu -I/home/xtong/qemu-trace/target-i386
CXX_X86 = g++
CXXFLAGS += -O3 -g -shared -fPIC  -I../../

INSTRUMENT_MODULES= icount.so 
all:    $(INSTRUMENT_MODULES)

%.so:   %.cc
	$(CXX_X86) $(CXXFLAGS) -o $@ $^
clean:  
	rm -rf *.so
