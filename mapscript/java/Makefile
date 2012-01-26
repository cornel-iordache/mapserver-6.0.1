# Run ./configure in the main MapServer directory to turn this Makefile.in
# into a proper Makefile

prefix		=	/usr/local
exec_prefix	=	${prefix}
INST_PREFIX	=	${prefix}
INST_LIB	=	${exec_prefix}/lib
INST_BIN	=	${exec_prefix}/bin

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
# If you want to use disable fast versions of NINT (used by default) then
# define the following
# It's OFF by default.
#
#USE_NINT=-DUSE_GENERIC_MS_NINT
USE_NINT = 

#
# Apparently these aren't as commonplace as I'd hoped. Edit the
# following line to reflect the missing functions on your platform.
#
# STRINGS=-DNEED_STRCASECMP -DNEED_STRNCASECMP -DNEED_STRDUP -DNEED_STRLCAT
STRINGS= -DHAVE_VSNPRINTF -DNEED_STRLCPY -DNEED_STRLCAT -DNEED_STRRSTR 

# Proj.4 distribution (cartographic projection routines). Not required for normal use. (EXPERIMENTAL)
PROJ_INC= 
PROJ_LIBS= -lproj
PROJ=     -DUSE_PROJ

# GD distribution (graphics library GIF and/or PNG support). (REQUIRED)
#
#   - Versions 1.3 to 1.5 write non-LZW GIF (-DUSE_GD_1_3).
#   - Versions 1.6 and greater write PNG (-DUSE_GD_1_6). Add -lpng -lz to GD_LIB line.
#
#
GDFONT_OBJ=gd-1.2/gdfontt.o gd-1.2/gdfonts.o gd-1.2/gdfontmb.o gd-1.2/gdfontl.o gd-1.2/gdfontg.o
GD_INC=  -I/usr/include
GD_LIB=  -lgd -L/usr/lib -lgd -ljpeg -lfreetype -lz -lpng -lz -lXpm -lX11 
GD=       -DUSE_GD_GIF -DUSE_GD_PNG -DUSE_GD_JPEG -DUSE_GD_WBMP -DUSE_GD_FT -DGD_HAS_FTEX_XSHOW -DGD_HAS_GDIMAGEGIFPTR -DGD_HAS_GETBITMAPFONTS -DGD_HAS_GET_TRUE_COLOR_PIXEL
GD_STATIC = 

#
# Optional Anti-Grain Geometry Support.
#
#AGG=     @AGG_ENABLED@ commented because it breaks the build
AGG_INC=  -Irenderers/agg/include 
AGG_OBJ=  renderers/agg/src/clipper.o renderers/agg/src/agg_arc.o renderers/agg/src/agg_curves.o renderers/agg/src/agg_vcgen_contour.o renderers/agg/src/agg_vcgen_dash.o renderers/agg/src/agg_vcgen_stroke.o renderers/agg/src/agg_image_filters.o renderers/agg/src/agg_line_aa_basics.o renderers/agg/src/agg_line_profile_aa.o renderers/agg/src/agg_sqrt_tables.o renderers/agg/src/agg_embedded_raster_fonts.o renderers/agg/src/agg_trans_affine.o renderers/agg/src/agg_vpgen_clip_polygon.o renderers/agg/src/agg_vpgen_clip_polyline.o renderers/agg/src/agg_font_freetype.o renderers/agg/src/agg_svg_parser.o renderers/agg/src/agg_svg_path_renderer.o renderers/agg/src/agg_svg_path_tokenizer.o
AGG_LIB=  -lexpat

#
# Optional Opengl Support.
#
FTGL=     
FTGL_LIB=  
FTGL_INC=  

#Freetype support
FT_ENABLE=     -DUSE_FREETYPE
FT_LIB=  -lfreetype -lz
FT_INC=  -I/usr/include/freetype2

#
# Optional Opengl Support.
#
OGL=     
OGL_LIB=  
OGL_INC=  

PNG_INC= 
PNG_LIB= -lpng

#
# Giflib support
#
GIF_INC= 
GIF_LIB= -lgif
GIF=     -DUSE_GIF

# 
# ZLIB option (compressed SVG)
#
ZLIB_INC=  
ZLIB_LIB=  -lz
ZLIB=      -DUSE_ZLIB

# JPEG distribution (raster support for grayscale JPEG images, INPUT ONLY).
JPEG_INC= 
JPEG_LIB= -ljpeg
JPEG=     -DUSE_JPEG

# ESRI SDE Support. You MUST have the SDE Client libraries and include files
# on your system someplace. The actual SDE server you wish to connect to can
# be elsewhere.
SDE=	  
SDE_LIB=  
SDE_INC=  

# Optional OGR Support.  OGC Simple Feature inspired interface for vector
# formats.  See http://ogr.maptools.org/
# Note that since OGR is part of the GDAL library, it uses GDAL_LIB + GDAL_INC
OGR=	  

# Optional GDAL Support (provides read access to a variety of raster formats)
# See http://www.remotesensing.org/gdal/
GDAL=	  
GDAL_LIB=  
GDAL_INC=  

# Optional GEOS Support.
# See http://geos.refractions.net/
GEOS=     
GEOS_LIB=  
GEOS_INC=  

# Optional PostGIS Support.  See http://postgis.refractions.net/
POSTGIS=      
POSTGIS_LIB=  
POSTGIS_INC=  

# Optional Mysql Support.  See http://postgis.refractions.net/
MYSQL=      
MYSQL_LIB=  
MYSQL_INC=  

# Optional ORACLESPATIAL Support. Contact: cabral@cttmar.univali.br
ORACLESPATIAL=
ORACLESPATIAL_LIB=
ORACLESPATIAL_INC=

# libcurl ... required for WMS/WFS Client Connections
CURL_INC=
CURL_LIB=

# libfribibi ... 
FRIBIDI=
FRIBIDI_INC=
FRIBIDI_LIB=


# libxml2 ... required for OGC SOS Server
XML2_INC=
XML2_LIB=

# libxslt ... required for xml mapfile
XSLT_INC=
XSLT_LIB=

# libexslt ... required for xml mapfile
EXSLT_INC=
EXSLT_LIB=

# OWS: OGC Web Services support
# OGC WMS Server:  -DUSE_WMS_SVR (Requires PROJ4 support)
# OGC WMS Client Connections:  -DUSE_WMS_LYR (Requires PROJ4 and libcurl)
# OGC WFS Server: -DUSE_WFS_SVR (Requires OGR, PROJ4 recommended)
# OGC WFS Client: -DUSE_WFS_LYR (Requires OGR, PROJ4 and libcurl)
# OGC WCS Server: -DUSE_WCS_SVR (Requires GDAL and PROJ4)
# OGC SOS Server: -DUSE_SOS_SVR (Requires PROJ4 and libxml2)
OWS=-DUSE_WMS_SVR     

#
# IMPORTANT NOTE ABOUT REGEX FOR PHP_MAPSCRIPT USERS:
#
# In order to compile the PHP_MAPSCRIPT module, we have to make MapServer
# uses the same version of the REGEX library that PHP was compiled with:
#
REGEX_OBJ=
REGEX_INC=

#
# Multithreading support.
#
THREAD=
THREAD_LIB=

#
# libiconv - Enables Internationalization
#
ICONV=-DUSE_ICONV
ICONV_LIB=-lc
ICONV_INC=

CAIRO=
CAIRO_LIB=
CAIRO_INC=

#
# Pick a compiler, etc. Flex and bison are only required if you need to modify the mapserver lexer (maplexer.l) or expression parser (mapparser.y).
#
CXX=	g++
CC=     gcc
LD=     g++
AR= ar rc
RANLIB= ranlib
LEX=    :
YACC=   yacc
LD_SHARED = g++ -shared 

XTRALIBS=  -lm -lstdc++
RUNPATHS= 

CCFLAGS= -O2 -fPIC -Wall  -DNDEBUG   \
        $(IGNORE_MISSING_DATA) $(STRINGS) $(PROJ) $(OWS) \
	$(ORACLESPATIAL) $(JPEG) $(GD) $(AGG) $(PDF) $(SDE) $(OGR) $(GDAL) $(GEOS) $(ICONV) \
	$(POSTGIS) $(MYSQL) $(THREAD) $(GD_INC) $(AGG_INC) \
	$(PROJ_INC) $(JPEG_INC) $(EGIS_INC) $(SDE_INC) \
	$(REGEX_INC) $(GDAL_INC) $(POSTGIS_INC) $(MYSQL_INC) \
	$(CURL_INC) $(ORACLESPATIAL_INC) $(GEOS_INC) $(ICONV_INC) $(XML2_INC) \
	$(FRIBIDI_INC) $(CAIRO_INC) $(OGL_INC) $(FTGL_INC)

# Link flags and shared libs only
SUP_LIBS =  $(FT_LIB) $(GD_LIB) $(AGG_LIB) $(OGL_LIB) $(FTGL_LIB) $(PROJ_LIBS) \
          $(JPEG_LIB) $(PNG_LIB) $(GIF_LIB) $(SDE_LIB) $(GDAL_LIB) $(POSTGIS_LIB) \
	  $(MYSQL_LIB) $(CURL_LIB) $(ORACLESPATIAL_LIB) $(GEOS_LIB) \
          $(THREAD_LIB) $(ICONV_LIB) $(FASTCGI_LIB) $(XSLT_LIB) $(EXSLT_LIB) \
          $(ZLIB_LIB) $(XML2_LIB) $(FRIBIDI_LIB) $(XTRALIBS) $(CAIRO_LIB)

# STATIC_LIBS is full filename with path of libs that will be statically linked
STATIC_LIBS= $(GD_STATIC)

EXE_LDFLAGS =	$(RUNPATHS) -L. -lmapserver $(SUP_LIBS) $(STATIC_LIBS)

LDFLAGS= $(RUNPATHS) -L../.. -lmapserver $(SUP_LIBS) $(STATIC_LIBS)

LIBMAPSCRIPT_SHARED= libmapscript.so
LD_SHARED= g++ -shared 

# END OF CONFIGURE----

#
# SWIG Stuff
#
SWIG=swig

#
# Java Stuff
#
JAVADOC=javadoc
JAVAC=javac
JAVA=java
JAR=jar
JAVA_INCLUDE=-I${JAVA_HOME}/include -I${JAVA_HOME}/include/linux

CC=gcc

#
# --- You shouldn't have to edit anything else. ---
#
all: mapscript_jar

# This target is for users who are generating their own interface
# with SWIG versions lower than 1.3.19 (no -outdir)
# It is here for bacwkards compatibilty as swig <1.3.24 is no longer supported
interface: interface_release

interface_release: mapscript_wrap.c

# This target will be used in the release script
mapscript_wrap.c: ../mapscript.i
	mkdir -p edu/umn/gis/mapscript 
	$(SWIG) -java -package edu.umn.gis.mapscript -outdir edu/umn/gis/mapscript -o mapscript_wrap.c ../mapscript.i

mapscript_so: mapscript_wrap.c
	$(CC) -fpic -c $(CCFLAGS) $(JAVA_INCLUDE) -fno-strict-aliasing mapscript_wrap.c
	$(LD_SHARED) -fpic mapscript_wrap.o -o $(LIBMAPSCRIPT_SHARED) $(LDFLAGS)

mapscript_java: mapscript_wrap.c
	$(JAVAC) edu/umn/gis/mapscript/*.java

mapscript_javadoc: mapscript_wrap.c
	$(JAVADOC) -d doc -sourcepath edu/umn/gis/mapscript/*.java

mapscript_jar: mapscript_java mapscript_so
	$(JAR) cf mapscript.jar edu

clean:
	rm -rf edu *.o *.c *.so *.java *.jar *.png
	rm -rf examples/*.class
	rm -rf tests/threadtest/*.class

ifneq ($(PROJ),)
ifneq ($(GDAL),)
test:
	$(JAVAC) -classpath ./:examples/:./mapscript.jar -d examples/ examples/RFC24.java examples/ConnPool.java examples/DrawMap.java examples/DumpShp.java examples/MakePoint.java examples/QueryByAttribute.java examples/ShapeInfo.java examples/WxSTest.java examples/Metadata.java examples/RunTimeBuiltWMSClient.java
	$(JAVAC) -encoding utf8 -classpath ./:examples/:./mapscript.jar -d examples/ examples/QueryByAttributeUnicode.java 
	$(JAVA) -classpath ./:examples/:./mapscript.jar -Djava.library.path=. DumpShp ../../tests/point.shp
	$(JAVA) -classpath ./:examples/:./mapscript.jar -Djava.library.path=. ShapeInfo ../../tests/point.shp ../../tests/point.dbf
	$(JAVA) -classpath ./:examples/:./mapscript.jar -Djava.library.path=. DrawMap ../../tests/test.map ./map.png
	$(JAVA) -classpath ./:examples/:./mapscript.jar -Djava.library.path=. ConnPool
	$(JAVA) -classpath ./:examples/:./mapscript.jar -Djava.library.path=. QueryByAttribute ../../tests/test.map
ifneq ($(findstring USE_WCS_SVR, $(OWS)),)
	$(JAVA) -classpath ./:examples/:./mapscript.jar -Djava.library.path=. WxSTest ../../tests/test.map
else
	@echo "WARN: Skipping WxSTest because OWS is not enabled"
endif
	$(JAVA) -classpath ./:examples/:./mapscript.jar -Djava.library.path=. RFC24 ../../tests/test.map
	LC_ALL=en_US.UTF-8 $(JAVA) -classpath ./:examples/:./mapscript.jar -Djava.library.path=. QueryByAttributeUnicode data/subset-umlauts.map
else
test:
	@echo "To run the tests GDAL and PROJ must be enabled"
endif
else
test:
	@echo "To run the tests GDAL and PROJ must be enabled"
endif

threadtests:
	$(JAVAC) -cp ./mapscript.jar -d tests/threadtest/ tests/threadtest/*.java
	@echo "-------------------------------------------------------------------------"
	@echo "   You can use your own map! Copy the following command in your shell"
	@echo "        and change the file to the map file (the last argument)"
	@echo "-------------------------------------------------------------------------"
	$(JAVA) -Djava.library.path=./ -cp tests/threadtest/:./mapscript.jar MapTest -t 10 -i 5 ../../tests/test.map

