--- config.mk.orig	2022-10-04 17:41:26 UTC
+++ config.mk
@@ -5,10 +5,10 @@ PREFIX = /usr/local
 
 # paths
 PREFIX = /usr/local
-MANPREFIX = $(PREFIX)/share/man
+MANPREFIX = /usr/local/man
 
-X11INC = /usr/X11R6/include
-X11LIB = /usr/X11R6/lib
+X11INC = /usr/local/include
+X11LIB = /usr/local/lib
 
 PKG_CONFIG = pkg-config
 
@@ -16,7 +16,7 @@ INCS = -I$(X11INC) \
 INCS = -I$(X11INC) \
        `$(PKG_CONFIG) --cflags fontconfig` \
        `$(PKG_CONFIG) --cflags freetype2`
-LIBS = -L$(X11LIB) -lm -lrt -lX11 -lutil -lXft \
+LIBS = -L$(X11LIB) -lm -lrt -lX11 -lutil -lXft -lXrender\
        `$(PKG_CONFIG) --libs fontconfig` \
        `$(PKG_CONFIG) --libs freetype2`
 
