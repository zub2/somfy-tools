set(RTS_PUBLIC_HEADERS
	include/Clock.h
	include/Duration.h
	include/DurationBuffer.h
	include/DurationTracker.h
	include/Transition.h
	include/SomfyFrame.h
	include/SomfyFrameType.h
	include/SomfyDecoder.h
	include/ManchesterDecoder.h
	include/SomfyFrameMatcher.h
	include/backend/rpi-gpio/RecordingThread.h
	include/backend/rpi-gpio/PlaybackThread.h
	include/backend/rpi-gpio/FastGPIO.h
	include/backend/rtlsdr/Filter.h
	include/backend/rtlsdr/OOKDecoder.h
	include/SomfyFrameHeader.h
	include/ManchesterEncoder.h
)

set(RTS_SOURCES
	src/backend/rpi-gpio/FastGPIO.cpp
	src/backend/rpi-gpio/PlaybackThread.cpp
	src/backend/rpi-gpio/RecordingThread.cpp
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
		include/backend/rtlsdr/RTLSDRDevice.h
		include/backend/rtlsdr/RTLSDRBufferReader.h
		include/backend/rtlsdr/RTLSDRIQSource.h
	)
	list(APPEND RTS_SOURCES
		src/backend/rtlsdr/RTLSDRDevice.cpp
		src/backend/rtlsdr/RTLSDRIQSource.cpp
	)
endif()

add_library(rts STATIC ${RTS_SOURCES} ${RTS_PUBLIC_HEADERS})
target_include_directories(rts PUBLIC include ${Boost_INCLUDE_DIRS} ${RTLSDR_INCLUDE_DIRS})
target_link_libraries(rts ${Boost_LIBRARIES} ${RTLSDR_LIBRARIES})

if (RTLSDR_FOUND)
	target_compile_definitions(rts PUBLIC HAVE_RTLSDR)
endif()

install(TARGETS rts ARCHIVE DESTINATION lib)
install(FILES ${RTS_PUBLIC_HEADERS} DESTINATION include/rts)
