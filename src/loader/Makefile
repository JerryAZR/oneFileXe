DEFINES		= -DUNICODE -D_UNICODE -DWIN32 -DMINGW_HAS_SECURE_API=1 -DQT_NO_DEBUG -DQT_CORE_LIB
QT			= Qt
CXX			= g++
CXXFLAGS	= -fno-keep-inline-dllexport -O2 -std=gnu++1z -Wall -Wextra -Wextra -fexceptions -mthreads $(DEFINES)
LINKER		= g++
LFLAGS		= -Wl,-s -Wl,-subsystem,console -static -mthreads
LIBS		= ${QT}\lib\libQt6Core.a -lsynchronization -lmpr -luserenv -ladvapi32 -lauthz -lkernel32 -lnetapi32 -lole32 -lshell32 -luser32 -luuid -lversion -lwinmm -lws2_32 ${QT}\lib\libQt6BundledPcre2.a   
INCPATH		= -I${QT}/include -I${QT}/include/QtCore -I${QT}/mkspecs/win32-g++ 
OBJECTS		= qrc_data.o main.o
TARGET		= app.exe

all: ${TARGET}

qrc_data.cpp: data.qrc
	${QT}\bin\rcc data.qrc -o qrc_data.cpp

%.o: %.cpp
	${CXX} -c $(CXXFLAGS) $(INCPATH) -o $@ $<

${TARGET}: $(OBJECTS)
	$(LINKER) $(LFLAGS) -o $@ $(OBJECTS) $(LIBS)

.PHONY: clean
clean:
	del $(OBJECTS) qrc_data.cpp ${TARGET}

