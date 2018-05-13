boost = dependency('boost', modules: ['program_options', 'unit_test_framework'])

executable('gpio-logger', [
		'GPIOLogWriter.cpp',
		'GPIOLogWriter.h',
		'GPIOLogger.cpp'
	],
	dependencies: [ boost, rts ],
	install: true
)

executable('gpio-somfy-decoder', [
		'SigIntHandler.cpp',
		'SigIntHandler.h',
		'GPIOLogReader.cpp',
		'GPIOLogReader.h',
		'GPIOSomfyDecoder.cpp'
	],
	dependencies: [ boost, rts ],
	install: true
)

sdr_somfy_decoder_sources = [
	'IQLogReader.cpp',
	'IQLogReader.h',
	'SDRSomfyDecoder.cpp'
]

if have_rtlsdr
	sdr_somfy_decoder_sources += [
		'SigIntHandler.cpp',
		'SigIntHandler.h'
	]
endif

executable('sdr-somfy-decoder',
	sdr_somfy_decoder_sources,
	dependencies: [ boost, rts ],
	install: true
)

executable('gpio-transmitter', [
		'DurationFileReader.cpp',
		'DurationFileReader.h',
		'GPIOTransmitter.cpp'
	],
	dependencies: [ boost, rts ],
	install: true
)

executable('gpio-somfy-transmitter', [
		'GPIOLogWriter.cpp',
		'GPIOLogWriter.h',
		'GPIOSomfyTransmitter.cpp'
	],
	dependencies: [ boost, rts ],
	install: true
)
