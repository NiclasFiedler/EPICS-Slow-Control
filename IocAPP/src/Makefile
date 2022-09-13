TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS BELOW HERE

# Use typed rset structure (see 3.16.1 release notes)
USR_CPPFLAGS += -DUSE_TYPED_RSET
USR_CXXFLAGS += -std=c++17 -lpigpio -lrt -lthread -I/usr/include/python3.9
PROD_SYS_LIBS += pigpio
PROD_SYS_LIBS += python3.9

# xxxRecord.h will be created from xxxRecord.dbd
DBDINC += xxxRecord

# Install xxxSupport.dbd into <top>/dbd
DBD += xxxSupport.dbd

# Build an IOC support library
LIBRARY_IOC += IocSupport

# Compile and add code to the support library
IocSupport_SRCS += xxxRecord.c
IocSupport_SRCS += devXxxSoft.c

# Link locally-provided code into the support library,
# rather than directly into the IOC application, that
# causes problems on Windows DLL builds
IocSupport_SRCS += devIocVersion.c
IocSupport_SRCS += IocHello.c
IocSupport_SRCS += initTrace.c
IocSupport_SRCS += dbSubExample.c
IocSupport_LIBS += $(EPICS_BASE_IOC_LIBS)

# Auto-generate a header file containing a version string.
# Version comes from the VCS if available, else date+time.
GENVERSION = IocVersion.h
# Macro name
GENVERSIONMACRO = IocVERSION

# Build the IOC application
PROD_IOC = Ioc

# Ioc.dbd will be created and installed
DBD += Ioc.dbd

# Ioc.dbd will include these files:
Ioc_DBD += base.dbd
Ioc_DBD += xxxSupport.dbd
Ioc_DBD += devIocVersion.dbd
Ioc_DBD += IocHello.dbd
Ioc_DBD += initTrace.dbd
Ioc_DBD += dbSubExample.dbd
Ioc_DBD += saa.dbd

# Ioc_registerRecordDeviceDriver.cpp derives from Ioc.dbd
Ioc_SRCS += Ioc_registerRecordDeviceDriver.cpp
Ioc_SRCS += epics_saa.cpp

Ioc_SRCS += hvboard.cpp
Ioc_SRCS += spi.cpp
Ioc_SRCS += serialadapter_aux_spi.cpp
Ioc_SRCS += sht21.cpp
Ioc_SRCS += sht31.cpp
Ioc_SRCS += lm75.cpp
Ioc_SRCS += eeprom.cpp
Ioc_SRCS += pca9554.cpp
Ioc_SRCS += ads1115.cpp
Ioc_SRCS += apfel.cpp
Ioc_SRCS += saa_i2c.cpp

# Build the main IOC entry point where needed
Ioc_SRCS_DEFAULT += IocMain.cpp
Ioc_SRCS_vxWorks += -nil-

# Link in the code from our support library
Ioc_LIBS += IocSupport

# To build SNL programs, SNCSEQ must be defined
# in the <top>/configure/RELEASE file
ifneq ($(SNCSEQ),)
    # Build sncExample into IocSupport
    sncExample_SNCFLAGS += +r
    Ioc_DBD += sncExample.dbd
    # A .stt sequence program is *not* pre-processed:
    IocSupport_SRCS += sncExample.stt
    IocSupport_LIBS += seq pv
    Ioc_LIBS += seq pv

    # Build sncProgram as a standalone program
    PROD_HOST += sncProgram
    sncProgram_SNCFLAGS += +m
    # A .st sequence program *is* pre-processed:
    sncProgram_SRCS += sncProgram.st
    sncProgram_LIBS += seq pv
    sncProgram_LIBS += $(EPICS_BASE_HOST_LIBS)
endif

# Link QSRV (pvAccess Server) if available
ifdef EPICS_QSRV_MAJOR_VERSION
    Ioc_LIBS += qsrv
    Ioc_LIBS += $(EPICS_BASE_PVA_CORE_LIBS)
    Ioc_DBD += PVAServerRegister.dbd
    Ioc_DBD += qsrv.dbd
endif

# Finally link IOC to the EPICS Base libraries
Ioc_LIBS += $(EPICS_BASE_IOC_LIBS)

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD EXTRA GNUMAKE RULES BELOW HERE

# Explicit dependency needed for generated header file
devIocVersion$(DEP): $(COMMON_DIR)/$(GENVERSION)
