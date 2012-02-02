# Hash Algorithm Collision Fix using an Apache Module

This Apache module can be used as a workaround for DOS attacks which take advantage of the 
predictability of string hashing used in several web server technologies. The vulnerability 
is tracked at http://www.kb.cert.org/vuls/id/903934

The details of this DOS threat are explained in http://www.youtube.com/watch?v=R2Cq3CLI6H8
at the 28th Chaos Communication Congress by Alexander Klink and Julian Walde.

## Status

This module can be used to protect Java and PHP5 sites that are reversed proxied using Apache
httpd.

## Install

The common way to install this module is:
```
	./configure --with-apache=/path/to/apache/install
	make
	make install
```
This will result in a DSO installation of the module in the Apache modules directory
and the activation of the module in the httpd.conf file by adding a line similar to:
```
	LoadModule hacf_module        modules/libmodhacf.so
```

The unit tests are run using the standard Automake idiom:
```
	make check
```

## Install from source repository

If you want to install from the sources you checked out from the source repository,
you will need to generate the configure script and supporting files using the GNU
autotools.

The autotools used by the author are:

- autoconf (GNU Autoconf) 2.61
- automake (GNU automake) 1.10
- libtoolize (GNU libtool) 2.2.10

Newer versions should work also.

To generate the configure script and supporting files it should suffice to run:
```
	autoreconf --install
```

## Usage

For a Java site with URL http://server.example.com/app one would use:
```
	<Location /app>
	  HacfLanguage Java
	  ProxyPass http://server.example.com/app
	  ProxyPassReverse http://server.example.com/app
	</Location>
```
For a PHP5 site one would use PHP as the value of the HacfLanguage setting

The request filtering can be tuned using the parameters:

- HacfMaxCollisions, maximum number of allowed collisions before a request is dropped (default 3)
- HacfMaxParameters, maximum number of allowed parameters before a request is dropped  (default 256)

The default settings for these parameters can seem aggressive but the should be fine for
most applications.

## TODO

The current features are very limited to the environment this module originally was written
for, but the following TODOs should be ready before version 1.0.0

- support Apache 2.2 and higher
- smart multipart post filtering
- PHP4 support, uses a different Hash Algorithm than PHP5
- Python support, 32bit only because 64bit doesn't seem practical to exploit 
- Use fast inline hash functions, add performance testing
- Try http://check.sourceforge.net/ for the unit tests
