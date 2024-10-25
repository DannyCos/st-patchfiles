--- st.h.orig	2022-10-04 17:41:26 UTC
+++ st.h
@@ -81,6 +81,8 @@ void draw(void);
 void redraw(void);
 void draw(void);
 
+void kscrolldown(const Arg *);
+void kscrollup(const Arg *);
 void printscreen(const Arg *);
 void printsel(const Arg *);
 void sendbreak(const Arg *);
@@ -124,3 +126,4 @@ extern unsigned int defaultcs;
 extern unsigned int defaultfg;
 extern unsigned int defaultbg;
 extern unsigned int defaultcs;
+extern float alpha;
