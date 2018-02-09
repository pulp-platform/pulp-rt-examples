coremark_v1.0:
	tar xvfz coremark_v1.0.tgz

patch: coremark_v1.0
	cd coremark_v1.0 && patch -p1 < ../coremark.patch

diff:
	-diff -Naur coremark_v1.0_ref coremark_v1.0 > coremark.patch

