add_executable(gpio-logger
	Clock.h
	Transition.h
	FastGPIO.cpp
	FastGPIO.h
	ThreadPrio.cpp
	ThreadPrio.h
	RecordingThread.cpp
	RecordingThread.h
	GPIOLogWriter.cpp
	GPIOLogWriter.h
	GPIOLogger.cpp
)
target_include_directories(gpio-logger PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(gpio-logger ${Boost_LIBRARIES})

add_executable(gpio-somfy-decoder
	SigIntHandler.cpp
	SigIntHandler.h
	Clock.h
	Transition.h
	FastGPIO.cpp
	FastGPIO.h
	ThreadPrio.cpp
	ThreadPrio.h
	RecordingThread.cpp
	RecordingThread.h
	GPIOLogReader.cpp
	GPIOLogReader.h
	Duration.h
	DurationTracker.h
	SomfyFrameHeader.cpp
	SomfyFrameHeader.h
	SomfyFrameType.h
	SomfyFrameMatcher.cpp
	SomfyFrameMatcher.h
	ManchesterDecoder.cpp
	ManchesterDecoder.h
	SomfyFrame.cpp
	SomfyFrame.h
	SomfyDecoder.h
	GPIOSomfyDecoder.cpp
)
target_include_directories(gpio-somfy-decoder PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(gpio-somfy-decoder ${Boost_LIBRARIES})

set(SDR_SOMFY_DECODER_SOURCES
	IQLogReader.cpp
	IQLogReader.h
	Filter.h
	Clock.h
	OOKDecoder.h
	Duration.h
	DurationTracker.h
	SomfyFrameHeader.cpp
	SomfyFrameHeader.h
	SomfyFrameType.h
	SomfyFrameMatcher.cpp
	SomfyFrameMatcher.h
	ManchesterDecoder.cpp
	ManchesterDecoder.h
	SomfyFrame.cpp
	SomfyFrame.h
	SomfyDecoder.h
	SDRSomfyDecoder.cpp
)

if (RTLSDR_FOUND)
	list(APPEND SDR_SOMFY_DECODER_SOURCES
		SigIntHandler.cpp
		SigIntHandler.h
		RTLSDRDevice.cpp
		RTLSDRDevice.h
		RTLSDRBufferReader.h
		RTLSDRIQSource.cpp
		RTLSDRIQSource.h
	)
endif()

add_executable(sdr-somfy-decoder ${SDR_SOMFY_DECODER_SOURCES})
target_include_directories(sdr-somfy-decoder PRIVATE ${Boost_INCLUDE_DIRS} ${RTLSDR_INCLUDE_DIRS})
target_link_libraries(sdr-somfy-decoder ${Boost_LIBRARIES} ${RTLSDR_LIBRARIES})

if (RTLSDR_FOUND)
	target_compile_definitions(sdr-somfy-decoder PRIVATE HAVE_RTLSDR)
endif()

add_executable(gpio-transmitter
	Clock.h
	FastGPIO.cpp
	FastGPIO.h
	ThreadPrio.cpp
	ThreadPrio.h
	DurationFileReader.cpp
	DurationFileReader.h
	DurationBuffer.h
	PlaybackThread.cpp
	PlaybackThread.h
	GPIOTransmitter.cpp
)
target_include_directories(gpio-transmitter PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(gpio-transmitter ${Boost_LIBRARIES})

add_executable(gpio-somfy-transmitter
	Clock.h
	FastGPIO.cpp
	FastGPIO.h
	ThreadPrio.cpp
	ThreadPrio.h
	SomfyFrameHeader.cpp
	SomfyFrameHeader.h
	SomfyFrameType.h
	SomfyFrame.cpp
	SomfyFrame.h
	DurationBuffer.h
	ManchesterEncoder.cpp
	ManchesterEncoder.h
	PlaybackThread.cpp
	PlaybackThread.h
	SomfyFrameTransmitter.cpp
	SomfyFrameTransmitter.h
	GPIOLogWriter.cpp
	GPIOLogWriter.h
	GPIOSomfyTransmitter.cpp
)
target_include_directories(gpio-somfy-transmitter PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(gpio-somfy-transmitter ${Boost_LIBRARIES})

install(TARGETS
	gpio-logger
	gpio-somfy-decoder
	sdr-somfy-decoder
	gpio-transmitter
	gpio-somfy-transmitter
	RUNTIME DESTINATION bin)
