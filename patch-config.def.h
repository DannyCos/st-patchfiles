--- config.def.h.orig	2022-10-04 17:41:26 UTC
+++ config.def.h
@@ -5,8 +5,8 @@
  *
  * font: see http://freedesktop.org/software/fontconfig/fontconfig-user.html
  */
-static char *font = "Liberation Mono:pixelsize=12:antialias=true:autohint=true";
-static int borderpx = 2;
+static char *font = "Hack:style=Regular:pixelsize=18:antialias=true:autohint=true";
+static int borderpx = 10;
 
 /*
  * What program is execed by st depends of these precedence rules:
@@ -16,7 +16,7 @@ static int borderpx = 2;
  * 4: value of shell in /etc/passwd
  * 5: value of shell in config.h
  */
-static char *shell = "/bin/sh";
+static char *shell = "/bin/tcsh";
 char *utmp = NULL;
 /* scroll program: to enable use a string like "scroll" */
 char *scroll = NULL;
@@ -96,53 +96,64 @@ static const char *colorname[] = {
 /* Terminal colors (16 first used in escape sequence) */
 static const char *colorname[] = {
 	/* 8 normal colors */
-	"black",
-	"red3",
-	"green3",
-	"yellow3",
-	"blue2",
-	"magenta3",
-	"cyan3",
-	"gray90",
+	"#45475A",
+	"#F38BA8",
+	"#A6E3A1",
+	"#F9E2AF",
+	"#89B4FA",
+	"#F5C2E7",
+	"#94E2D5",
+	"#BAC2DE",
 
 	/* 8 bright colors */
-	"gray50",
-	"red",
-	"green",
-	"yellow",
-	"#5c5cff",
-	"magenta",
-	"cyan",
-	"white",
+	"#585B70",
+	"#F38BA8",
+	"#A6E3A1",
+	"#F9E2AF",
+	"#89B4FA",
+	"#F5C2E7",
+	"#94E2D5",
+	"#A6ADC8",
 
-	[255] = 0,
+	[256] = "#CDD6F4", /* default foreground colour */
+	[257] = "#1E1E2E", /* default background colour */
+	[258] = "#F5E0DC", /*575268*/
 
-	/* more colors can be added after 255 to use with DefaultXX */
-	"#cccccc",
-	"#555555",
-	"gray90", /* default foreground colour */
-	"black", /* default background colour */
 };
 
+/*
+ *  * foreground, background, cursor, reverse cursor
+ *   */
+unsigned int defaultfg = 256;
+unsigned int defaultbg = 257;
+unsigned int defaultcs = 258;
+static unsigned int defaultrcs = 258;
 
+/* bg opacity */
+float alpha = 0.93;
+
 /*
- * Default colors (colorname index)
- * foreground, background, cursor, reverse cursor
+ * https://invisible-island.net/xterm/ctlseqs/ctlseqs.html#h4-Functions-using-CSI-_-ordered-by-the-final-character-lparen-s-rparen:CSI-Ps-SP-q.1D81
+ * Default style of cursor
+ * 0: blinking block
+ * 1: blinking block (default)
+ * 2: steady block ("█")
+ * 3: blinking underline
+ * 4: steady underline ("_")
+ * 5: blinking bar
+ * 6: steady bar ("|")
+ * 7: blinking st cursor
+ * 8: steady st cursor
  */
-unsigned int defaultfg = 258;
-unsigned int defaultbg = 259;
-unsigned int defaultcs = 256;
-static unsigned int defaultrcs = 257;
+static unsigned int cursorstyle = 1;
+static Rune stcursor = 0x2603; /* snowman ("☃") */
 
 /*
- * Default shape of cursor
- * 2: Block ("█")
- * 4: Underline ("_")
- * 6: Bar ("|")
- * 7: Snowman ("☃")
+ * Whether to use pixel geometry or cell geometry
  */
-static unsigned int cursorshape = 2;
 
+static Geometry geometry = CellGeometry;
+
 /*
  * Default columns and rows numbers
  */
@@ -151,6 +162,13 @@ static unsigned int rows = 24;
 static unsigned int rows = 24;
 
 /*
+ * Default width and height (including borders!)
+ */
+
+static unsigned int width = 564;
+static unsigned int height = 364;
+
+/*
  * Default colour and shape of the mouse cursor
  */
 static unsigned int mouseshape = XC_xterm;
@@ -184,7 +202,7 @@ static MouseShortcut mshortcuts[] = {
 };
 
 /* Internal keyboard shortcuts. */
-#define MODKEY Mod1Mask
+#define MODKEY Mod4Mask
 #define TERMMOD (ControlMask|ShiftMask)
 
 static Shortcut shortcuts[] = {
@@ -201,6 +219,8 @@ static Shortcut shortcuts[] = {
 	{ TERMMOD,              XK_Y,           selpaste,       {.i =  0} },
 	{ ShiftMask,            XK_Insert,      selpaste,       {.i =  0} },
 	{ TERMMOD,              XK_Num_Lock,    numlock,        {.i =  0} },
+	{ ShiftMask,            XK_Page_Up,     kscrollup,      {.i = -1} },
+	{ ShiftMask,            XK_Page_Down,   kscrolldown,    {.i = -1} },
 };
 
 /*
