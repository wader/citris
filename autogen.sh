#!/bin/sh
#
# autogen.sh 
#
# Requires: automake 1.9, autoconf 2.50
set -e

# Refresh GNU autotools toolchain.
for i in config.guess config.sub missing install-sh mkinstalldirs ; do
	test -r /usr/share/automake-1.9/${i} && {
		rm -f ${i}
		cp /usr/share/automake-1.9/${i} .
	}
	chmod 755 ${i}
done

aclocal-1.9
autoheader
automake-1.9 --foreign --add-missing
autoconf

exit 0
