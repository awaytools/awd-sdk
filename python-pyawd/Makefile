BLENPATH=/Applications/blender.app/Contents/MacOS/2.59/python/lib/python3.2
LIBPATH32=./build/lib.macosx-10.4-intel-3.2
LIBPATH26=./build/lib.macosx-10.6-intel-2.7

#DIST_CFLAGS=-g -arch ppc -arch i386 -arch x86_64 -isysroot /Developer/SDKs/MacOSX10.4u.sdk -fno-strict-aliasing -fno-common -dynamic -I/usr/local/include/
DIST_CFLAGS=-Wall -g -arch i386 -arch x86_64 -fno-strict-aliasing -fno-common -dynamic -I/usr/include
CFLAGS=-c -O0 $(DIST_FLAGS)

BLENSITE=$(BLENPATH)
MAYASITE=/Applications/Autodesk/maya2012/Maya.app/Contents/Frameworks/Python.framework/Versions/2.6/lib/python2.6/site-packages


install: dist_install26 dist_install32 blender_install maya_install

build26:
	CFLAGS="$(DIST_CFLAGS)" python2.7 setup.py clean build

build32:
	CFLAGS="$(DIST_CFLAGS)" python3 setup.py clean build

dist_install26: build26
	python2.7 setup.py install
    
dist_install32: build32
	python3 setup.py install
    
blender_install: build32
	cp -Rf $(LIBPATH32)/pyawd $(BLENSITE)/

maya_install: build26
	cp -R $(LIBPATH26)/pyawd $(MAYASITE)/

c4d_install: build26
	cp -R $(LIBPATH26)/pyawd ~richardolsson/Library/Preferences/MAXON/CINEMA\ 4D\ R13_DA28FA2D/library/python/packages/osx/

clean:
	rm -rf build
