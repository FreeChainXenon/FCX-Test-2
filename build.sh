#!/bin/sh

TOOLCHAIN_PREFIX=${FREECHAINXENON:-/usr/local/fcx}
TOOLCHAIN_NAME_PREFIX="powerpc64-fcx-xenon"
BASE_ADDR=0x82000000
ALIGNMENT=0x10000
PROGRAM_NAME="cause360error-c"

# Process the ELF linker script
${TOOLCHAIN_NAME_PREFIX}-cpp -DBASE_ADDR="${BASE_ADDR}" -DALIGNMENT="${ALIGNMENT}" -o fcxlds.lds ${TOOLCHAIN_PREFIX}/ldscripts/fcxlds.lds.tmpl

# Build the ELF
${TOOLCHAIN_NAME_PREFIX}-gcc -Wa,-mregnames,-a32,-mppc64 -Wl,-T,fcxlds.lds -m32 -mpowerpc64 -nostdlib -o ${PROGRAM_NAME}.elf crt0.s cause360error-c.c

# Process ELF loader
# File path *probably* doesn't contain '|', so we can use it as a separator.
# Can't use the C preprocessor here as syntax conflicts.
cat ${TOOLCHAIN_PREFIX}/elfloader/fcxelfldr.s.tmpl | sed "s|EXEC_NAME|${PROGRAM_NAME}.elf|" > fcxelfldr.s

# Assemble the ELF loader, and link it together with the ELF using the PE-targeted linker
${TOOLCHAIN_NAME_PREFIX}-as -mregnames -a32 -mppc64 -o fcxelfldr.o fcxelfldr.s
${TOOLCHAIN_NAME_PREFIX}pe-ld --accept-unknown-input-arch --image-base="${BASE_ADDR}" --section-alignment="${ALIGNMENT}" --subsystem=xbox \
			--disable-dynamicbase --disable-long-section-names -T ${FREECHAINXENON}/ldscripts/fcxpelds.lds \
			--major-subsystem-version 1 --minor-subsystem-version 0 --major-os-version 0 --minor-os-version 0 \
			-o ${PROGRAM_NAME}.exe fcxelfldr.o

# Finally, transform the PE into an XEX with SynthXEX
synthxex --skip-machine-check --input ${PROGRAM_NAME}.exe --output ${PROGRAM_NAME}.xex
