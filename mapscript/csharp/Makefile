# $Id: Makefile.in 11285 2011-03-19 21:53:46Z tamas $
#
# Run ./configure in the main MapServer directory to turn this Makefile.in
# into a proper Makefile

#
# If you want to ignore missing datafile errors uncomment the following
# line. This is especially useful with large tiled datasets that may not
# have complete data for each tile.
#
#IGNORE_MISSING_DATA=-DIGNORE_MISSING_DATA
IGNORE_MISSING_DATA = 

#
# If you want to use shape Z and M parameter this option must be set.
# It's OFF by default.
#
#USE_POINT_Z_M=-DUSE_POINT_Z_M
USE_POINT_Z_M = 

#
# Apparently these aren't as commonplace as I'd hoped. Edit the
# following line to reflect the missing functions on your platform.
#
# STRINGS=-DNEED_STRCASECMP -DNEED_STRNCASECMP -DNEED_STRDUP -DNEED_STRLCAT
STRINGS= -DHAVE_VSNPRINTF -DNEED_STRLCPY -DNEED_STRLCAT -DNEED_STRRSTR 

#
# Flags.
#

XTRALIBS=  -lm -lstdc++
RUNPATHS= 

FLAGS =                   -DUSE_WMS_SVR        -DUSE_PROJ -DUSE_AGG_SVG_SYMBOLS    -DUSE_GD_GIF -DUSE_GD_PNG -DUSE_GD_JPEG -DUSE_GD_WBMP -DUSE_GD_FT -DGD_HAS_FTEX_XSHOW -DGD_HAS_GDIMAGEGIFPTR -DGD_HAS_GETBITMAPFONTS -DGD_HAS_GET_TRUE_COLOR_PIXEL -DUSE_ICONV -DUSE_GIF -DUSE_PNG -DUSE_ZLIB -DUSE_FREETYPE               -Irenderers/agg/include  -Irenderers/agg/include   -I/usr/include   -I/usr/include/freetype2  $(STRINGS) $(IGNORE_MISSING_DATA) $(USE_POINT_Z_M)

CCFLAGS   = -O2 -fPIC -Wall  -DNDEBUG  $(FLAGS)

# Link flags and shared libs only
SUP_LIBS =               -lproj  -lexpat   -lgd -L/usr/lib -lgd -ljpeg -lfreetype -lz -lpng -lz -lXpm -lX11  -lc -lz -lfreetype -lz 

# STATIC_LIBS is full filename with path of libs that will be statically linked
STATIC_LIBS= $(GD_STATIC)

LDFLAGS= $(RUNPATHS) -L../.. -lmapserver $(SUP_LIBS) $(STATIC_LIBS)

LIBMAPSCRIPT_SHARED= libmapscript.so
LD_SHARED= g++ -shared 

# END OF CONFIGURE----

#
# SWIG Stuff
#
SWIG= swig

#
# CSHARP Stuff
#
CSC= mcs

CC= gcc

#
# --- You shouldn't have to edit anything else. ---
#
all: interface mapscript_so mapscript_csharp

sign:
	sn -k mapscript.snk

interface: ../mapscript.i
	$(SWIG) -csharp -namespace OSGeo.MapServer $(FLAGS) -o mapscript_wrap.c ../mapscript.i

mapscript_so: mapscript_wrap.c
	$(CC) -fpic -c $(CCFLAGS) mapscript_wrap.c
	$(LD_SHARED) mapscript_wrap.o -o $(LIBMAPSCRIPT_SHARED) $(LDFLAGS)

mapscript_csharp::
	$(CSC) /t:library /out:mapscript_csharp.dll *.cs config/AssemblyInfo.cs
	$(CSC) /r:mapscript_csharp.dll /out:shpdump.exe examples/shpdump.cs
	$(CSC) /r:mapscript_csharp.dll /out:shapeinfo.exe examples/shapeinfo.cs
	$(CSC) /r:mapscript_csharp.dll /out:drawmap.exe examples/drawmap.cs
	$(CSC) /r:mapscript_csharp.dll /out:inline.exe examples/inline.cs
	$(CSC) /r:mapscript_csharp.dll /out:drawquery.exe examples/drawquery.cs
	$(CSC) /r:mapscript_csharp.dll /r:System.Drawing.dll /out:getbytes.exe examples/getbytes.cs
	$(CSC) /r:mapscript_csharp.dll /out:HTMLtemplate.exe examples/HTMLtemplate.cs
	$(CSC) /r:mapscript_csharp.dll /out:RFC24.exe examples/RFC24.cs

dll_config: Makefile
	rm -rf mapscript_csharp.dll.config
	echo "<configuration>" >> mapscript_csharp.dll.config
	echo "<dllmap dll=\"mapscript\" target=\""mapscript"\">" >> mapscript_csharp.dll.config
	echo "</configuration>" >> mapscript_csharp.dll.config

clean:
	rm -rf edu *.o *.c *.cs *.so *.dll *.dll.config

test:
	LC_ALL=C mono ./shpdump.exe ../../tests/point.shp
	LC_ALL=C mono ./shapeinfo.exe ../../tests/point.shp
	LC_ALL=C mono ./inline.exe png24 inline_test.png
	LC_ALL=C mono ./getbytes.exe ../../tests/test.map test_csharp2.png
	LC_ALL=C mono ./RFC24.exe ../../tests/test.map

