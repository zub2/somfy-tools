add_executable(tests
	../src/SomfyFrameType.h
	../src/SomfyFrameHeader.cpp
	../src/SomfyFrameHeader.h
	../src/SomfyFrame.cpp
	../src/SomfyFrame.h
	../src/SomfyFrameMatcher.cpp
	../src/SomfyFrameMatcher.h
	../src/ManchesterDecoder.cpp
	../src/ManchesterDecoder.h
	../src/ManchesterEncoder.cpp
	../src/ManchesterEncoder.h
	../src/DurationTracker.h
	TestMain.cpp
	TestUtils.h
	TestSomfyFrame.cpp
	TestSomfyFrameMatcher.cpp
	TestDurationTracker.cpp
	TestManchester.cpp
)
target_include_directories(tests PRIVATE ${Boost_INCLUDE_DIRS} ../src)
target_link_libraries(tests ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

add_test(NAME TestAll COMMAND tests)
