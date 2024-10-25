--- x.c.orig	2022-10-04 17:41:26 UTC
+++ x.c
@@ -45,6 +45,11 @@ typedef struct {
 	signed char appcursor; /* application cursor */
 } Key;
 
+typedef enum {
+	PixelGeometry,
+	CellGeometry
+} Geometry;
+
 /* X modifiers */
 #define XK_ANY_MOD    UINT_MAX
 #define XK_NO_MOD     0
@@ -105,6 +110,7 @@ typedef struct {
 	XSetWindowAttributes attrs;
 	int scr;
 	int isfixed; /* is fixed geometry? */
+	int depth; /* bit depth */
 	int l, t; /* left and top offset */
 	int gm; /* geometry mask */
 } XWindow;
@@ -243,6 +249,7 @@ static double defaultfontsize = 0;
 static double usedfontsize = 0;
 static double defaultfontsize = 0;
 
+static char *opt_alpha = NULL;
 static char *opt_class = NULL;
 static char **opt_cmd  = NULL;
 static char *opt_embed = NULL;
@@ -253,6 +260,7 @@ static uint buttons; /* bit field of pressed buttons *
 static char *opt_title = NULL;
 
 static uint buttons; /* bit field of pressed buttons */
+static int cursorblinks = 0;
 
 void
 clipcopy(const Arg *dummy)
@@ -752,7 +760,7 @@ xresize(int col, int row)
 
 	XFreePixmap(xw.dpy, xw.buf);
 	xw.buf = XCreatePixmap(xw.dpy, xw.win, win.w, win.h,
-			DefaultDepth(xw.dpy, xw.scr));
+			xw.depth);
 	XftDrawChange(xw.draw, xw.buf);
 	xclear(0, 0, win.w, win.h);
 
@@ -812,6 +820,13 @@ xloadcols(void)
 			else
 				die("could not allocate color %d\n", i);
 		}
+
+	/* set alpha value of bg color */
+	if (opt_alpha)
+		alpha = strtof(opt_alpha, NULL);
+	dc.col[defaultbg].color.alpha = (unsigned short)(0xffff * alpha);
+	dc.col[defaultbg].pixel &= 0x00FFFFFF;
+	dc.col[defaultbg].pixel |= (unsigned char)(0xff * alpha) << 24;
 	loaded = 1;
 }
 
@@ -1127,19 +1142,31 @@ void
 }
 
 void
-xinit(int cols, int rows)
+xinit(int w, int h)
 {
 	XGCValues gcvalues;
 	Cursor cursor;
 	Window parent;
 	pid_t thispid = getpid();
 	XColor xmousefg, xmousebg;
+	XWindowAttributes attr;
+	XVisualInfo vis;
 
 	if (!(xw.dpy = XOpenDisplay(NULL)))
 		die("can't open display\n");
 	xw.scr = XDefaultScreen(xw.dpy);
-	xw.vis = XDefaultVisual(xw.dpy, xw.scr);
 
+	if (!(opt_embed && (parent = strtol(opt_embed, NULL, 0)))) {
+		parent = XRootWindow(xw.dpy, xw.scr);
+		xw.depth = 32;
+	} else {
+		XGetWindowAttributes(xw.dpy, parent, &attr);
+		xw.depth = attr.depth;
+	}
+
+	XMatchVisualInfo(xw.dpy, xw.scr, xw.depth, TrueColor, &vis);
+	xw.vis = vis.visual;
+
 	/* font */
 	if (!FcInit())
 		die("could not init fontconfig.\n");
@@ -1148,12 +1175,20 @@ xinit(int cols, int rows)
 	xloadfonts(usedfont, 0);
 
 	/* colors */
-	xw.cmap = XDefaultColormap(xw.dpy, xw.scr);
+	xw.cmap = XCreateColormap(xw.dpy, parent, xw.vis, None);
 	xloadcols();
 
 	/* adjust fixed window geometry */
-	win.w = 2 * borderpx + cols * win.cw;
-	win.h = 2 * borderpx + rows * win.ch;
+	switch (geometry) {
+	case CellGeometry:
+		win.w = 2 * borderpx + w * win.cw;
+		win.h = 2 * borderpx + h * win.ch;
+		break;
+	case PixelGeometry:
+		win.w = w;
+		win.h = h;
+		break;
+	}
 	if (xw.gm & XNegative)
 		xw.l += DisplayWidth(xw.dpy, xw.scr) - win.w - 2;
 	if (xw.gm & YNegative)
@@ -1168,19 +1203,15 @@ xinit(int cols, int rows)
 		| ButtonMotionMask | ButtonPressMask | ButtonReleaseMask;
 	xw.attrs.colormap = xw.cmap;
 
-	if (!(opt_embed && (parent = strtol(opt_embed, NULL, 0))))
-		parent = XRootWindow(xw.dpy, xw.scr);
 	xw.win = XCreateWindow(xw.dpy, parent, xw.l, xw.t,
-			win.w, win.h, 0, XDefaultDepth(xw.dpy, xw.scr), InputOutput,
+			win.w, win.h, 0, xw.depth, InputOutput,
 			xw.vis, CWBackPixel | CWBorderPixel | CWBitGravity
 			| CWEventMask | CWColormap, &xw.attrs);
 
 	memset(&gcvalues, 0, sizeof(gcvalues));
 	gcvalues.graphics_exposures = False;
-	dc.gc = XCreateGC(xw.dpy, parent, GCGraphicsExposures,
-			&gcvalues);
-	xw.buf = XCreatePixmap(xw.dpy, xw.win, win.w, win.h,
-			DefaultDepth(xw.dpy, xw.scr));
+	xw.buf = XCreatePixmap(xw.dpy, xw.win, win.w, win.h, xw.depth);
+	dc.gc = XCreateGC(xw.dpy, xw.buf, GCGraphicsExposures, &gcvalues);
 	XSetForeground(xw.dpy, dc.gc, dc.col[defaultbg].pixel);
 	XFillRectangle(xw.dpy, xw.buf, dc.gc, 0, 0, win.w, win.h);
 
@@ -1558,29 +1589,44 @@ xdrawcursor(int cx, int cy, Glyph g, int ox, int oy, G
 	/* draw the new one */
 	if (IS_SET(MODE_FOCUSED)) {
 		switch (win.cursor) {
-		case 7: /* st extension */
-			g.u = 0x2603; /* snowman (U+2603) */
+		default:
+		case 0: /* blinking block */
+		case 1: /* blinking block (default) */
+			if (IS_SET(MODE_BLINK))
+				break;
 			/* FALLTHROUGH */
-		case 0: /* Blinking Block */
-		case 1: /* Blinking Block (Default) */
-		case 2: /* Steady Block */
+		case 2: /* steady block */
 			xdrawglyph(g, cx, cy);
 			break;
-		case 3: /* Blinking Underline */
-		case 4: /* Steady Underline */
+		case 3: /* blinking underline */
+			if (IS_SET(MODE_BLINK))
+				break;
+			/* FALLTHROUGH */
+		case 4: /* steady underline */
 			XftDrawRect(xw.draw, &drawcol,
 					borderpx + cx * win.cw,
 					borderpx + (cy + 1) * win.ch - \
 						cursorthickness,
 					win.cw, cursorthickness);
 			break;
-		case 5: /* Blinking bar */
-		case 6: /* Steady bar */
+		case 5: /* blinking bar */
+			if (IS_SET(MODE_BLINK))
+				break;
+			/* FALLTHROUGH */
+		case 6: /* steady bar */
 			XftDrawRect(xw.draw, &drawcol,
 					borderpx + cx * win.cw,
 					borderpx + cy * win.ch,
 					cursorthickness, win.ch);
 			break;
+		case 7: /* blinking st cursor */
+			if (IS_SET(MODE_BLINK))
+				break;
+			/* FALLTHROUGH */
+		case 8: /* steady st cursor */
+			g.u = stcursor;
+			xdrawglyph(g, cx, cy);
+			break;
 		}
 	} else {
 		XftDrawRect(xw.draw, &drawcol,
@@ -1737,9 +1783,12 @@ xsetcursor(int cursor)
 int
 xsetcursor(int cursor)
 {
-	if (!BETWEEN(cursor, 0, 7)) /* 7: st extension */
+	if (!BETWEEN(cursor, 0, 8)) /* 7-8: st extensions */
 		return 1;
 	win.cursor = cursor;
+	cursorblinks = win.cursor == 0 || win.cursor == 1 ||
+	               win.cursor == 3 || win.cursor == 5 ||
+	               win.cursor == 7;
 	return 0;
 }
 
@@ -1983,6 +2032,10 @@ run(void)
 		if (FD_ISSET(ttyfd, &rfd) || xev) {
 			if (!drawing) {
 				trigger = now;
+				if (IS_SET(MODE_BLINK)) {
+					win.mode ^= MODE_BLINK;
+				}
+				lastblink = now;
 				drawing = 1;
 			}
 			timeout = (maxlatency - TIMEDIFF(now, trigger)) \
@@ -1993,7 +2046,7 @@ run(void)
 
 		/* idle detected or maxlatency exhausted -> draw */
 		timeout = -1;
-		if (blinktimeout && tattrset(ATTR_BLINK)) {
+		if (blinktimeout && (cursorblinks || tattrset(ATTR_BLINK))) {
 			timeout = blinktimeout - TIMEDIFF(now, lastblink);
 			if (timeout <= 0) {
 				if (-timeout > blinktimeout) /* start visible */
@@ -2029,12 +2082,15 @@ main(int argc, char *argv[])
 {
 	xw.l = xw.t = 0;
 	xw.isfixed = False;
-	xsetcursor(cursorshape);
+	xsetcursor(cursorstyle);
 
 	ARGBEGIN {
 	case 'a':
 		allowaltscreen = 0;
 		break;
+	case 'A':
+		opt_alpha = EARGF(usage());
+		break;
 	case 'c':
 		opt_class = EARGF(usage());
 		break;
@@ -2048,7 +2104,13 @@ main(int argc, char *argv[])
 	case 'g':
 		xw.gm = XParseGeometry(EARGF(usage()),
 				&xw.l, &xw.t, &cols, &rows);
+		geometry = CellGeometry;
 		break;
+	case 'G':
+		xw.gm = XParseGeometry(EARGF(usage()),
+				&xw.l, &xw.t, &width, &height);
+		geometry = PixelGeometry;
+		break;
 	case 'i':
 		xw.isfixed = 1;
 		break;
@@ -2084,10 +2146,19 @@ run:
 
 	setlocale(LC_CTYPE, "");
 	XSetLocaleModifiers("");
+	switch (geometry) {
+	case CellGeometry:
+		xinit(cols, rows);
+		break;
+	case PixelGeometry:
+		xinit(width, height);
+		cols = (win.w - 2 * borderpx) / win.cw;
+		rows = (win.h - 2 * borderpx) / win.ch;
+		break;
+	}
 	cols = MAX(cols, 1);
 	rows = MAX(rows, 1);
 	tnew(cols, rows);
-	xinit(cols, rows);
 	xsetenv();
 	selinit();
 	run();
