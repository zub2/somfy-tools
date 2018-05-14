boost_tests = dependency('boost', modules: ['unit_test_framework'])

executable('tests', [
	'../include/rts/SomfyFrameType.h',
	'../include/rts/SomfyFrameHeader.h',
	'../include/rts/SomfyFrame.h',
	'../include/rts/SomfyFrameMatcher.h',
	'../include/rts/ManchesterDecoder.h',
	'../include/rts/ManchesterEncoder.h',
	'../include/rts/DurationTracker.h',
	'../src/SomfyFrameHeader.cpp',
	'../src/SomfyFrame.cpp',
	'../src/SomfyFrameMatcher.cpp',
	'../src/ManchesterDecoder.cpp',
	'../src/ManchesterEncoder.cpp',
	'TestMain.cpp',
	'TestUtils.h',
	'TestSomfyFrame.cpp',
	'TestSomfyFrameMatcher.cpp',
	'TestDurationTracker.cpp',
	'TestManchester.cpp'
], include_directories: include_directories('../include/rts'), dependencies: [boost_tests])
