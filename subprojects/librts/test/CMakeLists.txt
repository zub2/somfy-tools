add_executable(tests
	../include/rts/SomfyFrameType.h
	../include/rts/SomfyFrameHeader.h
	../include/rts/SomfyFrame.h
	../include/rts/SomfyFrameMatcher.h
	../include/rts/ManchesterDecoder.h
	../include/rts/ManchesterEncoder.h
	../include/rts/DurationTracker.h
	../src/SomfyFrameHeader.cpp
	../src/SomfyFrame.cpp
	../src/SomfyFrameMatcher.cpp
	../src/ManchesterDecoder.cpp
	../src/ManchesterEncoder.cpp
	TestMain.cpp
	TestUtils.h
	TestSomfyFrame.cpp
	TestSomfyFrameMatcher.cpp
	TestDurationTracker.cpp
	TestManchester.cpp
)
target_include_directories(tests PRIVATE ${Boost_INCLUDE_DIRS} ../src ../include/rts)
target_link_libraries(tests ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

add_test(NAME TestAll COMMAND tests)
