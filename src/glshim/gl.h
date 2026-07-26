#ifndef GLSHIM_GL_H
#define GLSHIM_GL_H

/* SGI GL API - shim for Win32+OpenGL
 * NOTE: In C89, empty parens means "any params" - we use this to accept
 * whatever glmodule.c passes without fighting API mismatches. */

/* Basic types */
typedef short Device;

/* Constants */
#define TRUE  1
#define FALSE 0
#define NORMALDRAW 0
#define PUPDRAW 1
#define OVERDRAW 2
#define UNDERDRAW 3
#define FLAT  0
#define GOURAUD 1
#define MSINGLE 0
#define MPROJECTION 1
#define MVIEWING 2

#define LMNULL 0.0
#define DEFMATERIAL 0
#define EMISSION 1
#define AMBIENT 2
#define DIFFUSE 3
#define SPECULAR 4
#define SHININESS 5
#define ALPHA 7
#define DEFLIGHT 100
#define LCOLOR 101
#define POSITION 102
#define DEFLMODEL 200
#define LOCALVIEWER 201
#define ATTENUATION 202
#define MATERIAL 1000
#define LIGHT0 1100
#define LIGHT1 1101
#define LIGHT2 1102
#define LIGHT3 1103
#define LIGHT4 1104
#define LIGHT5 1105
#define LIGHT6 1106
#define LIGHT7 1107
#define LMODEL 1200

#define GD_XPMAX 0
#define GD_YPMAX 1
#define GD_XMMAX 2
#define GD_YMMAX 3
#define GD_ZMIN 4
#define GD_ZMAX 5
#define GD_BITS_NORM_SNG_RED 6
#define GD_BITS_NORM_DBL_RED 9
#define GD_BITS_NORM_ZBUFFER 16
#define GD_NMMAPS 63
#define GD_TIMERHZ 66

#define N_PIXEL_TOLERANCE 1
#define N_CULLING 2
#define N_DISPLAY 3
#define N_ERRORCHECKING 4
#define N_SUBDIVISIONS 5
#define N_S_STEPS 6
#define N_T_STEPS 7
#define N_TILES 8
#define N_XYZ 0x4c
#define N_XYZW 0x51
#define N_WXYZ 0x53
#define N_STW 0x0d
#define N_ST 0x08

/* All function declarations using empty-param syntax for compatibility */
void foreground();
void keepaspect();
void prefposition();
long winopen();
void winconstraints();
void noborder();
void minsize();
void maxsize();
void iconsize();
void wintitle();
void winclose();
long winget();
void winset();
void winmove();
void winposition();
void prefsize();
void reshapeviewport();
void getorigin();
void getsize();
long getplanes();
long getgdesc();
void getcpos();
void getviewport();
void gbegin();
void gconfig();
void greset();
void ginit();

void bgnpolygon();
void endpolygon();
void bgnline();
void endline();
void bgnpoint();
void endpoint();
void bgnclosedline();
void endclosedline();
void bgnsurface();
void endsurface();
void bgntmesh();
void endtmesh();
void bgntrim();
void endtrim();

void v2f();
void v2i();
void v2s();
void v3f();
void v3i();
void v3s();
void v4f();
void v4i();
void v4s();
void v2d();
void v3d();
void v4d();
void n3f();
void normal();

void move();
void move2();
void draw();
void draw2();
void pnt();
void pnt2();
void cmov();
void cmov2();

void rect();
void rectf();
void recti();
void rects();
void rectfi();
void rectfs();

void circ();
void circf();
void circi();
void circs();
void arcf();
void arc();
void arci();
void arcs();
void arcfi();
void arcfs();

void sbox();
void sboxf();
void sboxfi();
void sboxfs();
void sboxi();
void sboxs();

void clear();
void zclear();
void swapbuffers();

void color();
void colorf();
void c3i();
void c3f();
void c3s();
void c4i();
void c4f();
void c4s();
void RGBcolor();
void RGBmode();
void cpack();
void mapcolor();

void pushmatrix();
void popmatrix();
void loadmatrix();
void multmatrix();
void rotate();
void rot();
void scale();
void translate();
void ortho();
void ortho2();
void perspective();
void window();
void lookat();
void polarview();

void zbuffer();
void zfunction();
void zsource();
void zdraw();
void zwritemask();
void doublebuffer();
void singlebuffer();
void frontbuffer();
void backbuffer();
void lsetdepth();

void lmdef();
void lmbind();
void lmcolor();
void shademodel();
void lshaderange();

void nurbssurface();
void nurbscurve();
void pwlcurve();
void setnurbsproperty();
void getnurbsproperty();

void charstr();
void font();
long strwidth();
void textport();
long getheight();
long getfont();

void qdevice();
void unqdevice();
long qread();
long qtest();
long getbutton();
long getvaluator();
void noise();

void initnames();
void pushname();
void popname();
void loadname();
long genobj();
long gentag();
int isobj();
int istag();
void makeobj();
void callobj();
void closeobj();
void delobj();
void deltag();
void editobj();
void objinsert();
void objreplace();
void objdelete();
long endpick();
long endselect();
void pick();
void gselect();
void picksize();

void popattributes();
void pushattributes();
void winpush();
void winpop();

void overlay();
void underlay();

long getplanes();
long getdisplaymode();
void getdepth();
long getwritemask();
long getcolor();
void setdepth();
void cursor();
void curorigin();
void defcursor();
void setcursor();
void cursoff();
void curson();
long getlstyle();
long getlwidth();
long getmap();
void setmap();
void multimap();
void onemap();
long getcmmode();
long getmonitor();
void setmonitor();
void getmcolor();
void setbell();
void ringbell();
void blankscreen();
void blanktime();

long newpup();
void addtopup();
long dopup();
void freepup();

long getbuffer();
long getdrawmode();
long getshade();
long getvideo();

void swapinterval();

void rectcopy();
void rectzoom();
void readsource();
void blendfunction();
void logicop();
void subpixel();
void smoothline();
void pntsmooth();
void linesmooth();
void swaptmesh();
void gammaramp();
void RGBrange();
void lRGBrange();
void czclear();
void shaderange();
void setshade();
void setvideo();
long getsm();
long getmmode();
long getbackface();
long getdescender();

void viewport();
void popviewport();
void pushviewport();
void screenspace();

void defbasis();
void curvebasis();
void curveit();
void curveprecision();

void gsync();
void tpon();
void tpoff();
void clkon();
void clkoff();
void qreset();
void qenter();
long getbutton();
long getvaluator();

void setlinestyle();
void linewidth();
void setpattern();
long getpattern();
void deflinestyle();

void move2i();
void move2s();
void draw2i();
void draw2s();
void pnt2i();
void pnt2s();
void cmov2i();
void cmov2s();
void movei();
void moves();
void drawi();
void draws();
void pnti();
void pnts();
void cmovi();
void cmovs();

void curson();
void cursoff();

void backface();
void concave();
void depthcue();
void cmode();



void getmatrix();


long nvarray();
long varray();

long dglopen();
void dglclose();
void glcompat();

void gettp();
void getgpos();
void getport();
void noport();
void gRGBcolor();
void gRGBmask();
void getscrmask();
void scrmask();

void lmpack();
void setpup();
void pupmode();
void endpupmode();
void endfullscrn();
void fullscrn();
void attachcursor();
void tie();

void poly();
void poly2();
void polf();
void polf2();
void polyi();
void poly2i();
void polfi();
void polf2i();
void polys();
void poly2s();
void polfs();
void polf2s();

void pdr(); void pdr2(); void pdri(); void pdrs();
void pdr2i(); void pdr2s();
void pmv(); void pmv2(); void pmvi(); void pmvs();
void pmv2i(); void pmv2s();
void rdr(); void rdr2(); void rdri(); void rdrs();
void rdr2i(); void rdr2s();
void rmv(); void rmv2(); void rmvi(); void rmvs();
void rmv2i(); void rmv2s();
void rpdr(); void rpdr2(); void rpdri(); void rpdrs();
void rpdr2i(); void rpdr2s();
void rpmv(); void rpmv2(); void rpmvi(); void rpmvs();
void rpmv2i(); void rpmv2s();
void xfpt(); void xfpt2(); void xfpti(); void xfpts();
void xfpt2i(); void xfpt2s();
void xfpt4(); void xfpt4i(); void xfpt4s();

void mmode();

void crv(); void crvn(); void rcrv(); void rcrvn();
void patch(); void rpatch();
void patchbasis(); void patchcurves(); void patchprecision();

void defrasterfont();
void getcursor();
void getsize();
void winconstraints();
void keepaspect();
void backbuffer();
void frontbuffer();
void wmpack();
void pagecolor();
void textcolor();
void writemask();
void textwritemask();
void cyclemap();
void curveit();
void depthcue();
void getdepth();
void gRGBcolor();
void gRGBmask();
void scrmask();
void getscrmask();
long getlsbackup();
long getresetls();
void lsbackup();
void lsrepeat();
long getlsrepeat();
void resetls();
long getopenobj();
long getothermonitor();
void lampoff();
void lampon();
void pixmode();

#endif /* GLSHIM_GL_H */
