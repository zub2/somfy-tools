#!/bin/bash
set -e

GPIO_ROOT=/sys/class/gpio

export_gpio() {
	GPIO_NR="$1"
	GPIO_DIRECTION="$2"

	GPIO_DIR="$GPIO_ROOT/gpio$GPIO_NR"

	if [ ! -e "$GPIO_DIR" ]; then
		echo "exporting GPIO $GPIO_NR..."
		echo "$GPIO_NR" > "$GPIO_ROOT/export"
	fi

	echo "setting direction to $GPIO_DIRECTION..."
	echo "$GPIO_DIRECTION" > "$GPIO_DIR/direction"
}

export_gpio 4 in
export_gpio 17 out
