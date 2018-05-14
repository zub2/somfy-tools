project('librts', 'cpp',
        default_options: ['cpp_std=c++17'],
        license: 'GPLv3'
)

threads = dependency('threads')
boost = dependency('boost')

rtlsdr = dependency('librtlsdr', required: false)

rts_public_headers = [
	'include/rts/FrameTransmitterFactory.h',
	'include/rts/IFrameTransmitter.h',
	'include/rts/Clock.h',
	'include/rts/Duration.h',
	'include/rts/DurationBuffer.h',
	'include/rts/DurationTracker.h',
	'include/rts/Transition.h',
	'include/rts/SomfyFrame.h',
	'include/rts/SomfyFrameType.h',
	'include/rts/SomfyDecoder.h',
	'include/rts/ManchesterDecoder.h',
	'include/rts/SomfyFrameMatcher.h',
	'include/rts/backend/rpi-gpio/RecordingThread.h',
	'include/rts/backend/rpi-gpio/PlaybackThread.h',
	'include/rts/backend/rpi-gpio/FastGPIO.h',
	'include/rts/backend/rtlsdr/Filter.h',
	'include/rts/backend/rtlsdr/OOKDecoder.h',
	'include/rts/SomfyFrameHeader.h',
	'include/rts/ManchesterEncoder.h'
]

rts_sources = [
	'src/backend/rpi-gpio/FastGPIO.cpp',
	'src/backend/rpi-gpio/PlaybackThread.cpp',
	'src/backend/rpi-gpio/RecordingThread.cpp',
	'src/backend/rpi-gpio/GPIOFrameTransmitter.cpp',
	'src/backend/rpi-gpio/GPIOFrameTransmitter.h',
	'src/FrameTransmitterFactory.cpp',
	'src/ManchesterDecoder.cpp',
	'src/ManchesterEncoder.cpp',
	'src/SomfyFrame.cpp',
	'src/SomfyFrameHeader.cpp',
	'src/SomfyFrameMatcher.cpp',
	'src/ThreadPrio.cpp',
	'src/ThreadPrio.h'
]

# only include rtlsdr functionality if librtlsdr is available
have_rtlsdr = rtlsdr.found()
if have_rtlsdr
	rts_public_headers += [
		'include/rts/backend/rtlsdr/RTLSDRDevice.h',
		'include/rts/backend/rtlsdr/RTLSDRBufferReader.h',
		'include/rts/backend/rtlsdr/RTLSDRIQSource.h'
	]
	rts_sources += [
		'src/backend/rtlsdr/RTLSDRDevice.cpp',
		'src/backend/rtlsdr/RTLSDRIQSource.cpp'
	]
endif

librts = static_library('rts',
	rts_public_headers + rts_sources,
	# lib sources expect to be able to include the lib headers directly w/o the rts prefix
	include_directories: include_directories('include/rts'),
	dependencies: [ threads, boost, rtlsdr ],
	install: true
)

rts = declare_dependency(
	dependencies: [threads, boost],
	link_with: librts,
	# ... but ext. users should include via the rts prefix
	include_directories: include_directories('include')
)

pkg_mod = import('pkgconfig')
pkg_mod.generate(libraries : [ librts, threads, boost ],
	version : '0.1',
	name : 'librts',
	filebase : 'librts',
	description : 'Library to receive and transmit Somfy RTS frames.'
)

subdir('test')
