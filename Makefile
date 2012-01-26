# Run ./configure in the main MapServer directory to turn this Makefile.in
# into a proper Makefile

prefix		=	/usr/local
exec_prefix	=	${prefix}
INST_PREFIX	=	${prefix}
INST_LIB	=	${exec_prefix}/lib
INST_BIN	=	${exec_prefix}/bin

#
# MS_VERSION - Set by configure based in mapserver.h
#
MS_VERSION=	6.0.1

#
# Name of static and shared libs to produce
#
LIBMAP_STATIC=	libmapserver.a
LIBMAP_SHARED=	libmapserver.so
LIBMAP_SH_VER=	libmapserver.$(MS_VERSION).so

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

# Proj.4 distribution (cartographic projection routines). Not required for normal use.
PROJ_INC=  
PROJ_LIBS= -lproj
PROJ=      -DUSE_PROJ

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
AGG=     @AGG_ENABLED@
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
GIF=	 -DUSE_GIF

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

# Optional Mysql Support. 
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



# Optional FastCGI
FASTCGI=
FASTCGI_INC=
FASTCGI_LIB=

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
# use the same version of the REGEX library that PHP was compiled with:
#
PHP_REGEX_OBJ=
PHP_REGEX_INC=

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


# figure out how to build a shared library.  
# most of the relevant stuff is in AC_LD_SHARED in 
# aclocal.m4

LD_SHARED = g++ -shared 
LD_SONAME_LIBMAP = -W1,-soname,$(LIBMAP_SH_VER)

XTRALIBS=  -lm -lstdc++
RUNPATHS= 

DEFINES = $(IGNORE_MISSING_DATA) $(USE_POINT_Z_M) $(STRINGS)                  -DUSE_WMS_SVR        -DUSE_PROJ -DUSE_AGG_SVG_SYMBOLS    -DUSE_GD_GIF -DUSE_GD_PNG -DUSE_GD_JPEG -DUSE_GD_WBMP -DUSE_GD_FT -DGD_HAS_FTEX_XSHOW -DGD_HAS_GDIMAGEGIFPTR -DGD_HAS_GETBITMAPFONTS -DGD_HAS_GET_TRUE_COLOR_PIXEL -DUSE_ICONV -DUSE_GIF -DUSE_PNG -DUSE_ZLIB -DUSE_FREETYPE  $(FRIBIDI) -DDISABLE_CVSID

INCLUDES = $(FT_INC) $(REGEX_INC) $(PNG_INC) $(GIF_INC) $(JPEG_INC) $(GD_INC) \
        $(AGG_INC) $(OGL_INC) $(FTGL_INC) $(PROJ_INC) $(EGIS_INC) \
        $(SDE_INC) $(GDAL_INC) $(POSTGIS_INC) $(MYSQL_INC) \
        $(CURL_INC) $(ORACLESPATIAL_INC) $(GEOS_INC) $(ICONV_INC) \
        $(FASTCGI_INC) $(ZLIB_INC) $(XML2_INC) $(FRIBIDI_INC) $(CAIRO_INC)

FLAGS =  $(DEFINES) $(INCLUDES)

CFLAGS   = -O2 -fPIC -Wall  -DNDEBUG  $(FLAGS)
CXXFLAGS = -O2 -fPIC -Wall  -DNDEBUG  $(FLAGS)

# Link flags and shared libs only
SUP_LIBS =  $(FT_LIB) $(GD_LIB) $(AGG_LIB) $(OGL_LIB) $(FTGL_LIB) $(PROJ_LIBS) \
          $(JPEG_LIB) $(PNG_LIB) $(GIF_LIB) $(SDE_LIB) $(GDAL_LIB) $(POSTGIS_LIB) \
	  $(MYSQL_LIB) $(CURL_LIB) $(ORACLESPATIAL_LIB) $(GEOS_LIB) \
	  $(THREAD_LIB) $(ICONV_LIB) $(FASTCGI_LIB) $(XSLT_LIB) $(EXSLT_LIB) \
	  $(ZLIB_LIB) $(XML2_LIB) $(FRIBIDI_LIB) $(XTRALIBS)  $(CAIRO_LIB)

# STATIC_LIBS is full filename with path of libs that will be statically linked
STATIC_LIBS= $(GD_STATIC)

EXE_LDFLAGS =	$(RUNPATHS) -L. -lmapserver $(SUP_LIBS) $(STATIC_LIBS)

RM= /bin/rm -f

OBJS= $(AGG_OBJ) mapgeomutil.o mapdummyrenderer.o mapogl.o mapoglrenderer.o mapoglcontext.o \
				mapimageio.o mapcairo.o maprendering.o mapgeomtransform.o mapquantization.o \
				maptemplate.o mapbits.o maphash.o mapshape.o mapxbase.o mapparser.o maplexer.o \
				maptree.o mapsearch.o mapstring.o mapsymbol.o mapfile.o maplegend.o maputil.o \
				mapscale.o mapquery.o maplabel.o maperror.o mapprimitive.o mapproject.o mapraster.o \
				mapsde.o mapogr.o mappostgis.o maplayer.o mapresample.o mapwms.o \
				mapwmslayer.o maporaclespatial.o mapgml.o mapprojhack.o mapthread.o mapdraw.o \
				mapgd.o mapagg.o mapoutput.o mapgdal.o mapimagemap.o mapows.o mapwfs.o \
				mapwfs11.o mapwfslayer.o mapcontext.o maphttp.o mapdrawgdal.o mapjoin.o mapgraticule.o \
				mapcopy.o mapogcfilter.o mapogcsld.o maptime.o mapwcs.o mapwcs11.o mapcpl.o cgiutil.o \
				maprasterquery.o mapobject.o mapgeos.o classobject.o layerobject.o mapio.o mappool.o \
				mapregex.o mappluginlayer.o mapogcsos.o mappostgresql.o mapcrypto.o mapowscommon.o \
				maplibxml2.o mapdebug.o mapchart.o maptclutf.o mapxml.o mapkml.o mapkmlrenderer.o \
				mapogroutput.o mapwcs20.o  mapogcfiltercommon.o mapunion.o mapcluster.o

EXE_LIST = 	shp2img legend mapserv shptree shptreevis \
		shptreetst scalebar sortshp mapscriptvars tile4ms \
		msencrypt mapserver-config

#
# --- You shouldn't have to edit anything else. ---
#
all: $(MAKE_GD) libmapserver.a $(EXTRA_DEFAULT) $(EXE_LIST)  

# Explicitly invoke this rule when maplexer.l is altered.  We don't do
# it automatically for the reasons listed in #2310

lexer:
	$(LEX) --nounistd -Pmsyy -i -omaplexer.c maplexer.l

maplexer.c:	maplexer.l
	@echo '----------------------------------------------------------------'
	@echo '--  Please run "make lexer" if you have altered maplexer.l     -'
	@echo '----------------------------------------------------------------'

#
# Non-gnumake's don't seem to use this pattern rule, 
# but have a similar built-in rule for C code.  So try not
# to change these, since the change is unlikely to stick.
# 
.c.o:
	$(CC) -c $(CFLAGS) $< -o $@
   
mapogl.o: mapogl.cpp
	$(CXX) -c $(CXXFLAGS) mapogl.cpp -o mapogl.o

mapoglrenderer.o: mapoglrenderer.cpp mapoglrenderer.h
	$(CXX) -c $(CXXFLAGS) mapoglrenderer.cpp -o mapoglrenderer.o
   
mapoglcontext.o: mapoglcontext.cpp mapoglcontext.h
	$(CXX) -c $(CXXFLAGS) mapoglcontext.cpp -o mapoglcontext.o
   
mapogr.o:	mapogr.cpp
	$(CXX) -c $(CXXFLAGS) mapogr.cpp -o mapogr.o

php_mapscript:: $(LIBMAP_STATIC)
	cd mapscript/php; $(MAKE); cd ../..

maplexer.o: maplexer.c mapserver.h mapfile.h

mapparser.o: mapparser.c mapserver.h

mapparser.c: mapparser.y
	$(YACC) -d -omapparser.c mapparser.y

lib:    $(LIBMAP_STATIC)
libmapserver: $(LIBMAP_STATIC)
libmapserver.a: mapserver.h $(OBJS)
	$(AR) $(LIBMAP_STATIC) $(OBJS)
	$(RANLIB) $(LIBMAP_STATIC)
	# We don't want to let the shared library get out of sync with the
	# static library if it exists. 
	if test -r $(LIBMAP_SHARED) ; then \
	  make shared ; \
	fi

shared:	$(LIBMAP_SHARED)
$(LIBMAP_SHARED):	$(LIBMAP_STATIC)
	$(LD_SHARED) $(LD_SONAME_LIBMAP) -o $(LIBMAP_SH_VER) \
		$(RUNPATHS) $(OBJS) $(SUP_LIBS) $(STATIC_LIBS) \
	&& ln -f -s $(LIBMAP_SH_VER) $(LIBMAP_SHARED)

shp2pdf: $(LIBMAP_STATIC) shp2pdf.o mapserver.h
	$(LD) $(CFLAGS) shp2pdf.o $(EXE_LDFLAGS) -o shp2pdf

shp2img: $(LIBMAP_STATIC)  shp2img.o mapserver.h
	$(LD) $(CFLAGS) shp2img.o $(EXE_LDFLAGS) -o shp2img

sym2img: $(LIBMAP_STATIC)   sym2img.o mapserver.h
	$(LD) $(CFLAGS) sym2img.o $(EXE_LDFLAGS) -o sym2img

legend: $(LIBMAP_STATIC)  legend.o mapserver.h
	$(LD) $(CFLAGS) legend.o $(EXE_LDFLAGS) -o legend

scalebar: $(LIBMAP_STATIC)  scalebar.o mapserver.h
	$(LD) $(CFLAGS) scalebar.o $(EXE_LDFLAGS) -o scalebar

mapserv: mapserv.h maptile.h $(LIBMAP_STATIC)  mapserv.o cgiutil.o maptile.o mapserver.h
	$(LD) $(CFLAGS) mapserv.o cgiutil.o maptile.o $(EXE_LDFLAGS) -o mapserv

shpindex: $(LIBMAP_STATIC) shpindex.o mapserver.h
	$(LD) $(CFLAGS) shpindex.o $(EXE_LDFLAGS) -o shpindex

shptree: $(LIBMAP_STATIC) shptree.o mapserver.h
	$(LD) $(CFLAGS) shptree.o $(EXE_LDFLAGS) -o shptree

shptreevis: $(LIBMAP_STATIC) shptreevis.o mapserver.h
	$(LD) $(CFLAGS) shptreevis.o $(EXE_LDFLAGS) -o shptreevis

shptreetst: $(LIBMAP_STATIC) shptreetst.o mapserver.h
	$(LD) $(CFLAGS) shptreetst.o $(EXE_LDFLAGS) -o shptreetst

sortshp: $(LIBMAP_STATIC) sortshp.o mapserver.h
	$(LD) $(CFLAGS) sortshp.o $(EXE_LDFLAGS) -o sortshp

tile4ms: $(LIBMAP_STATIC) tile4ms.o mapserver.h
	$(LD) $(CFLAGS) tile4ms.o $(EXE_LDFLAGS) -o tile4ms

msencrypt: $(LIBMAP_STATIC) msencrypt.o mapserver.h
	$(LD) $(CFLAGS) msencrypt.o $(EXE_LDFLAGS) -o msencrypt

testexpr: $(LIBMAP_STATIC) testexpr.o mapparser.o maplexer.o mapserver.h
	$(LD) $(CFLAGS) testexpr.o $(EXE_LDFLAGS) -o testexpr

testcopy: $(LIBMAP_STATIC) testcopy.o mapcopy.o mapserver.h
	$(LD) $(CFLAGS) testcopy.o $(EXE_LDFLAGS) -o testcopy

test_mapcrypto: $(LIBMAP_STATIC) mapcrypto.c mapserver.h
	$(CC) $(CFLAGS) mapcrypto.c -DTEST_MAPCRYPTO $(EXE_LDFLAGS) -o test_mapcrypto

mapscriptvars:	Makefile
	touch mapscriptvars
	pwd > mapscriptvars
	echo $(IGNORE_MISSING_DATA) $(USE_POINT_Z_M) $(STRINGS)                  -DUSE_WMS_SVR        -DUSE_PROJ -DUSE_AGG_SVG_SYMBOLS    -DUSE_GD_GIF -DUSE_GD_PNG -DUSE_GD_JPEG -DUSE_GD_WBMP -DUSE_GD_FT -DGD_HAS_FTEX_XSHOW -DGD_HAS_GDIMAGEGIFPTR -DGD_HAS_GETBITMAPFONTS -DGD_HAS_GET_TRUE_COLOR_PIXEL -DUSE_ICONV -DUSE_GIF -DUSE_PNG -DUSE_ZLIB -DUSE_FREETYPE  >> mapscriptvars
	echo -I. $(PROJ_INC) $(GD_INC) $(TTF_INC) $(JPEG_INC) $(SDE_INC) $(OGR_INC) $(GDAL_INC) $(GEOS_INC) >> mapscriptvars
	echo $(EXE_LDFLAGS) >> mapscriptvars
	echo $(STATIC_LIBS) >> mapscriptvars
	grep '#define MS_VERSION ' mapserver.h >> mapscriptvars

mapserver-config: Makefile
	rm -f mapserver-config
	echo '#!/bin/sh' > mapserver-config
	echo 'CONFIG_LIBS="$(SUP_LIBS)"' >> mapserver-config
	echo 'CONFIG_DEP_LIBS="$(LIBS)"' >> mapserver-config
	echo 'CONFIG_CFLAGS="$(CFLAGS)"' >> mapserver-config
	echo 'CONFIG_DEFINES="$(DEFINES)"' >> mapserver-config
	echo 'CONFIG_INCLUDES="$(INCLUDES)"' >> mapserver-config
	echo 'CONFIG_VERSION="'`grep '#define MS_VERSION ' mapserver.h | sed 's/\"//g' | sed 's/#define MS_VERSION //'`'"' >> mapserver-config
	cat mapserver-config.in >> mapserver-config
	chmod a+x mapserver-config

php_mapscript_clean::
	cd mapscript/php; $(MAKE) clean; cd ../..

install:
	@echo ""
	@echo "***** MapServer Installation *****"
	@echo "To install MapServer, copy the 'mapserv' file to your web server's cgi-bin "
	@echo "directory."
	@echo "If you use MapScript then see the documentation for your specific MapScript"
	@echo "version for installation instructions."
	@echo ""

install-force:	all
	cp $(EXE_LIST) $(INST_BIN)
	if test -x $(LIBMAP_SHARED) ; then \
	  cp $(LIBMAP_SH_VER) $(INST_LIB) ; \
	  (cd $(INST_LIB) ; ln -f -s $(LIBMAP_SH_VER) $(LIBMAP_SHARED) ) ; \
	fi

uninstall:
	(cd $(INST_BIN) && rm $(EXE_LIST) )
	(cd $(INST_LIB) && rm $(LIBMAP_SH_VER) $(LIBMAP_SHARED) )

clean: 
	rm -f $(LIBMAP_STATIC) $(LIBMAP_SHARED) $(LIBMAP_SH_VER) *.o $(EXE_LIST) renderers/agg/src/*.o

exe-clean:
	rm -f $(EXE_LIST)

distclean:
	$(MAKE) clean
	rm config.*
	if test -d autom4te.cache ; then \
	  rm -f -r autom4te.cache ; \
	fi

sorta-clean:
	rm -f *.o
