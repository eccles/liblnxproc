topiary
=======

C library for accessing /proc and /sys filesystems (and other system files such
as NTP drift) in modular fashion

Installation
------------

The standard installation targets are available viz:

    ./configure
    make
    make check
    make dist
    make distcheck
    sudo make install
    make installcheck
    sudo make uninstall

Additionally VPATH builds are also available:

    make distclean (if previously made above)
    ./makeall.sh    (makes all VPATH targets)

    ./makeall.sh build (makes standard target in directory build)

    ./makeall.sh debug (makes target with trace statements enabled)

    ./makeall.sh valgrind (makes target and checks for memory leaks etc. using valgrind)
                if valgrind is installed, inspect file valgrind/check.log

    ./makeall.sh gcov (makes target and computes test coverage for all test routines)
                if gcov is installed, inspect file gcov/check.log

    ./makeall.sh lcov (makes target and computes test coverage for all test routines)
                if lcov is installed, point browser to lcov/coverage-{lib|bin}/index.html

See files 'options.{build|debug|valgrind|gcov|lcov}' for the settings for each of these builds

