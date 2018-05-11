add_executable(gpio-logger
	GPIOLogWriter.cpp
	GPIOLogWriter.h
	GPIOLogger.cpp
)
target_include_directories(gpio-logger PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(gpio-logger rts ${Boost_PROGRAM_OPTIONS_LIBRARY})

add_executable(gpio-somfy-decoder
	SigIntHandler.cpp
	SigIntHandler.h
	GPIOLogReader.cpp
	GPIOLogReader.h
	GPIOSomfyDecoder.cpp
)
target_include_directories(gpio-somfy-decoder PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(gpio-somfy-decoder rts ${Boost_PROGRAM_OPTIONS_LIBRARY})

set(SDR_SOMFY_DECODER_SOURCES
	IQLogReader.cpp
	IQLogReader.h
	SDRSomfyDecoder.cpp
)

if (RTLSDR_FOUND)
	list(APPEND SDR_SOMFY_DECODER_SOURCES
		SigIntHandler.cpp
		SigIntHandler.h
	)
endif()

add_executable(sdr-somfy-decoder ${SDR_SOMFY_DECODER_SOURCES})
target_include_directories(sdr-somfy-decoder PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(sdr-somfy-decoder rts ${Boost_PROGRAM_OPTIONS_LIBRARY})

add_executable(gpio-transmitter
	DurationFileReader.cpp
	DurationFileReader.h
	GPIOTransmitter.cpp
)
target_include_directories(gpio-transmitter PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(gpio-transmitter rts ${Boost_PROGRAM_OPTIONS_LIBRARY})

add_executable(gpio-somfy-transmitter
	SomfyFrameTransmitter.cpp
	SomfyFrameTransmitter.h
	GPIOLogWriter.cpp
	GPIOLogWriter.h
	GPIOSomfyTransmitter.cpp
)
target_include_directories(gpio-somfy-transmitter PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(gpio-somfy-transmitter rts ${Boost_PROGRAM_OPTIONS_LIBRARY})

install(TARGETS
	gpio-logger
	gpio-somfy-decoder
	sdr-somfy-decoder
	gpio-transmitter
	gpio-somfy-transmitter
	RUNTIME DESTINATION bin)
