add_executable(tests
	../include/SomfyFrameType.h
	../include/SomfyFrameHeader.h
	../include/SomfyFrame.h
	../include/SomfyFrameMatcher.h
	../include/ManchesterDecoder.h
	../include/ManchesterEncoder.h
	../include/DurationTracker.h
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
target_include_directories(tests PRIVATE ${Boost_INCLUDE_DIRS} ../src ../include)
target_link_libraries(tests ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

add_test(NAME TestAll COMMAND tests)
