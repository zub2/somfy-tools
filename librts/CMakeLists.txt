find_package(Boost REQUIRED COMPONENTS unit_test_framework)
find_package(PkgConfig REQUIRED)
pkg_check_modules(RTLSDR librtlsdr)

set(RTS_PUBLIC_HEADERS
	include/rts/FrameTransmitterFactory.h
	include/rts/IFrameTransmitter.h
	include/rts/Clock.h
	include/rts/Duration.h
	include/rts/DurationBuffer.h
	include/rts/DurationTracker.h
	include/rts/Transition.h
	include/rts/SomfyFrame.h
	include/rts/SomfyFrameType.h
	include/rts/SomfyDecoder.h
	include/rts/ManchesterDecoder.h
	include/rts/SomfyFrameMatcher.h
	include/rts/backend/rpi-gpio/RecordingThread.h
	include/rts/backend/rpi-gpio/PlaybackThread.h
	include/rts/backend/rpi-gpio/FastGPIO.h
	include/rts/backend/rtlsdr/Filter.h
	include/rts/backend/rtlsdr/OOKDecoder.h
	include/rts/SomfyFrameHeader.h
	include/rts/ManchesterEncoder.h
)

set(RTS_SOURCES
	src/backend/rpi-gpio/FastGPIO.cpp
	src/backend/rpi-gpio/PlaybackThread.cpp
	src/backend/rpi-gpio/RecordingThread.cpp
	src/backend/rpi-gpio/GPIOFrameTransmitter.cpp
	src/backend/rpi-gpio/GPIOFrameTransmitter.h
	src/FrameTransmitterFactory.cpp
	src/ManchesterDecoder.cpp
	src/ManchesterEncoder.cpp
	src/SomfyFrame.cpp
	src/SomfyFrameHeader.cpp
	src/SomfyFrameMatcher.cpp
	src/ThreadPrio.cpp
	src/ThreadPrio.h
)

# only include rtlsdr functionality if librtlsdr is available
if (RTLSDR_FOUND)
	list(APPEND RTS_PUBLIC_HEADERS
		include/rts/backend/rtlsdr/RTLSDRDevice.h
		include/rts/backend/rtlsdr/RTLSDRBufferReader.h
		include/rts/backend/rtlsdr/RTLSDRIQSource.h
	)
	list(APPEND RTS_SOURCES
		src/backend/rtlsdr/RTLSDRDevice.cpp
		src/backend/rtlsdr/RTLSDRIQSource.cpp
	)
endif()

add_library(rts STATIC ${RTS_SOURCES} ${RTS_PUBLIC_HEADERS})
target_include_directories(rts PUBLIC include ${Boost_INCLUDE_DIRS} ${RTLSDR_INCLUDE_DIRS} PRIVATE include/rts)
target_link_libraries(rts ${RTLSDR_LIBRARIES})

if (RTLSDR_FOUND)
	target_compile_definitions(rts PUBLIC HAVE_RTLSDR)
endif()

install(TARGETS rts ARCHIVE DESTINATION lib)
install(FILES ${RTS_PUBLIC_HEADERS} DESTINATION include/rts)

add_subdirectory(test)
