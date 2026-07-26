#include <windows.h>
#include <windowsx.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* Tracked GL state */
static float clear_r = 0, clear_g = 0, clear_b = 0;

/* Internal constants (keep separate from gl.h to avoid prototype conflicts) */
#define TRUE  1
#define FALSE 0
#define GOURAUD 1
#define MVIEWING 2
#define MPROJECTION 1
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
#define DEFMATERIAL 0
#define DEFLIGHT 100
#define DEFLMODEL 200
#define EMISSION 1
#define AMBIENT 2
#define DIFFUSE 3
#define SPECULAR 4
#define SHININESS 5
#define LCOLOR 101
#define POSITION 102
#define N_PIXEL_TOLERANCE 1
#define N_ERRORCHECKING 4
#define N_XYZ 0x4c
#define GD_XPMAX 0
#define GD_YPMAX 1
#define GD_XMMAX 2
#define GD_YMMAX 3
#define GD_BITS_NORM_SNG_RED 6
#define GD_BITS_NORM_DBL_RED 9
#define GD_BITS_NORM_ZBUFFER 16
#define GD_NMMAPS 63

typedef short Device;
#include "device.h"

/* ===================================================================
 * Internal data structures
 * =================================================================== */

typedef struct Event {
    Device dev;
    short val;
    struct Event *next;
} Event;

typedef struct GLWindow {
    long wid;
    HWND hwnd;
    HDC hdc;
    HGLRC hglrc;
    int width, height;
    int is_double;
    int rgb_mode;
    int drawmode;
    int foreground;
    Event *queue_head, *queue_tail;
    Device enabled_devices[256];
    int num_enabled;
    struct GLWindow *next;
} GLWindow;

static GLWindow *windows = NULL;
static GLWindow *current_win = NULL;
static long next_wid = 1;
static int gl_initialized = 0;
static const char *WCLASS = "GLShimWindow";

/* ===================================================================
 * Event queue
 * =================================================================== */

static void push_event(Device dev, short val) {
    Event *e = malloc(sizeof(Event));
    if (!e) return;
    e->dev = dev; e->val = val; e->next = NULL;
    if (!current_win) return;
    if (current_win->queue_tail)
        current_win->queue_tail->next = e;
    else
        current_win->queue_head = e;
    current_win->queue_tail = e;
}

static int pop_event(Device *dev, short *val) {
    if (!current_win || !current_win->queue_head) return 0;
    Event *e = current_win->queue_head;
    *dev = e->dev; *val = e->val;
    current_win->queue_head = e->next;
    if (!current_win->queue_head) current_win->queue_tail = NULL;
    free(e);
    return 1;
}

static void pump_messages(void) {
    MSG msg;
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

/* ===================================================================
 * Window Procedure
 * =================================================================== */

static GLWindow *find_window(HWND hwnd) {
    GLWindow *w = windows;
    while (w && w->hwnd != hwnd) w = w->next;
    return w;
}

static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    GLWindow *w = find_window(hwnd);
    if (!w) return DefWindowProc(hwnd, msg, wp, lp);

    switch (msg) {
    case WM_SIZE:
        w->width = LOWORD(lp);
        w->height = HIWORD(lp);
        break;
    case WM_PAINT: {
        PAINTSTRUCT ps;
        BeginPaint(hwnd, &ps);
        if (current_win == w) SwapBuffers(w->hdc);
        EndPaint(hwnd, &ps);
        push_event(REDRAW, 0);
        break;
    }
    case WM_KEYDOWN:
        if (wp == VK_ESCAPE) push_event(ESCKEY, 1);
        else if (wp == VK_RETURN) push_event(RETKEY, 1);
        else if (wp == VK_SPACE) push_event(SPACEKEY, 1);
        else if (wp >= 'A' && wp <= 'Z')
            push_event(wp - 'A' + AKEY, 1);
        else if (wp >= '0' && wp <= '9')
            push_event(wp - '0' + ZEROKEY, 1);
        break;
    case WM_KEYUP:
        if (wp == VK_ESCAPE) push_event(ESCKEY, 0);
        break;
    case WM_LBUTTONDOWN:
        push_event(MOUSE3, 1);
        push_event(MOUSEX, GET_X_LPARAM(lp));
        push_event(MOUSEY, GET_Y_LPARAM(lp));
        SetCapture(hwnd);
        break;
    case WM_LBUTTONUP:
        push_event(MOUSE3, 0);
        push_event(MOUSEX, GET_X_LPARAM(lp));
        push_event(MOUSEY, GET_Y_LPARAM(lp));
        ReleaseCapture();
        break;
    case WM_RBUTTONDOWN:
        push_event(MOUSE1, 1);
        SetCapture(hwnd);
        break;
    case WM_RBUTTONUP:
        push_event(MOUSE1, 0);
        ReleaseCapture();
        break;
    case WM_MBUTTONDOWN:
        push_event(MOUSE2, 1);
        SetCapture(hwnd);
        break;
    case WM_MBUTTONUP:
        push_event(MOUSE2, 0);
        ReleaseCapture();
        break;
    case WM_MOUSEMOVE:
        push_event(MOUSEX, GET_X_LPARAM(lp));
        push_event(MOUSEY, GET_Y_LPARAM(lp));
        break;
    case WM_CLOSE:
        push_event(WINSHUT, 0);
        DestroyWindow(hwnd);
        break;
    case WM_DESTROY:
        w->hwnd = NULL;
        break;
    default:
        return DefWindowProc(hwnd, msg, wp, lp);
    }
    return 0;
}

static void ensure_window_class(void) {
    if (gl_initialized) return;
    WNDCLASSEX wc = {0};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_OWNDC | CS_DBLCLKS;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = WCLASS;
    RegisterClassEx(&wc);
    gl_initialized = 1;
}

/* ===================================================================
 * Window management implementation
 * =================================================================== */

void foreground(void) {
    if (current_win) current_win->foreground = 1;
}

void keepaspect(int x, int y) { /* stub */ }

void prefposition(int x1, int x2, int y1, int y2) {
    /* Not used; set by winopen */
}

long winopen(char *name) {
    ensure_window_class();
    GLWindow *w = calloc(1, sizeof(GLWindow));
    if (!w) return 0;
    w->wid = next_wid++;
    w->width = 512;
    w->height = 512;

    DWORD style = WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
    RECT r = {100, 100, 100 + w->width, 100 + w->height};
    AdjustWindowRect(&r, style, FALSE);

    w->hwnd = CreateWindowEx(0, WCLASS, name ? name : "GL",
        style, r.left, r.top, r.right - r.left, r.bottom - r.top,
        NULL, NULL, GetModuleHandle(NULL), NULL);

    w->hdc = GetDC(w->hwnd);

    PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR), 1,
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
        PFD_TYPE_RGBA, 24, 0,0,0,0,0,0,0,0,0,0,0,0,0,
        16, 0, 0, PFD_MAIN_PLANE, 0,0,0,0
    };
    int pf = ChoosePixelFormat(w->hdc, &pfd);
    SetPixelFormat(w->hdc, pf, &pfd);
    w->hglrc = wglCreateContext(w->hdc);
    w->is_double = 1;
    w->rgb_mode = 1;

    w->next = windows;
    windows = w;
    current_win = w;

    wglMakeCurrent(w->hdc, w->hglrc);
    ShowWindow(w->hwnd, SW_SHOW);

    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    push_event(REDRAW, 0);
    push_event(WINTHAW, 0);
    return w->wid;
}

void winclose(long wid) {
    GLWindow *prev = NULL, *w = windows;
    while (w) {
        if (w->wid == wid) {
            if (prev) prev->next = w->next;
            else windows = w->next;
            if (current_win == w) current_win = windows;
            if (w->hglrc) wglDeleteContext(w->hglrc);
            if (w->hdc) ReleaseDC(w->hwnd, w->hdc);
            if (w->hwnd) DestroyWindow(w->hwnd);
            free(w);
            return;
        }
        prev = w;
        w = w->next;
    }
}

long winget(void) {
    return current_win ? current_win->wid : 0;
}

void winset(long wid) {
    GLWindow *w = windows;
    while (w && w->wid != wid) w = w->next;
    if (w) {
        current_win = w;
        wglMakeCurrent(w->hdc, w->hglrc);
    }
}

void winconstraints(void) {}
void noborder(void) {}
void minsize(int w, int h) {}
void maxsize(int w, int h) {}
void iconsize(int w, int h) {}
void wintitle(char *s) {
    if (current_win && current_win->hwnd)
        SetWindowText(current_win->hwnd, s);
}

void getorigin(int *x, int *y) {
    if (!current_win) { *x = 0; *y = 0; return; }
    RECT r;
    GetClientRect(current_win->hwnd, &r);
    POINT p = {r.left, r.top};
    ClientToScreen(current_win->hwnd, &p);
    *x = p.x; *y = p.y;
}

void getsize(int *w, int *h) {
    if (!current_win) { *w = 512; *h = 512; return; }
    RECT r;
    GetClientRect(current_win->hwnd, &r);
    *w = r.right - r.left;
    *h = r.bottom - r.top;
}

void reshapeviewport(void) {
    if (!current_win) return;
    RECT r;
    GetClientRect(current_win->hwnd, &r);
    int w = r.right - r.left, h = r.bottom - r.top;
    if (w < 1) w = 1; if (h < 1) h = 1;
    glViewport(0, 0, w, h);
    current_win->width = w;
    current_win->height = h;
}

long getplanes(void) {
    if (!current_win) return 24;
    HDC hdc = current_win->hdc;
    return GetDeviceCaps(hdc, BITSPIXEL) * GetDeviceCaps(hdc, PLANES);
}

long getgdesc(int which) {
    switch (which) {
    case GD_XPMAX: return 1279;
    case GD_YPMAX: return 1023;
    case GD_XMMAX: return 1023;
    case GD_YMMAX: return 767;
    case GD_BITS_NORM_SNG_RED:
    case GD_BITS_NORM_DBL_RED: return 8;
    case GD_BITS_NORM_ZBUFFER: return 24;
    case GD_NMMAPS: return 1;
    default: return 0;
    }
}

void gconfig(void) {}
void greset(void) {}
void ginit(void) { ensure_window_class(); }
void getcpos(int *x, int *y) { *x = 0; *y = 0; }
void getviewport(int *l, int *b, int *r, int *t) {
    if (!current_win) { *l=0;*b=0;*r=512;*t=512; return; }
    GLint vp[4];
    glGetIntegerv(GL_VIEWPORT, vp);
    *l = vp[0]; *b = vp[1]; *r = vp[0]+vp[2]; *t = vp[1]+vp[3];
}

/* ===================================================================
 * Drawing primitives
 * =================================================================== */

void bgnpolygon(void) { glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); glBegin(GL_POLYGON); }
void endpolygon(void) { glEnd(); }
void bgnline(void) { glBegin(GL_LINE_STRIP); }
void endline(void) { glEnd(); }
void bgnpoint(void) { glBegin(GL_POINTS); }
void endpoint(void) { glEnd(); }
void bgnclosedline(void) { glBegin(GL_LINE_LOOP); }
void endclosedline(void) { glEnd(); }
void bgnsurface(void) { /* NURBS handled separately */ }
void endsurface(void) {}
void bgntmesh(void) { glBegin(GL_TRIANGLE_STRIP); }
void endtmesh(void) { glEnd(); }
void bgntrim(void) { }
void endtrim(void) { }

void v2f(float *v) { glVertex2fv(v); }
void v2i(long *v) { glVertex2i((int)v[0], (int)v[1]); }
void v2s(short *v) { glVertex2s(v[0], v[1]); }
void v3f(float *v) { glVertex3fv(v); }
void v3i(long *v) { glVertex3i((int)v[0], (int)v[1], (int)v[2]); }
void v3s(short *v) { glVertex3s(v[0], v[1], v[2]); }
void v4f(float *v) { glVertex4fv(v); }
void v4i(long *v) { glVertex4i((int)v[0], (int)v[1], (int)v[2], (int)v[3]); }
void v4s(short *v) { glVertex4s(v[0], v[1], v[2], v[3]); }
void v2d(double *v) { glVertex2dv(v); }
void v3d(double *v) { glVertex3dv(v); }
void v4d(double *v) { glVertex4dv(v); }
void n3f(float *v) { glNormal3fv(v); }
void normal(float *v) { glNormal3fv(v); }

void clear(void) { glClearColor(clear_r, clear_g, clear_b, 1); glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); }
void zclear(void) { glClear(GL_DEPTH_BUFFER_BIT); }

void swapbuffers(void) {
    HDC hdc = wglGetCurrentDC();
    if (hdc) SwapBuffers(hdc);
}

void rect(int x1, int y1, int x2, int y2) {
    glRecti(x1, y1, x2, y2);
}
void rectf(float x1, float y1, float x2, float y2) {
    glRectf(x1, y1, x2, y2);
}
void recti(long x1, long y1, long x2, long y2) {
    glRecti((int)x1, (int)y1, (int)x2, (int)y2);
}
void rects(short x1, short y1, short x2, short y2) {
    glRecti(x1, y1, x2, y2);
}
void rectfi(long x1, long y1, long x2, long y2) {
    glRecti((int)x1, (int)y1, (int)x2, (int)y2);
}
void rectfs(short x1, short y1, short x2, short y2) {
    glRecti(x1, y1, x2, y2);
}

void circ(float x, float y, float r) { /* stub */ }
void circf(float x, float y, float r) {
    int segs = 40;
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for (int i = 0; i <= segs; i++) {
        float a = 2 * (float)3.14159 * i / segs;
        glVertex2f(x + r * (float)cos(a), y + r * (float)sin(a));
    }
    glEnd();
}
void circi(long x, long y, long r) { circf((float)x, (float)y, (float)r); }
void circs(short x, short y, short r) { circf((float)x, (float)y, (float)r); }

void arcf(float x, float y, float r, int start, int end) {
    int segs = 40;
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    float sa = 2 * (float)3.14159 * start / 3600.0f;
    float ea = 2 * (float)3.14159 * end / 3600.0f;
    int ns = segs * abs(end - start) / 3600;
    if (ns < 1) ns = 1;
    for (int i = 0; i <= ns; i++) {
        float a = sa + (ea - sa) * i / ns;
        glVertex2f(x + r * (float)cos(a), y + r * (float)sin(a));
    }
    glEnd();
}
void arc(float x, float y, float r, int start, int end) {
    int segs = 40;
    glBegin(GL_LINE_STRIP);
    float sa = 2 * (float)3.14159 * start / 3600.0f;
    float ea = 2 * (float)3.14159 * end / 3600.0f;
    int ns = segs * abs(end - start) / 3600;
    if (ns < 1) ns = 1;
    for (int i = 0; i <= ns; i++) {
        float a = sa + (ea - sa) * i / ns;
        glVertex2f(x + r * (float)cos(a), y + r * (float)sin(a));
    }
    glEnd();
}

void sbox(float x1, float y1, float x2, float y2) { glRectf(x1, y1, x2, y2); }
void sboxf(float x1, float y1, float x2, float y2) { glRectf(x1, y1, x2, y2); }
void sboxi(long x1, long y1, long x2, long y2) { glRecti((int)x1,(int)y1,(int)x2,(int)y2); }
void sboxs(short x1, short y1, short x2, short y2) { glRecti(x1, y1, x2, y2); }

void move(int x, int y) { glBegin(GL_POINTS); glVertex2i(x, y); glEnd(); }
void move2(int x, int y) { glBegin(GL_POINTS); glVertex2i(x, y); glEnd(); }
void draw(int x, int y) { glBegin(GL_LINES); glVertex2i(0,0); glVertex2i(x, y); glEnd(); }
void draw2(int x, int y) { glBegin(GL_LINES); glVertex2i(0,0); glVertex2i(x, y); glEnd(); }

void cmov(int x, int y) { glRasterPos2i(x, y); }
void cmov2(float x, float y) { glRasterPos2f(x, y); }
void cmov2i(long x, long y) { glRasterPos2i((int)x, (int)y); }
void cmov2s(short x, short y) { glRasterPos2i(x, y); }

/* ===================================================================
 * Color
 * =================================================================== */

static float col_map[3] = {1,1,1};

void color(int i) {
    /* For colormap mode, try mapping */
    glColor3f(i/255.0f, i/255.0f, i/255.0f);
}
void colorf(float c) {
    glColor3f(c, c, c);
}
void c3i(long *rgb) {
    glColor3ub((unsigned char)rgb[0], (unsigned char)rgb[1], (unsigned char)rgb[2]);
    clear_r=rgb[0]/255.0f; clear_g=rgb[1]/255.0f; clear_b=rgb[2]/255.0f;
}
void c3f(float *rgb) { glColor3fv(rgb); }
void c3s(short *rgb) { glColor3s(rgb[0], rgb[1], rgb[2]); }
void c4i(long *rgb) { glColor4ub((unsigned char)rgb[0],(unsigned char)rgb[1],(unsigned char)rgb[2],(unsigned char)rgb[3]); }
void c4f(float *rgb) { glColor4fv(rgb); }
void c4s(short *rgb) { glColor4s(rgb[0],rgb[1],rgb[2],rgb[3]); }
void RGBcolor(int r, int g, int b) { glColor3ub(r,g,b); clear_r=r/255.0f; clear_g=g/255.0f; clear_b=b/255.0f; }
void RGBmode(void) {}
void cpack(long rgba) { unsigned char r=(rgba>>16)&0xff, g=(rgba>>8)&0xff, b=rgba&0xff; glColor3ub(r,g,b); clear_r=r/255.0f; clear_g=g/255.0f; clear_b=b/255.0f; }
void mapcolor(int i, int r, int g, int b) { /* stub, colormap mode not supported */ }

/* ===================================================================
 * Matrix
 * =================================================================== */

static int matrix_mode = MVIEWING;

void pushmatrix(void) { glPushMatrix(); }
void popmatrix(void) { glPopMatrix(); }
void loadmatrix(float *m) { glLoadMatrixf(m); }
void multmatrix(float *m) { glMultMatrixf(m); }

void rotate(float angle, char axis) {
    if (current_win) glRotatef(angle, axis=='x'||axis=='X' ? 1 : 0, axis=='y'||axis=='Y' ? 1 : 0, axis=='z'||axis=='Z' ? 1 : 0);
}
void rot(float angle, char axis) { rotate(angle, axis); }

void scale(float x, float y, float z) { glScalef(x, y, z); }
void translate(float x, float y, float z) { glTranslatef(x, y, z); }

void ortho(float l, float r, float b, float t, float n, float f) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(l, r, b, t, n, f);
    if (matrix_mode == MVIEWING) glMatrixMode(GL_MODELVIEW);
}
void ortho2(float l, float r, float b, float t) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(l, r, b, t, -1.0, 1.0);
    if (matrix_mode == MVIEWING) glMatrixMode(GL_MODELVIEW);
}
void perspective(long fov, float aspect, float nearval, float farval) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fov / 10.0, aspect, nearval, farval);
    if (matrix_mode == MVIEWING) glMatrixMode(GL_MODELVIEW);
}
void window(float l, float r, float b, float t, float n, float f) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(l, r, b, t, n, f);
    if (matrix_mode == MVIEWING) glMatrixMode(GL_MODELVIEW);
}
void lookat(float x, float y, float z, float cx, float cy, float cz, int twist) {
    gluLookAt(x, y, z, cx, cy, cz, 0, 1, 0);
}
void polarview(float dist, int az, int inc, int twist) { /* stub */ }

/* ===================================================================
 * Depth & Buffers
 * =================================================================== */

void zbuffer(int on) {
    if (on) glEnable(GL_DEPTH_TEST);
    else glDisable(GL_DEPTH_TEST);
}
void zfunction(int mode) {
    static const GLuint zf[] = {GL_NEVER, GL_LESS, GL_EQUAL, GL_LEQUAL,
        GL_GREATER, GL_NOTEQUAL, GL_GEQUAL, GL_ALWAYS};
    if (mode >= 0 && mode <= 7) glDepthFunc(zf[mode]);
}
void zsource(int) {}
void zdraw(int) {}
void zwritemask(long) {}
void doublebuffer(void) { if (current_win) current_win->is_double = 1; }
void singlebuffer(void) { if (current_win) current_win->is_double = 0; }
void frontbuffer(int on) {
    if (on) glDrawBuffer(GL_FRONT);
    else glDrawBuffer(GL_BACK);
}
void backbuffer(int on) {
    glDrawBuffer(on ? GL_BACK : GL_FRONT);
}
void lsetdepth(long n, long f) {
    glDepthRange(n / 8388607.0, f / 8388607.0);
}

/* ===================================================================
 * Lighting / Material
 * =================================================================== */

#define MAX_PROPS 64
static float mat_props[9][MAX_PROPS];  /* up to 9 material defs */
static float light_props[9][MAX_PROPS]; /* up to 9 light defs */
static float lmodel_props[9][MAX_PROPS]; /* up to 9 lmodel defs */

static void apply_material(float *p) {
    if (!p) { glDisable(GL_LIGHTING); return; }
    glEnable(GL_LIGHTING);
    float amb[]={0.2f,0.2f,0.2f,1.0f}, dif[]={0.8f,0.8f,0.8f,1.0f};
    float spec[]={0,0,0,1}, emit[]={0,0,0,1}, shine=0;
    int i=0;
    while (p[i] != 0.0f && i < MAX_PROPS-1) {
        int prop = (int)p[i++];
        switch (prop) {
        case EMISSION: emit[0]=p[i]; emit[1]=p[i+1]; emit[2]=p[i+2]; i+=3; break;
        case AMBIENT:  amb[0]=p[i]; amb[1]=p[i+1]; amb[2]=p[i+2]; i+=3; break;
        case DIFFUSE:  dif[0]=p[i]; dif[1]=p[i+1]; dif[2]=p[i+2]; i+=3; break;
        case SPECULAR: spec[0]=p[i]; spec[1]=p[i+1]; spec[2]=p[i+2]; i+=3; break;
        case SHININESS: shine=p[i]; i+=1; break;
        default: i++; break;
        }
    }
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, amb);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, dif);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emit);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shine);
}

static void apply_light(float *p, int light) {
    if (!p) { glDisable(GL_LIGHT0 + light); return; }
    glEnable(GL_LIGHT0 + light);
    float col[]={1,1,1,1}, pos[]={0,0,1,0};
    int i=0;
    while (p[i] != 0.0f && i < MAX_PROPS-1) {
        int prop = (int)p[i++];
        switch (prop) {
        case LCOLOR:   col[0]=p[i]; col[1]=p[i+1]; col[2]=p[i+2]; i+=3; break;
        case POSITION: pos[0]=p[i]; pos[1]=p[i+1]; pos[2]=p[i+2]; pos[3]=p[i+3]; i+=4; break;
        default: i++; break;
        }
    }
    glLightfv(GL_LIGHT0 + light, GL_DIFFUSE, col);
    glLightfv(GL_LIGHT0 + light, GL_SPECULAR, col);
    glLightfv(GL_LIGHT0 + light, GL_POSITION, pos);
}

static void apply_lmodel(float *p) {
    if (!p) return;
    float amb[]={0.2f,0.2f,0.2f,1.0f};
    int i=0;
    while (p[i] != 0.0f && i < MAX_PROPS-1) {
        int prop = (int)p[i++];
        switch (prop) {
        case AMBIENT: amb[0]=p[i]; amb[1]=p[i+1]; amb[2]=p[i+2]; i+=3; break;
        default: i++; break;
        }
    }
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, amb);
}

void lmdef(long type, long idx, long count, float *data) {
    if (idx < 0 || idx >= 9) return;
    float *dest = NULL;
    switch ((int)type) {
    case DEFMATERIAL: dest = mat_props[idx]; break;
    case DEFLIGHT:    dest = light_props[idx]; break;
    case DEFLMODEL:   dest = lmodel_props[idx]; break;
    default: return;
    }
    int i;
    for (i = 0; i < count && i < MAX_PROPS-1; i++) dest[i] = data[i];
    dest[i] = 0;
}

void lmbind(int target, int idx) {
    switch (target) {
    case MATERIAL:
        if (idx == 0) { glDisable(GL_LIGHTING); return; }
        if (idx < 9) apply_material(mat_props[idx]);
        break;
    case LIGHT0: case LIGHT1: case LIGHT2: case LIGHT3:
    case LIGHT4: case LIGHT5: case LIGHT6: case LIGHT7: {
        int n = target - LIGHT0;
        if (idx == 0) glDisable(GL_LIGHT0 + n);
        else if (idx < 9) apply_light(light_props[idx], n);
        break;
    }
    case LMODEL:
        if (idx < 9) apply_lmodel(lmodel_props[idx]);
        break;
    }
}
void lmcolor(int mode) { /* stub */ }
void shademodel(int mode) {
    glShadeModel(mode == GOURAUD ? GL_SMOOTH : GL_FLAT);
}
void lshaderange(short a, short b, long c, long d) { /* stub */ }

/* ===================================================================
 * NURBS
 * =================================================================== */

static GLUnurbsObj *nurbs_obj = NULL;

void nurbssurface(float *sk, float *tk, float **ctl, int sorder, int torder, int type) {
    if (!nurbs_obj) nurbs_obj = gluNewNurbsRenderer();
    gluBeginSurface(nurbs_obj);
    gluNurbsSurface(nurbs_obj, 8, sk, 8, tk, 4, 3, ctl[0], sorder, torder, GL_MAP2_VERTEX_3);
    gluEndSurface(nurbs_obj);
}
void nurbscurve(float *knots, float **ctl, int order, int type) {
    if (!nurbs_obj) nurbs_obj = gluNewNurbsRenderer();
    gluBeginCurve(nurbs_obj);
    gluNurbsCurve(nurbs_obj, 11, knots, 3, ctl[0], order, GL_MAP1_VERTEX_3);
    gluEndCurve(nurbs_obj);
}
void pwlcurve(float *knots, float **ctl, int type) { /* stub */ }
void setnurbsproperty(int prop, float val) {
    if (!nurbs_obj) nurbs_obj = gluNewNurbsRenderer();
    switch (prop) {
    case N_PIXEL_TOLERANCE: gluNurbsProperty(nurbs_obj, GLU_SAMPLING_TOLERANCE, val); break;
    case N_ERRORCHECKING: gluNurbsProperty(nurbs_obj, GLU_DISPLAY_MODE, val ? GLU_FILL : GLU_OUTLINE_POLYGON); break;
    }
}
void getnurbsproperty(int prop, float *val) { *val = 0; }

/* ===================================================================
 * Text
 * =================================================================== */

void charstr(char *s) {
    if (!current_win) return;
    HFONT hfont = CreateFont(14, 0, 0, 0, 0, 0, 0, 0,
        ANSI_CHARSET, 0, 0, 0, 0, "Courier New");
    HFONT old = (HFONT)SelectObject(current_win->hdc, hfont);
    wglUseFontBitmaps(current_win->hdc, 0, 255, 1000);
    glListBase(1000);
    glCallLists((int)strlen(s), GL_UNSIGNED_BYTE, s);
    glDeleteLists(1000, 256);
    SelectObject(current_win->hdc, old);
    DeleteObject(hfont);
}
void font(int f) {}
long strwidth(char *s) {
    return (long)(strlen(s) * 9);
}
void textport(int, int, int, int) {}
long getheight(void) { return 14; }
long getfont(void) { return 0; }

/* ===================================================================
 * Events / Input
 * =================================================================== */

void qdevice(Device dev) {
    if (!current_win) return;
    for (int i = 0; i < current_win->num_enabled; i++)
        if (current_win->enabled_devices[i] == dev) return;
    if (current_win->num_enabled < 256)
        current_win->enabled_devices[current_win->num_enabled++] = dev;
}
void unqdevice(Device dev) {
    if (!current_win) return;
    for (int i = 0; i < current_win->num_enabled; i++) {
        if (current_win->enabled_devices[i] == dev) {
            current_win->enabled_devices[i] = current_win->enabled_devices[--current_win->num_enabled];
            return;
        }
    }
}
long qread(Device *dev, short *val) {
    while (!pop_event(dev, val))
        pump_messages();
    return 1;
}
long qtest(void) {
    pump_messages();
    return (current_win && current_win->queue_head) ? 1 : 0;
}
long getbutton(Device dev) {
    return (GetAsyncKeyState(VK_LBUTTON) & 0x8000) ? 1 : 0;
}
long getvaluator(Device dev) {
    POINT p;
    GetCursorPos(&p);
    if (current_win && current_win->hwnd)
        ScreenToClient(current_win->hwnd, &p);
    switch (dev) {
    case MOUSEX: return p.x;
    case MOUSEY: return p.y;
    default: return 0;
    }
}
void noise(Device dev, long count) { /* TIMER0 noise - stub, ignored */ }

/* ===================================================================
 * Popups
 * =================================================================== */

long newpup(void) { return 1; }
void addtopup(long pup, char *str, int val) {}
long dopup(long pup) { return 0; }
void freepup(long pup) {}

/* ===================================================================
 * Colormap
 * =================================================================== */

void multimap(void) {}
void setmap(int) {}
void getmcolor(int i, short *r, short *g, short *b) {
    *r = *g = *b = 0;
}

/* ===================================================================
 * Stubs for all other GL functions
 * =================================================================== */

/* Window management stubs */
void winmove() {}
void prefsize() {}

/* Coordinate stubs */
void draw2i() {}
void drawi() {}
void draws() {}
void move2i() {}
void movei() {}
void moves() {}
void pnt2i() {}
void pnti() {}
void pnts() {}
void cmovi() {}
void cmovs() {}
void arci() {}
void arcfi() {}

/* Misc drawing stubs */
void curvebasis() {}
void curveit() {}
void curveprecision() {}
void defbasis() {}
void deflinestyle() {}
void setlinestyle() {}
void linewidth() { glLineWidth(1); }
void setpattern() {}
long getlstyle() { return 0; }
long getlwidth() { return 1; }
long getmap() { return 0; }
long getcmmode() { return 0; }
long getmonitor() { return 0; }
void setbell() {}
void ringbell() { MessageBeep(0xFFFFFFFF); }
void blankscreen() {}
void blanktime() {}

/* Matrix misc */
void winpush() {}
void winpop() {}
void popattributes() { glPopAttrib(); }
void pushattributes() { glPushAttrib(GL_ALL_ATTRIB_BITS); }
void popviewport() { glPopAttrib(); }
void pushviewport() { glPushAttrib(GL_VIEWPORT_BIT); }

/* Display list stubs */
long genobj() {
    static long next = 1;
    return next++;
}
long gentag() {
    static long next = 1;
    return next++;
}
int isobj(long n) { return glIsList((GLuint)n) ? 1 : 0; }
int istag() { return 0; }
void makeobj(long n) { glNewList((GLuint)n, GL_COMPILE); }
void callobj(long n) { glCallList((GLuint)n); }
void closeobj() { glEndList(); }
void delobj(long n) { glDeleteLists((GLuint)n, 1); }
void deltag() {}
void editobj() {}
void objinsert() {}
void objreplace() {}

/* Misc stubs */
void overlay() { }
void underlay() { }
long getbuffer() { return 0; }
long getdrawmode() { return current_win ? current_win->drawmode : 0; }
void rectzoom() {}
void readsource() {}
void blendfunction() {}
void logicop() {}
void subpixel() {}
void smoothline() {}
void pntsmooth() {}
void linesmooth() {}
void swaptmesh() {}
void gammaramp() {}
long getmmode() { return matrix_mode; }
long getbackface() { return 0; }
long getdescender() { return 0; }
long getdisplaymode() { return 0; }
long getwritemask() { return 0xFFFFFFFF; }
long getcolor() { return 0; }
void curvn() {}
void rcrv() {}
void rcrvn() {}
void crv() {}
void crvn() {}
void lsbackup() {}
void lsrepeat() {}
long getlsrepeat() { return 0; }
void resetls() {}
void lampoff() {}
void lampon() {}
void wmpack() {}
void cursoff() { ShowCursor(FALSE); }
void curson() { ShowCursor(TRUE); }
void swapinterval() {}
void writemask() {}
void textwritemask() {}
void pagecolor() {}
void textcolor() {}
void backface(int on) { if(on) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE); }
void depthcue() { }

/* Polyline stubs */
void poly() {}
void poly2() {}
void polf() {}
void polf2() {}
void polyi() {}
void poly2i() {}
void polfi() {}
void polf2i() {}

/* Extended drawing stubs */
void pdr() { }
void pdr2() {}
void pdri() {}
void pdrs() {}
void pmv() {}
void pmv2() {}
void pmvi() {}
void pmvs() {}
void rdr() {}
void rdr2() {}
void rdri() {}
void rdrs() {}
void rmv() {}
void rmv2() {}
void rmvi() {}
void rmvs() {}
void rpdr() {}
void rpdr2() {}
void rpdri() {}
void rpdrs() {}
void rpmv() {}
void rpmv2() {}
void rpmvi() {}
void rpmvs() {}
void xfpt() {}
void xfpt2() {}
void xfpti() {}
void xfpts() {}
void xfpt4() {}
void xfpt4i() {}

/* Hardware stubs */
void gsync() {}
void tpon() {}
void tpoff() {}
void clkon() {}
void clkoff() {}
void qreset() {}
void qenter() {}
void gettp() {}
void getgpos() {}
void noport() {}

/* NURBS stubs */
void rpatch() {}
void patch() {}
void patchbasis() {}
void patchcurves() {}
void patchprecision() {}

/* DGL */
long dglopen() { return winopen(""); }
void dglclose() { }
void glcompat() { }

/* Picking */
void initnames() { glInitNames(); }
void pushname(long n) { glPushName((GLuint)n); }
void popname() { glPopName(); }
void loadname(long n) { glLoadName((GLuint)n); }
void picksize() {}
void rectcopy() {}

/* Miscellaneous */
void gRGBcolor() {}
void gRGBmask() {}
void getscrmask() {}
void scrmask() {}
void getcursor() {}
void cmode() {}

/* Pup mode */
void pupmode() {}
void endpupmode() {}
void endfullscrn() {}
void fullscrn() {}
void attachcursor() {}
void tie() {}

/* Font management */
void defrasterfont() {}
void pixmode() {}

/* Concave polygon */
void concave() {}
/* Auto-generated stubs for undefined symbols */
void arcfs() {}
void arcs() {}
void bbox2() {}
void bbox2i() {}
void bbox2s() {}
void blink() {}
void chunksize() {}
void circfi() {}
void circfs() {}
void clearhitcode() {}
void compactify() {}
void curorigin() {}
void curstype() {}
void cyclemap() {}
long czclear() { return 0; }
void defcursor() {}
void devport() {}
void draw2s() {}
void drawmode() {}
long endpick() { return 0; }
long endselect() { return 0; }
void finish() { }
void fudge() {}
void gbegin() {}
long getdcm() { return 0; }
void getdepth() { }
long gethitcode() { return 0; }
long getlsbackup() { return 0; }
void getmatrix() { }
long getopenobj() { return 0; }
long getothermonitor() { return 0; }
long getpattern() { return 0; }
void getport() { }
long getresetls() { return 0; }
long getshade() { return 0; }
long getsm() { return 0; }
long getvideo() { return 0; }
long getzbuffer() { return 0; }
void gewrite() {}
void gselect() {}
void icontitle() {}
void imakebackground() {}
int ismex() { return 0; }
int isqueued() { return 0; }
void lRGBrange() {}
void maketag() {}
void mapw() {}
void mapw2() {}
void mmode() {}
void move2s() {}
void newtag() {}
void objdelete() {}
void onemap() {}
void pagewritemask() {}
void passthrough() {}
void pclos() {}
void pdr2i() {}
void pdr2s() {}
void pick() {}
void pmv2i() {}
void pmv2s() {}
void pnt() {}
void pnt2() {}
void pnt2s() {}
void polf2s() {}
void polfs() {}
void poly2s() {}
void polys() {}
void rdr2i() {}
void rdr2s() {}
void RGBcursor() {}
void RGBrange() {}
void RGBwritemask() {}
void rmv2i() {}
void rmv2s() {}
void rpdr2i() {}
void rpdr2s() {}
void rpmv2i() {}
void rpmv2s() {}
void sboxfi() {}
void sboxfs() {}
void screenspace() {}
void setcursor() {}
void setdepth() {}
void setmonitor() {}
void setpup() {}
void setshade() {}
void setvaluator() {}
void setvideo() {}
void shaderange() {}
void spclos() {}
void splf() {}
void splf2() {}
void splf2i() {}
void splf2s() {}
void splfi() {}
void splfs() {}
void stepunit() {}
void swinopen() {}
void textinit() {}
void videocmd() {}
void viewport() {}
void winattach() {}
long windepth() { return 0; }
void winposition() {}
void writepixels() {}
void xfpt2i() {}
void xfpt2s() {}
void xfpt4s() {}
