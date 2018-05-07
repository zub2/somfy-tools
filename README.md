# Somfy-tools

A set of tools to receive, decode, and transmit Somfy RTS Protocol frames, either on the Raspberry Pi using OOK receiver and transmitter modules connected to GPIO, or (only for input) using [rtl_sdr](https://osmocom.org/projects/sdr/wiki/rtl-sdr).

The protocol is used by some blinds, shades, rolling shutters, awnings, etc. by [Somfy](https://www.somfysystems.com/). If you have such a device, you can use this software to both receive what the Somfy remote sends and also to broadcast your own commands.

The code is based on the description available at the [Pushstack blog](https://pushstack.wordpress.com/somfy-rts-protocol/). Also included is a tool to record and play back signal via GPIO. This can be used for debugging.

## Compiling

CMake, a C++ compiler supporting C++ 17 (e.g. gcc 6.4) and the [boost](http://boost.org/) libraries are required. To build the complete `sdr-somfy-decoder`, [rtl_sdr](https://osmocom.org/projects/sdr/wiki/rtl-sdr) is needed too. (Without rtl_sdr only a version that can decode an IQ log (analytic signal) is built.)

## Tools

This is essentially a C++ version of [octave-somfy](https://github.com/zub2/octave-somfy) extended by the ability to record and transmit via GPIO.

The following tools are included:

* for Raspberry Pi (using GPIO):
	* `gpio-logger`
	* `gpio-transmitter`
	* `gpio-somfy-decoder`
	* `gpio-somfy-transmitter`
* using rtl_sdr:
	* `sdr-somfy-decoder`

The `gpio-*` tools can be only used on the Raspberry Pi because they use a mechanism specific to the device. Standard [Linux GPIO interface](https://www.kernel.org/doc/Documentation/gpio/sysfs.txt) is not used becuase its speed, at least on the Raspberry Pi, is not sufficient and it easily drops data. Raspberry Pi specific `mmap()` is used instead. For this reason access to `/dev/mem` is needed which normally requires to run the programs as root.

To decrease probability of dropping data or transmitting broken frames, the GPIO reading and writing threads use realtime priority ([SCHED_FIFO](http://linuxrealtime.org/index.php/Basic_Linux_from_a_Real-Time_Perspective#SCHED_FIFO_and_SCHED_RR)). This also requires root privileges. The high-priority threads only deal with sampling and writing GPIO data. Reading requires frequent polling, so it's more CPU intensive. While it should not eat up the whole CPU and leave the Raspberry Pi frozen, be warned that if some bug causes the reading or writing thread to misbehave, the Raspberry Pi could become non-responsive.

The `mmap()` + realtime thread polling approach is not the best the Raspbbery Pi can do. While it's faster than using `poll()` with the Linux GPIO interface, and it seems sufficient for decoding the Somfy RTS protocol, there are crazier tools that can sample faster. See e.g. [Panalyzer](https://github.com/richardghirst/Panalyzer) and [this discussion](https://www.raspberrypi.org/forums/viewtopic.php?f=37&t=7696).

When using the programs, note that each supports option `-h` or `--help` that prints some basic description of the arguments it accepts.

### GPIO setup

To use the `gpio-*` programs on a Raspberry Pi, an OOK transmitter and/or an OOK receiver module are needed. The ubiquitous and cheap [MX-FS-03V and MX-05](http://hobbycomponents.com/wired-wireless/168-433mhz-wireless-modules-mx-fs-03v-mx-05) worked OK enough for me, though it's not ideal as the RTS devices use 433.42 MHz while the modules use 433.92 MHz. There are some other options discussed in the comments at the [PushStack blog](https://pushstack.wordpress.com/somfy-rts-protocol/).

Keep in mind the the Raspberry Pi GPIO runs on 3.3V and if you connect it to 5V, you probably release the magic smoke. The MX-05 receiver is designed to run on 5V. Being lazy, I feed it 3.3V and it works.

GPIO needs to be configured to actually use the modules. Assuming the receiver is connected to GPIO 4 and the transmitter to GPIO 17 the following commands could be used (typically only root is allowed to do this):

```shell
# echo 4 > /sys/class/gpio/export
# echo in > /sys/class/gpio/gpio4/direction
# echo 17 > /sys/class/gpio/export
# echo out > /sys/class/gpio/gpio4/direction
```

An example shell script that does the same in a more extensible way is in `examples/setup-gpio.sh`.

### gpio-logger

A tool that samples a GPIO input and records durations between transitions. The resulting file can be loaded by [octave-somfy](https://github.com/zub2/octave-somfy) via the function `loadAndDecodeGPIOLog`.

To decrease jitter the tool works by preallocating a buffer of configurable size and then it keeps sampling the GPIO via memory reads using a thread with realtime priority. The thread sleeps between each sample. Only timepoints of transitions are stored in the log, so the size of buffer needed depends on how many transitions there are. The recording stops either after a given number of seconds (-d) passes or when the log becomes full.

Example:

```
# gpio-logger -n 4 -d 2 -f test_log.bin
Starting recording... done
```

### gpio-transmitter

A tool that can play back a text description of durations and values on an GPIO output.

The format of the description file is simple: Each line must be either empty or contain a number specifying the duration in µs, one or more whitespace characters and the output value (0, 1, or the strings false and true). Leading and trailing whitespace is ignored. The character `#` begins a comment that ends at the end of the line.

There is an example file (in `examples/gpio-play-test.log`) that can be played back:

```
# gpio-transmitter -n 17 -f examples/gpio-play-test.log
```

If you connect a LED to the output, you should see it blink.

### gpio-somfy-decoder

A tool that can decode Somfy RTS frames from an OOK receiver module connected to GPIO input. Alternatively, a GPIO log can be used as a source.

Example:

```
# gpio-somfy-decoder -n 4
>>> GOT SOMFY FRAME [type=normal] <<<
got all bits!
decoded bits: 1010.0001|1110.0111|1110.1110|1000.0110|1011.0101|1101.1100|1001.1001
decoded bytes: a1|e7|ee|86|b5|dc|99
key: 0xa1
code: 0x4 [Down]
rolling code: 0x0968
address: 0x336945
>>> GOT SOMFY FRAME [type=repeat] <<<
got all bits!
decoded bits: 1010.0001|1110.0111|1110.1110|1000.0110|1011.0101|1101.1100|1001.1001
decoded bytes: a1|e7|ee|86|b5|dc|99
key: 0xa1
code: 0x4 [Down]
rolling code: 0x0968
address: 0x336945
```

Although sometimes the decoding fails, e.g.:

```
>>> GOT SOMFY FRAME [type=repeat] <<<
DECODER error: expecting short transition
decoding failed after 13 bits!
decoded bits: 1010.0001|1110.0111|110
```

... and more often so than the `sdr-somfy-decoder`.


### gpio-somfy-transmitter

A tool that can transmit Somfy RTS frames via an OOK transmitter module connected to GPIO output.

As the RTS device keeps a list of associated remotes and a rolling counter for each, please first read up on the Somfy RTS protocol at the [PushStack blog](https://pushstack.wordpress.com/somfy-rts-protocol/).

If you want to control your blinds/shutters/what not from the Raspberry Pi or other device, you probably want to create a new "virtual" remote, rather than interfere with your existing one.

An example invocation looks like this:

```
# gpio-somfy-transmitter -n 17 -k 0xa4 -c down -r 335 -a 0x100001
```

### sdr-somfy-decoder

A tool that can decode Somfy RTS frames either from a [rtl_sdr](https://osmocom.org/projects/sdr/wiki/rtl-sdr) device or from a rtl_sdr log.

Example:

```
$ sdr-somfy-decoder -d 0
>>> GOT SOMFY FRAME [type=normal] <<<
got all bits!
decoded bits: 1010.0011|1110.0101|1110.1100|1000.0110|1011.0101|1101.1100|1001.1001
decoded bytes: a3|e5|ec|86|b5|dc|99
key: 0xa3
code: 0x4 [Down]
rolling code: 0x096a
address: 0x336945
>>> GOT SOMFY FRAME [type=repeat] <<<
got all bits!
decoded bits: 1010.0011|1110.0101|1110.1100|1000.0110|1011.0101|1101.1100|1001.1001
decoded bytes: a3|e5|ec|86|b5|dc|99
key: 0xa3
code: 0x4 [Down]
rolling code: 0x096a
address: 0x336945
```

## Tests

There are also some tests in the directory `test`. They can be run either via the build system (e.g. `make test`) or by running the executable `tests` directly.

## Links

This is not the only piece of software that can be used for the task. But finding other useful projects is, in my opinion, difficult because they are mostly obscure. Anyway here are some links I've found:

* [NodeMCU](https://en.wikipedia.org/wiki/NodeMCU)'s [Somfy module](https://nodemcu.readthedocs.io/en/master/en/modules/somfy/) - It seems the NodeMCU (an [ESP8266](https://en.wikipedia.org/wiki/ESP8266) with [custom firmware](http://nodemcu.com/index_en.html)) contains a module that can send Somfy RTS commands via a GPIO-connected OOK transmitter. I found this via [NodeMCU-Somfy](https://github.com/StryKaizer/NodeMCU-Somfy) which adds a web interface on top of it.
* [Somfy-Remote](https://github.com/Nickduino/Somfy_Remote) - An Arduino Sketch that can send a Somfy RTS frame via a GPIO-connected OOK transmitter. (NodeMCU's somfy module mentions it's based on this)
* [OpenHAB](http://www.openhab.org/) has binding to [RFXCOM devices](https://docs.openhab.org/addons/bindings/rfxcom1/readme.html). With the
[RFXtrx433E](http://www.rfxcom.com/RFXtrx433E-USB-43392MHz-Transceiver/en) and the binding you can send Somfy RTS frames.
* [henrythasler/sdr](https://github.com/henrythasler/sdr) - A collection of tools related to SDR, [Somfy RTS receiver and transmitter](https://github.com/henrythasler/sdr/tree/master/somfy) are included. Also contains some useful RF intro documents.

There are also some interesting devices:

* [RFXtrx433E](http://www.rfxcom.com/RFXtrx433E-USB-43392MHz-Transceiver/en) which I discovered via the [OpenHAB binding](https://docs.openhab.org/addons/bindings/rfxcom1/readme.html). It connects via USB, so you can connect it to any PC and you don't have to worry about timing, the device handles that. It also has a transmitter at the correct frequency for Somfy RTS (433.42 MHz). It presents itself as a serial device so communicating with it is also easy. But it's not cheap and as far as I can tell it can't receive Somfy RTS frames.
* [RFM69HCW](http://www.hoperf.com/rf_transceiver/modules/RFM69HCW.html) (used by [henrythasler/sdr Somfy RTS receiver and transmitter](https://github.com/henrythasler/sdr/tree/master/somfy)) - It connects via [SPI](https://cs.wikipedia.org/wiki/Serial_Peripheral_Interface) and seems to have [many features](http://www.hoperf.com/upload/rf/RFM69HCW-V1.1.pdf).
