/*
Input used to generate the Python module "glmodule.c".
The stub generator is a Python script called "cgen".

Each definition must be contained on one line:

<returntype> <name> <type> <arg> <type> <arg>

<returntype> can be: void, short, long (XXX maybe others?)

<type> can be: char, string, short, float, long, or double
	string indicates a null terminated string;
	if <type> is char and <arg> begins with a *, the * is stripped
	and <type> is changed into string

<arg> has the form <mode> or <mode>[<subscript>]
	where <mode> can be
		s: arg is sent
		r: arg is received		(arg is a pointer)
	and <subscript> can be (N and I are numbers):
		N
		argI
		retval
		N*argI
		N*retval
*/

#include <gl.h>
#include <device.h>

#include "allobjects.h"
#include "import.h"
#include "modsupport.h"
#include "cgensupport.h"

/*
Some stubs are too complicated for the stub generator.
We can include manually written versions of them here.
A line starting with '%' gives the name of the function so the stub
generator can include it in the table of functions.
*/

/*
varray -- an array of v.. calls.
The argument is an array (maybe list or tuple) of points.
Each point must be a tuple or list of coordinates (x, y, z).
The points may be 2- or 3-dimensional but must all have the
same dimension.  Float and int values may be mixed however.
The points are always converted to 3D double precision points
by assuming z=0.0 if necessary (as indicated in the man page),
and for each point v3d() is called.
*/


static object *
gl_varray(self, args)
	object *self;
	object *args;
{
	object *v, *w;
	int i, n, width;
	double vec[3];
	object * (*getitem) FPROTO((object *, int));
	
	if (!getiobjectarg(args, 1, 0, &v))
		return NULL;
	
	if (is_listobject(v)) {
		n = getlistsize(v);
		getitem = getlistitem;
	}
	else if (is_tupleobject(v)) {
		n = gettuplesize(v);
		getitem = gettupleitem;
	}
	else {
		err_badarg();
		return NULL;
	}
	
	if (n == 0) {
		INCREF(None);
		return None;
	}
	if (n > 0)
		w = (*getitem)(v, 0);
	
	width = 0;
	if (w == NULL) {
	}
	else if (is_listobject(w)) {
		width = getlistsize(w);
	}
	else if (is_tupleobject(w)) {
		width = gettuplesize(w);
	}
	
	switch (width) {
	case 2:
		vec[2] = 0.0;
		/* Fall through */
	case 3:
		break;
	default:
		err_badarg();
		return NULL;
	}
	
	for (i = 0; i < n; i++) {
		w = (*getitem)(v, i);
		if (!getidoublearray(w, 1, 0, width, vec))
			return NULL;
		v3d(vec);
	}
	
	INCREF(None);
	return None;
}

/*
vnarray, nvarray -- an array of n3f and v3f calls.
The argument is an array (list or tuple) of pairs of points and normals.
Each pair is a tuple (NOT a list) of a point and a normal for that point.
Each point or normal must be a tuple (NOT a list) of coordinates (x, y, z).
Three coordinates must be given.  Float and int values may be mixed.
For each pair, n3f() is called for the normal, and then v3f() is called
for the vector.

vnarray and nvarray differ only in the order of the vector and normal in
the pair: vnarray expects (v, n) while nvarray expects (n, v).
*/

static object *gen_nvarray(); /* Forward */


static object *
gl_nvarray(self, args)
	object *self;
	object *args;
{
	return gen_nvarray(args, 0);
}


static object *
gl_vnarray(self, args)
	object *self;
	object *args;
{
	return gen_nvarray(args, 1);
}

/* Generic, internal version of {nv,nv}array: inorm indicates the
   argument order, 0: normal first, 1: vector first. */

static object *
gen_nvarray(args, inorm)
	object *args;
	int inorm;
{
	object *v, *w, *wnorm, *wvec;
	int i, n;
	float norm[3], vec[3];
	object * (*getitem) FPROTO((object *, int));
	
	if (!getiobjectarg(args, 1, 0, &v))
		return NULL;
	
	if (is_listobject(v)) {
		n = getlistsize(v);
		getitem = getlistitem;
	}
	else if (is_tupleobject(v)) {
		n = gettuplesize(v);
		getitem = gettupleitem;
	}
	else {
		err_badarg();
		return NULL;
	}
	
	for (i = 0; i < n; i++) {
		w = (*getitem)(v, i);
		if (!is_tupleobject(w) || gettuplesize(w) != 2) {
			err_badarg();
			return NULL;
		}
		wnorm = gettupleitem(w, inorm);
		wvec = gettupleitem(w, 1 - inorm);
		if (!getifloatarray(wnorm, 1, 0, 3, norm) ||
			!getifloatarray(wvec, 1, 0, 3, vec))
			return NULL;
		n3f(norm);
		v3f(vec);
	}
	
	INCREF(None);
	return None;
}

/* nurbssurface(s_knots[], t_knots[], ctl[][], s_order, t_order, type).
   The dimensions of ctl[] are computed as follows:
   [len(s_knots) - s_order], [len(t_knots) - t_order]
*/


static object *
gl_nurbssurface(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	double * arg2 ;
	long arg3 ;
	double * arg4 ;
	double *arg5 ;
	long arg6 ;
	long arg7 ;
	long arg8 ;
	long ncoords;
	long s_byte_stride, t_byte_stride;
	long s_nctl, t_nctl;
	long s, t;
	object *v, *w, *pt;
	double *pnext;
	if (!getilongarraysize(args, 6, 0, &arg1))
		return NULL;
	if ((arg2 = NEW(double, arg1 )) == NULL) {
		return err_nomem();
	}
	if (!getidoublearray(args, 6, 0, arg1 , arg2))
		return NULL;
	if (!getilongarraysize(args, 6, 1, &arg3))
		return NULL;
	if ((arg4 = NEW(double, arg3 )) == NULL) {
		return err_nomem();
	}
	if (!getidoublearray(args, 6, 1, arg3 , arg4))
		return NULL;
	if (!getilongarg(args, 6, 3, &arg6))
		return NULL;
	if (!getilongarg(args, 6, 4, &arg7))
		return NULL;
	if (!getilongarg(args, 6, 5, &arg8))
		return NULL;
	if (arg8 == N_XYZ)
		ncoords = 3;
	else if (arg8 == N_XYZW)
		ncoords = 4;
	else {
		err_badarg();
		return NULL;
	}
	s_nctl = arg1 - arg6;
	t_nctl = arg3 - arg7;
	if (!getiobjectarg(args, 6, 2, &v))
		return NULL;
	if (!is_listobject(v) || getlistsize(v) != s_nctl) {
		err_badarg();
		return NULL;
	}
	if ((arg5 = NEW(double, s_nctl*t_nctl*ncoords )) == NULL) {
		return err_nomem();
	}
	pnext = arg5;
	for (s = 0; s < s_nctl; s++) {
		w = getlistitem(v, s);
		if (w == NULL || !is_listobject(w) ||
					getlistsize(w) != t_nctl) {
			err_badarg();
			return NULL;
		}
		for (t = 0; t < t_nctl; t++) {
			pt = getlistitem(w, t);
			if (!getidoublearray(pt, 1, 0, ncoords, pnext))
				return NULL;
			pnext += ncoords;
		}
	}
	s_byte_stride = sizeof(double) * ncoords;
	t_byte_stride = s_byte_stride * s_nctl;
	nurbssurface( arg1 , arg2 , arg3 , arg4 ,
		s_byte_stride , t_byte_stride , arg5 , arg6 , arg7 , arg8 );
	DEL(arg2);
	DEL(arg4);
	DEL(arg5);
	INCREF(None);
	return None;
}

/* nurbscurve(knots, ctlpoints, order, type).
   The length of ctlpoints is len(knots)-order. */


static object *
gl_nurbscurve(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	double * arg2 ;
	long arg3 ;
	double * arg4 ;
	long arg5 ;
	long arg6 ;
	int ncoords, npoints;
	int i;
	object *v;
	double *pnext;
	if (!getilongarraysize(args, 4, 0, &arg1))
		return NULL;
	if ((arg2 = NEW(double, arg1 )) == NULL) {
		return err_nomem();
	}
	if (!getidoublearray(args, 4, 0, arg1 , arg2))
		return NULL;
	if (!getilongarg(args, 4, 2, &arg5))
		return NULL;
	if (!getilongarg(args, 4, 3, &arg6))
		return NULL;
	if (arg6 == N_ST)
		ncoords = 2;
	else if (arg6 == N_STW)
		ncoords = 3;
	else {
		err_badarg();
		return NULL;
	}
	npoints = arg1 - arg5;
	if (!getiobjectarg(args, 4, 1, &v))
		return NULL;
	if (!is_listobject(v) || getlistsize(v) != npoints) {
		err_badarg();
		return NULL;
	}
	if ((arg4 = NEW(double, npoints*ncoords )) == NULL) {
		return err_nomem();
	}
	pnext = arg4;
	for (i = 0; i < npoints; i++) {
		if (!getidoublearray(getlistitem(v, i), 1, 0, ncoords, pnext))
			return NULL;
		pnext += ncoords;
	}
	arg3 = (sizeof(double)) * ncoords;
	nurbscurve( arg1 , arg2 , arg3 , arg4 , arg5 , arg6 );
	DEL(arg2);
	DEL(arg4);
	INCREF(None);
	return None;
}

/* pwlcurve(points, type).
   Points is a list of points. Type must be N_ST. */


static object *
gl_pwlcurve(self, args)
	object *self;
	object *args;
{
	object *v;
	long type;
	double *data, *pnext;
	long npoints, ncoords;
	int i;
	if (!getiobjectarg(args, 2, 0, &v))
		return NULL;
	if (!getilongarg(args, 2, 1, &type))
		return NULL;
	if (!is_listobject(v)) {
		err_badarg();
		return NULL;
	}
	npoints = getlistsize(v);
	if (type == N_ST)
		ncoords = 2;
	else {
		err_badarg();
		return NULL;
	}
	if ((data = NEW(double, npoints*ncoords)) == NULL) {
		return err_nomem();
	}
	pnext = data;
	for (i = 0; i < npoints; i++) {
		if (!getidoublearray(getlistitem(v, i), 1, 0, ncoords, pnext))
			return NULL;
		pnext += ncoords;
	}
	pwlcurve(npoints, data, sizeof(double)*ncoords, type);
	DEL(data);
	INCREF(None);
	return None;
}


/* Picking and Selecting */

static short *pickbuffer = NULL;
static long pickbuffersize;

static object *
pick_select(args, func)
	object *args;
	void (*func)();
{
	if (!getilongarg(args, 1, 0, &pickbuffersize))
		return NULL;
	if (pickbuffer != NULL) {
		err_setstr(RuntimeError,
			"pick/gselect: already picking/selecting");
		return NULL;
	}
	if ((pickbuffer = NEW(short, pickbuffersize)) == NULL) {
		return err_nomem();
	}
	(*func)(pickbuffer, pickbuffersize);
	INCREF(None);
	return None;
}

static object *
endpick_select(args, func)
	object *args;
	long (*func)();
{
	object *v, *w;
	int i, nhits, n;
	if (!getnoarg(args))
		return NULL;
	if (pickbuffer == NULL) {
		err_setstr(RuntimeError,
			"endpick/endselect: not in pick/select mode");
		return NULL;
	}
	nhits = (*func)(pickbuffer);
	if (nhits < 0) {
		nhits = -nhits; /* How to report buffer overflow otherwise? */
	}
	/* Scan the buffer to see how many integers */
	n = 0;
	for (; nhits > 0; nhits--) {
		n += 1 + pickbuffer[n];
	}
	v = newlistobject(n);
	if (v == NULL)
		return NULL;
	/* XXX Could do it nicer and interpret the data structure here,
	   returning a list of lists. But this can be done in Python... */
	for (i = 0; i < n; i++) {
		w = newintobject((long)pickbuffer[i]);
		if (w == NULL) {
			DECREF(v);
			return NULL;
		}
		setlistitem(v, i, w);
	}
	DEL(pickbuffer);
	pickbuffer = NULL;
	return v;
}

extern void pick(), gselect();
extern long endpick(), endselect();

static object *gl_pick(self, args) object *self, *args; {
	return pick_select(args, pick);
}

static object *gl_endpick(self, args) object *self, *args; {
	return endpick_select(args, endpick);
}

static object *gl_gselect(self, args) object *self, *args; {
	return pick_select(args, gselect);
}

static object *gl_endselect(self, args) object *self, *args; {
	return endpick_select(args, endselect);
}


/* XXX The generator botches this one.  Here's a quick hack to fix it. */


static object *
gl_getmatrix(self, args)
	object *self;
	object *args;
{
	float arg1 [ 16 ] ;
	object *v, *w;
	int i;
	getmatrix( arg1 );
	v = newlistobject(16);
	if (v == NULL) {
		return err_nomem();
	}
	for (i = 0; i < 16; i++) {
		w = mknewfloatobject(arg1[i]);
		if (w == NULL) {
			DECREF(v);
			return NULL;
		}
		setlistitem(v, i, w);
	}
	return v;
}

/* End of manually written stubs */


/* long getshade */

static object *
gl_getshade(self, args)
	object *self;
	object *args;
{
	long retval;
	retval = getshade( );
	return mknewlongobject(retval);
}

/* void devport short s long s */

static object *
gl_devport(self, args)
	object *self;
	object *args;
{
	short arg1 ;
	long arg2 ;
	if (!getishortarg(args, 2, 0, &arg1))
		return NULL;
	if (!getilongarg(args, 2, 1, &arg2))
		return NULL;
	devport( arg1 , arg2 );
	INCREF(None);
	return None;
}

/* void rdr2i long s long s */

static object *
gl_rdr2i(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	long arg2 ;
	if (!getilongarg(args, 2, 0, &arg1))
		return NULL;
	if (!getilongarg(args, 2, 1, &arg2))
		return NULL;
	rdr2i( arg1 , arg2 );
	INCREF(None);
	return None;
}

/* void rectfs short s short s short s short s */

static object *
gl_rectfs(self, args)
	object *self;
	object *args;
{
	short arg1 ;
	short arg2 ;
	short arg3 ;
	short arg4 ;
	if (!getishortarg(args, 4, 0, &arg1))
		return NULL;
	if (!getishortarg(args, 4, 1, &arg2))
		return NULL;
	if (!getishortarg(args, 4, 2, &arg3))
		return NULL;
	if (!getishortarg(args, 4, 3, &arg4))
		return NULL;
	rectfs( arg1 , arg2 , arg3 , arg4 );
	INCREF(None);
	return None;
}

/* void rects short s short s short s short s */

static object *
gl_rects(self, args)
	object *self;
	object *args;
{
	short arg1 ;
	short arg2 ;
	short arg3 ;
	short arg4 ;
	if (!getishortarg(args, 4, 0, &arg1))
		return NULL;
	if (!getishortarg(args, 4, 1, &arg2))
		return NULL;
	if (!getishortarg(args, 4, 2, &arg3))
		return NULL;
	if (!getishortarg(args, 4, 3, &arg4))
		return NULL;
	rects( arg1 , arg2 , arg3 , arg4 );
	INCREF(None);
	return None;
}

/* void rmv2i long s long s */

static object *
gl_rmv2i(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	long arg2 ;
	if (!getilongarg(args, 2, 0, &arg1))
		return NULL;
	if (!getilongarg(args, 2, 1, &arg2))
		return NULL;
	rmv2i( arg1 , arg2 );
	INCREF(None);
	return None;
}

/* void noport */

static object *
gl_noport(self, args)
	object *self;
	object *args;
{
	noport( );
	INCREF(None);
	return None;
}

/* void popviewport */

static object *
gl_popviewport(self, args)
	object *self;
	object *args;
{
	popviewport( );
	INCREF(None);
	return None;
}

/* void clear */

static object *
gl_clear(self, args)
	object *self;
	object *args;
{
	clear( );
	INCREF(None);
	return None;
}

/* void clearhitcode */

static object *
gl_clearhitcode(self, args)
	object *self;
	object *args;
{
	clearhitcode( );
	INCREF(None);
	return None;
}

/* void closeobj */

static object *
gl_closeobj(self, args)
	object *self;
	object *args;
{
	closeobj( );
	INCREF(None);
	return None;
}

/* void cursoff */

static object *
gl_cursoff(self, args)
	object *self;
	object *args;
{
	cursoff( );
	INCREF(None);
	return None;
}

/* void curson */

static object *
gl_curson(self, args)
	object *self;
	object *args;
{
	curson( );
	INCREF(None);
	return None;
}

/* void doublebuffer */

static object *
gl_doublebuffer(self, args)
	object *self;
	object *args;
{
	doublebuffer( );
	INCREF(None);
	return None;
}

/* void finish */

static object *
gl_finish(self, args)
	object *self;
	object *args;
{
	finish( );
	INCREF(None);
	return None;
}

/* void gconfig */

static object *
gl_gconfig(self, args)
	object *self;
	object *args;
{
	gconfig( );
	INCREF(None);
	return None;
}

/* void ginit */

static object *
gl_ginit(self, args)
	object *self;
	object *args;
{
	ginit( );
	INCREF(None);
	return None;
}

/* void greset */

static object *
gl_greset(self, args)
	object *self;
	object *args;
{
	greset( );
	INCREF(None);
	return None;
}

/* void multimap */

static object *
gl_multimap(self, args)
	object *self;
	object *args;
{
	multimap( );
	INCREF(None);
	return None;
}

/* void onemap */

static object *
gl_onemap(self, args)
	object *self;
	object *args;
{
	onemap( );
	INCREF(None);
	return None;
}

/* void popattributes */

static object *
gl_popattributes(self, args)
	object *self;
	object *args;
{
	popattributes( );
	INCREF(None);
	return None;
}

/* void popmatrix */

static object *
gl_popmatrix(self, args)
	object *self;
	object *args;
{
	popmatrix( );
	INCREF(None);
	return None;
}

/* void pushattributes */

static object *
gl_pushattributes(self, args)
	object *self;
	object *args;
{
	pushattributes( );
	INCREF(None);
	return None;
}

/* void pushmatrix */

static object *
gl_pushmatrix(self, args)
	object *self;
	object *args;
{
	pushmatrix( );
	INCREF(None);
	return None;
}

/* void pushviewport */

static object *
gl_pushviewport(self, args)
	object *self;
	object *args;
{
	pushviewport( );
	INCREF(None);
	return None;
}

/* void qreset */

static object *
gl_qreset(self, args)
	object *self;
	object *args;
{
	qreset( );
	INCREF(None);
	return None;
}

/* void RGBmode */

static object *
gl_RGBmode(self, args)
	object *self;
	object *args;
{
	RGBmode( );
	INCREF(None);
	return None;
}

/* void singlebuffer */

static object *
gl_singlebuffer(self, args)
	object *self;
	object *args;
{
	singlebuffer( );
	INCREF(None);
	return None;
}

/* void swapbuffers */

static object *
gl_swapbuffers(self, args)
	object *self;
	object *args;
{
	swapbuffers( );
	INCREF(None);
	return None;
}

/* void gsync */

static object *
gl_gsync(self, args)
	object *self;
	object *args;
{
	gsync( );
	INCREF(None);
	return None;
}

/* void tpon */

static object *
gl_tpon(self, args)
	object *self;
	object *args;
{
	tpon( );
	INCREF(None);
	return None;
}

/* void tpoff */

static object *
gl_tpoff(self, args)
	object *self;
	object *args;
{
	tpoff( );
	INCREF(None);
	return None;
}

/* void clkon */

static object *
gl_clkon(self, args)
	object *self;
	object *args;
{
	clkon( );
	INCREF(None);
	return None;
}

/* void clkoff */

static object *
gl_clkoff(self, args)
	object *self;
	object *args;
{
	clkoff( );
	INCREF(None);
	return None;
}

/* void ringbell */

static object *
gl_ringbell(self, args)
	object *self;
	object *args;
{
	ringbell( );
	INCREF(None);
	return None;
}

/* void gbegin */

static object *
gl_gbegin(self, args)
	object *self;
	object *args;
{
	gbegin( );
	INCREF(None);
	return None;
}

/* void textinit */

static object *
gl_textinit(self, args)
	object *self;
	object *args;
{
	textinit( );
	INCREF(None);
	return None;
}

/* void initnames */

static object *
gl_initnames(self, args)
	object *self;
	object *args;
{
	initnames( );
	INCREF(None);
	return None;
}

/* void pclos */

static object *
gl_pclos(self, args)
	object *self;
	object *args;
{
	pclos( );
	INCREF(None);
	return None;
}

/* void popname */

static object *
gl_popname(self, args)
	object *self;
	object *args;
{
	popname( );
	INCREF(None);
	return None;
}

/* void spclos */

static object *
gl_spclos(self, args)
	object *self;
	object *args;
{
	spclos( );
	INCREF(None);
	return None;
}

/* void zclear */

static object *
gl_zclear(self, args)
	object *self;
	object *args;
{
	zclear( );
	INCREF(None);
	return None;
}

/* void screenspace */

static object *
gl_screenspace(self, args)
	object *self;
	object *args;
{
	screenspace( );
	INCREF(None);
	return None;
}

/* void reshapeviewport */

static object *
gl_reshapeviewport(self, args)
	object *self;
	object *args;
{
	reshapeviewport( );
	INCREF(None);
	return None;
}

/* void winpush */

static object *
gl_winpush(self, args)
	object *self;
	object *args;
{
	winpush( );
	INCREF(None);
	return None;
}

/* void winpop */

static object *
gl_winpop(self, args)
	object *self;
	object *args;
{
	winpop( );
	INCREF(None);
	return None;
}

/* void foreground */

static object *
gl_foreground(self, args)
	object *self;
	object *args;
{
	foreground( );
	INCREF(None);
	return None;
}

/* void endfullscrn */

static object *
gl_endfullscrn(self, args)
	object *self;
	object *args;
{
	endfullscrn( );
	INCREF(None);
	return None;
}

/* void endpupmode */

static object *
gl_endpupmode(self, args)
	object *self;
	object *args;
{
	endpupmode( );
	INCREF(None);
	return None;
}

/* void fullscrn */

static object *
gl_fullscrn(self, args)
	object *self;
	object *args;
{
	fullscrn( );
	INCREF(None);
	return None;
}

/* void pupmode */

static object *
gl_pupmode(self, args)
	object *self;
	object *args;
{
	pupmode( );
	INCREF(None);
	return None;
}

/* void winconstraints */

static object *
gl_winconstraints(self, args)
	object *self;
	object *args;
{
	winconstraints( );
	INCREF(None);
	return None;
}

/* void pagecolor short s */

static object *
gl_pagecolor(self, args)
	object *self;
	object *args;
{
	short arg1 ;
	if (!getishortarg(args, 1, 0, &arg1))
		return NULL;
	pagecolor( arg1 );
	INCREF(None);
	return None;
}

/* void textcolor short s */

static object *
gl_textcolor(self, args)
	object *self;
	object *args;
{
	short arg1 ;
	if (!getishortarg(args, 1, 0, &arg1))
		return NULL;
	textcolor( arg1 );
	INCREF(None);
	return None;
}

/* void color short s */

static object *
gl_color(self, args)
	object *self;
	object *args;
{
	short arg1 ;
	if (!getishortarg(args, 1, 0, &arg1))
		return NULL;
	color( arg1 );
	INCREF(None);
	return None;
}

/* void curveit short s */

static object *
gl_curveit(self, args)
	object *self;
	object *args;
{
	short arg1 ;
	if (!getishortarg(args, 1, 0, &arg1))
		return NULL;
	curveit( arg1 );
	INCREF(None);
	return None;
}

/* void font short s */

static object *
gl_font(self, args)
	object *self;
	object *args;
{
	short arg1 ;
	if (!getishortarg(args, 1, 0, &arg1))
		return NULL;
	font( arg1 );
	INCREF(None);
	return None;
}

/* void linewidth short s */

static object *
gl_linewidth(self, args)
	object *self;
	object *args;
{
	short arg1 ;
	if (!getishortarg(args, 1, 0, &arg1))
		return NULL;
	linewidth( arg1 );
	INCREF(None);
	return None;
}

/* void setlinestyle short s */

static object *
gl_setlinestyle(self, args)
	object *self;
	object *args;
{
	short arg1 ;
	if (!getishortarg(args, 1, 0, &arg1))
		return NULL;
	setlinestyle( arg1 );
	INCREF(None);
	return None;
}

/* void setmap short s */

static object *
gl_setmap(self, args)
	object *self;
	object *args;
{
	short arg1 ;
	if (!getishortarg(args, 1, 0, &arg1))
		return NULL;
	setmap( arg1 );
	INCREF(None);
	return None;
}

/* void swapinterval short s */

static object *
gl_swapinterval(self, args)
	object *self;
	object *args;
{
	short arg1 ;
	if (!getishortarg(args, 1, 0, &arg1))
		return NULL;
	swapinterval( arg1 );
	INCREF(None);
	return None;
}

/* void writemask short s */

static object *
gl_writemask(self, args)
	object *self;
	object *args;
{
	short arg1 ;
	if (!getishortarg(args, 1, 0, &arg1))
		return NULL;
	writemask( arg1 );
	INCREF(None);
	return None;
}

/* void textwritemask short s */

static object *
gl_textwritemask(self, args)
	object *self;
	object *args;
{
	short arg1 ;
	if (!getishortarg(args, 1, 0, &arg1))
		return NULL;
	textwritemask( arg1 );
	INCREF(None);
	return None;
}

/* void qdevice short s */

static object *
gl_qdevice(self, args)
	object *self;
	object *args;
{
	short arg1 ;
	if (!getishortarg(args, 1, 0, &arg1))
		return NULL;
	qdevice( arg1 );
	INCREF(None);
	return None;
}

/* void unqdevice short s */

static object *
gl_unqdevice(self, args)
	object *self;
	object *args;
{
	short arg1 ;
	if (!getishortarg(args, 1, 0, &arg1))
		return NULL;
	unqdevice( arg1 );
	INCREF(None);
	return None;
}

/* void curvebasis short s */

static object *
gl_curvebasis(self, args)
	object *self;
	object *args;
{
	short arg1 ;
	if (!getishortarg(args, 1, 0, &arg1))
		return NULL;
	curvebasis( arg1 );
	INCREF(None);
	return None;
}

/* void curveprecision short s */

static object *
gl_curveprecision(self, args)
	object *self;
	object *args;
{
	short arg1 ;
	if (!getishortarg(args, 1, 0, &arg1))
		return NULL;
	curveprecision( arg1 );
	INCREF(None);
	return None;
}

/* void loadname short s */

static object *
gl_loadname(self, args)
	object *self;
	object *args;
{
	short arg1 ;
	if (!getishortarg(args, 1, 0, &arg1))
		return NULL;
	loadname( arg1 );
	INCREF(None);
	return None;
}

/* void passthrough short s */

static object *
gl_passthrough(self, args)
	object *self;
	object *args;
{
	short arg1 ;
	if (!getishortarg(args, 1, 0, &arg1))
		return NULL;
	passthrough( arg1 );
	INCREF(None);
	return None;
}

/* void pushname short s */

static object *
gl_pushname(self, args)
	object *self;
	object *args;
{
	short arg1 ;
	if (!getishortarg(args, 1, 0, &arg1))
		return NULL;
	pushname( arg1 );
	INCREF(None);
	return None;
}

/* void setmonitor short s */

static object *
gl_setmonitor(self, args)
	object *self;
	object *args;
{
	short arg1 ;
	if (!getishortarg(args, 1, 0, &arg1))
		return NULL;
	setmonitor( arg1 );
	INCREF(None);
	return None;
}

/* void setshade short s */

static object *
gl_setshade(self, args)
	object *self;
	object *args;
{
	short arg1 ;
	if (!getishortarg(args, 1, 0, &arg1))
		return NULL;
	setshade( arg1 );
	INCREF(None);
	return None;
}

/* void setpattern short s */

static object *
gl_setpattern(self, args)
	object *self;
	object *args;
{
	short arg1 ;
	if (!getishortarg(args, 1, 0, &arg1))
		return NULL;
	setpattern( arg1 );
	INCREF(None);
	return None;
}

/* void pagewritemask short s */

static object *
gl_pagewritemask(self, args)
	object *self;
	object *args;
{
	short arg1 ;
	if (!getishortarg(args, 1, 0, &arg1))
		return NULL;
	pagewritemask( arg1 );
	INCREF(None);
	return None;
}

/* void callobj long s */

static object *
gl_callobj(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	if (!getilongarg(args, 1, 0, &arg1))
		return NULL;
	callobj( arg1 );
	INCREF(None);
	return None;
}

/* void delobj long s */

static object *
gl_delobj(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	if (!getilongarg(args, 1, 0, &arg1))
		return NULL;
	delobj( arg1 );
	INCREF(None);
	return None;
}

/* void editobj long s */

static object *
gl_editobj(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	if (!getilongarg(args, 1, 0, &arg1))
		return NULL;
	editobj( arg1 );
	INCREF(None);
	return None;
}

/* void makeobj long s */

static object *
gl_makeobj(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	if (!getilongarg(args, 1, 0, &arg1))
		return NULL;
	makeobj( arg1 );
	INCREF(None);
	return None;
}

/* void maketag long s */

static object *
gl_maketag(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	if (!getilongarg(args, 1, 0, &arg1))
		return NULL;
	maketag( arg1 );
	INCREF(None);
	return None;
}

/* void chunksize long s */

static object *
gl_chunksize(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	if (!getilongarg(args, 1, 0, &arg1))
		return NULL;
	chunksize( arg1 );
	INCREF(None);
	return None;
}

/* void compactify long s */

static object *
gl_compactify(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	if (!getilongarg(args, 1, 0, &arg1))
		return NULL;
	compactify( arg1 );
	INCREF(None);
	return None;
}

/* void deltag long s */

static object *
gl_deltag(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	if (!getilongarg(args, 1, 0, &arg1))
		return NULL;
	deltag( arg1 );
	INCREF(None);
	return None;
}

/* void lsrepeat long s */

static object *
gl_lsrepeat(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	if (!getilongarg(args, 1, 0, &arg1))
		return NULL;
	lsrepeat( arg1 );
	INCREF(None);
	return None;
}

/* void objinsert long s */

static object *
gl_objinsert(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	if (!getilongarg(args, 1, 0, &arg1))
		return NULL;
	objinsert( arg1 );
	INCREF(None);
	return None;
}

/* void objreplace long s */

static object *
gl_objreplace(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	if (!getilongarg(args, 1, 0, &arg1))
		return NULL;
	objreplace( arg1 );
	INCREF(None);
	return None;
}

/* void winclose long s */

static object *
gl_winclose(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	if (!getilongarg(args, 1, 0, &arg1))
		return NULL;
	winclose( arg1 );
	INCREF(None);
	return None;
}

/* void blanktime long s */

static object *
gl_blanktime(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	if (!getilongarg(args, 1, 0, &arg1))
		return NULL;
	blanktime( arg1 );
	INCREF(None);
	return None;
}

/* void freepup long s */

static object *
gl_freepup(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	if (!getilongarg(args, 1, 0, &arg1))
		return NULL;
	freepup( arg1 );
	INCREF(None);
	return None;
}

/* void backbuffer long s */

static object *
gl_backbuffer(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	if (!getilongarg(args, 1, 0, &arg1))
		return NULL;
	backbuffer( arg1 );
	INCREF(None);
	return None;
}

/* void frontbuffer long s */

static object *
gl_frontbuffer(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	if (!getilongarg(args, 1, 0, &arg1))
		return NULL;
	frontbuffer( arg1 );
	INCREF(None);
	return None;
}

/* void lsbackup long s */

static object *
gl_lsbackup(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	if (!getilongarg(args, 1, 0, &arg1))
		return NULL;
	lsbackup( arg1 );
	INCREF(None);
	return None;
}

/* void resetls long s */

static object *
gl_resetls(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	if (!getilongarg(args, 1, 0, &arg1))
		return NULL;
	resetls( arg1 );
	INCREF(None);
	return None;
}

/* void lampon long s */

static object *
gl_lampon(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	if (!getilongarg(args, 1, 0, &arg1))
		return NULL;
	lampon( arg1 );
	INCREF(None);
	return None;
}

/* void lampoff long s */

static object *
gl_lampoff(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	if (!getilongarg(args, 1, 0, &arg1))
		return NULL;
	lampoff( arg1 );
	INCREF(None);
	return None;
}

/* void setbell long s */

static object *
gl_setbell(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	if (!getilongarg(args, 1, 0, &arg1))
		return NULL;
	setbell( arg1 );
	INCREF(None);
	return None;
}

/* void blankscreen long s */

static object *
gl_blankscreen(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	if (!getilongarg(args, 1, 0, &arg1))
		return NULL;
	blankscreen( arg1 );
	INCREF(None);
	return None;
}

/* void depthcue long s */

static object *
gl_depthcue(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	if (!getilongarg(args, 1, 0, &arg1))
		return NULL;
	depthcue( arg1 );
	INCREF(None);
	return None;
}

/* void zbuffer long s */

static object *
gl_zbuffer(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	if (!getilongarg(args, 1, 0, &arg1))
		return NULL;
	zbuffer( arg1 );
	INCREF(None);
	return None;
}

/* void backface long s */

static object *
gl_backface(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	if (!getilongarg(args, 1, 0, &arg1))
		return NULL;
	backface( arg1 );
	INCREF(None);
	return None;
}

/* void cmov2i long s long s */

static object *
gl_cmov2i(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	long arg2 ;
	if (!getilongarg(args, 2, 0, &arg1))
		return NULL;
	if (!getilongarg(args, 2, 1, &arg2))
		return NULL;
	cmov2i( arg1 , arg2 );
	INCREF(None);
	return None;
}

/* void draw2i long s long s */

static object *
gl_draw2i(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	long arg2 ;
	if (!getilongarg(args, 2, 0, &arg1))
		return NULL;
	if (!getilongarg(args, 2, 1, &arg2))
		return NULL;
	draw2i( arg1 , arg2 );
	INCREF(None);
	return None;
}

/* void move2i long s long s */

static object *
gl_move2i(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	long arg2 ;
	if (!getilongarg(args, 2, 0, &arg1))
		return NULL;
	if (!getilongarg(args, 2, 1, &arg2))
		return NULL;
	move2i( arg1 , arg2 );
	INCREF(None);
	return None;
}

/* void pnt2i long s long s */

static object *
gl_pnt2i(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	long arg2 ;
	if (!getilongarg(args, 2, 0, &arg1))
		return NULL;
	if (!getilongarg(args, 2, 1, &arg2))
		return NULL;
	pnt2i( arg1 , arg2 );
	INCREF(None);
	return None;
}

/* void patchbasis long s long s */

static object *
gl_patchbasis(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	long arg2 ;
	if (!getilongarg(args, 2, 0, &arg1))
		return NULL;
	if (!getilongarg(args, 2, 1, &arg2))
		return NULL;
	patchbasis( arg1 , arg2 );
	INCREF(None);
	return None;
}

/* void patchprecision long s long s */

static object *
gl_patchprecision(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	long arg2 ;
	if (!getilongarg(args, 2, 0, &arg1))
		return NULL;
	if (!getilongarg(args, 2, 1, &arg2))
		return NULL;
	patchprecision( arg1 , arg2 );
	INCREF(None);
	return None;
}

/* void pdr2i long s long s */

static object *
gl_pdr2i(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	long arg2 ;
	if (!getilongarg(args, 2, 0, &arg1))
		return NULL;
	if (!getilongarg(args, 2, 1, &arg2))
		return NULL;
	pdr2i( arg1 , arg2 );
	INCREF(None);
	return None;
}

/* void pmv2i long s long s */

static object *
gl_pmv2i(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	long arg2 ;
	if (!getilongarg(args, 2, 0, &arg1))
		return NULL;
	if (!getilongarg(args, 2, 1, &arg2))
		return NULL;
	pmv2i( arg1 , arg2 );
	INCREF(None);
	return None;
}

/* void rpdr2i long s long s */

static object *
gl_rpdr2i(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	long arg2 ;
	if (!getilongarg(args, 2, 0, &arg1))
		return NULL;
	if (!getilongarg(args, 2, 1, &arg2))
		return NULL;
	rpdr2i( arg1 , arg2 );
	INCREF(None);
	return None;
}

/* void rpmv2i long s long s */

static object *
gl_rpmv2i(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	long arg2 ;
	if (!getilongarg(args, 2, 0, &arg1))
		return NULL;
	if (!getilongarg(args, 2, 1, &arg2))
		return NULL;
	rpmv2i( arg1 , arg2 );
	INCREF(None);
	return None;
}

/* void xfpt2i long s long s */

static object *
gl_xfpt2i(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	long arg2 ;
	if (!getilongarg(args, 2, 0, &arg1))
		return NULL;
	if (!getilongarg(args, 2, 1, &arg2))
		return NULL;
	xfpt2i( arg1 , arg2 );
	INCREF(None);
	return None;
}

/* void objdelete long s long s */

static object *
gl_objdelete(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	long arg2 ;
	if (!getilongarg(args, 2, 0, &arg1))
		return NULL;
	if (!getilongarg(args, 2, 1, &arg2))
		return NULL;
	objdelete( arg1 , arg2 );
	INCREF(None);
	return None;
}

/* void patchcurves long s long s */

static object *
gl_patchcurves(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	long arg2 ;
	if (!getilongarg(args, 2, 0, &arg1))
		return NULL;
	if (!getilongarg(args, 2, 1, &arg2))
		return NULL;
	patchcurves( arg1 , arg2 );
	INCREF(None);
	return None;
}

/* void minsize long s long s */

static object *
gl_minsize(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	long arg2 ;
	if (!getilongarg(args, 2, 0, &arg1))
		return NULL;
	if (!getilongarg(args, 2, 1, &arg2))
		return NULL;
	minsize( arg1 , arg2 );
	INCREF(None);
	return None;
}

/* void maxsize long s long s */

static object *
gl_maxsize(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	long arg2 ;
	if (!getilongarg(args, 2, 0, &arg1))
		return NULL;
	if (!getilongarg(args, 2, 1, &arg2))
		return NULL;
	maxsize( arg1 , arg2 );
	INCREF(None);
	return None;
}

/* void keepaspect long s long s */

static object *
gl_keepaspect(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	long arg2 ;
	if (!getilongarg(args, 2, 0, &arg1))
		return NULL;
	if (!getilongarg(args, 2, 1, &arg2))
		return NULL;
	keepaspect( arg1 , arg2 );
	INCREF(None);
	return None;
}

/* void prefsize long s long s */

static object *
gl_prefsize(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	long arg2 ;
	if (!getilongarg(args, 2, 0, &arg1))
		return NULL;
	if (!getilongarg(args, 2, 1, &arg2))
		return NULL;
	prefsize( arg1 , arg2 );
	INCREF(None);
	return None;
}

/* void stepunit long s long s */

static object *
gl_stepunit(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	long arg2 ;
	if (!getilongarg(args, 2, 0, &arg1))
		return NULL;
	if (!getilongarg(args, 2, 1, &arg2))
		return NULL;
	stepunit( arg1 , arg2 );
	INCREF(None);
	return None;
}

/* void fudge long s long s */

static object *
gl_fudge(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	long arg2 ;
	if (!getilongarg(args, 2, 0, &arg1))
		return NULL;
	if (!getilongarg(args, 2, 1, &arg2))
		return NULL;
	fudge( arg1 , arg2 );
	INCREF(None);
	return None;
}

/* void winmove long s long s */

static object *
gl_winmove(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	long arg2 ;
	if (!getilongarg(args, 2, 0, &arg1))
		return NULL;
	if (!getilongarg(args, 2, 1, &arg2))
		return NULL;
	winmove( arg1 , arg2 );
	INCREF(None);
	return None;
}

/* void attachcursor short s short s */

static object *
gl_attachcursor(self, args)
	object *self;
	object *args;
{
	short arg1 ;
	short arg2 ;
	if (!getishortarg(args, 2, 0, &arg1))
		return NULL;
	if (!getishortarg(args, 2, 1, &arg2))
		return NULL;
	attachcursor( arg1 , arg2 );
	INCREF(None);
	return None;
}

/* void deflinestyle short s short s */

static object *
gl_deflinestyle(self, args)
	object *self;
	object *args;
{
	short arg1 ;
	short arg2 ;
	if (!getishortarg(args, 2, 0, &arg1))
		return NULL;
	if (!getishortarg(args, 2, 1, &arg2))
		return NULL;
	deflinestyle( arg1 , arg2 );
	INCREF(None);
	return None;
}

/* void noise short s short s */

static object *
gl_noise(self, args)
	object *self;
	object *args;
{
	short arg1 ;
	short arg2 ;
	if (!getishortarg(args, 2, 0, &arg1))
		return NULL;
	if (!getishortarg(args, 2, 1, &arg2))
		return NULL;
	noise( arg1 , arg2 );
	INCREF(None);
	return None;
}

/* void picksize short s short s */

static object *
gl_picksize(self, args)
	object *self;
	object *args;
{
	short arg1 ;
	short arg2 ;
	if (!getishortarg(args, 2, 0, &arg1))
		return NULL;
	if (!getishortarg(args, 2, 1, &arg2))
		return NULL;
	picksize( arg1 , arg2 );
	INCREF(None);
	return None;
}

/* void qenter short s short s */

static object *
gl_qenter(self, args)
	object *self;
	object *args;
{
	short arg1 ;
	short arg2 ;
	if (!getishortarg(args, 2, 0, &arg1))
		return NULL;
	if (!getishortarg(args, 2, 1, &arg2))
		return NULL;
	qenter( arg1 , arg2 );
	INCREF(None);
	return None;
}

/* void setdepth short s short s */

static object *
gl_setdepth(self, args)
	object *self;
	object *args;
{
	short arg1 ;
	short arg2 ;
	if (!getishortarg(args, 2, 0, &arg1))
		return NULL;
	if (!getishortarg(args, 2, 1, &arg2))
		return NULL;
	setdepth( arg1 , arg2 );
	INCREF(None);
	return None;
}

/* void cmov2s short s short s */

static object *
gl_cmov2s(self, args)
	object *self;
	object *args;
{
	short arg1 ;
	short arg2 ;
	if (!getishortarg(args, 2, 0, &arg1))
		return NULL;
	if (!getishortarg(args, 2, 1, &arg2))
		return NULL;
	cmov2s( arg1 , arg2 );
	INCREF(None);
	return None;
}

/* void draw2s short s short s */

static object *
gl_draw2s(self, args)
	object *self;
	object *args;
{
	short arg1 ;
	short arg2 ;
	if (!getishortarg(args, 2, 0, &arg1))
		return NULL;
	if (!getishortarg(args, 2, 1, &arg2))
		return NULL;
	draw2s( arg1 , arg2 );
	INCREF(None);
	return None;
}

/* void move2s short s short s */

static object *
gl_move2s(self, args)
	object *self;
	object *args;
{
	short arg1 ;
	short arg2 ;
	if (!getishortarg(args, 2, 0, &arg1))
		return NULL;
	if (!getishortarg(args, 2, 1, &arg2))
		return NULL;
	move2s( arg1 , arg2 );
	INCREF(None);
	return None;
}

/* void pdr2s short s short s */

static object *
gl_pdr2s(self, args)
	object *self;
	object *args;
{
	short arg1 ;
	short arg2 ;
	if (!getishortarg(args, 2, 0, &arg1))
		return NULL;
	if (!getishortarg(args, 2, 1, &arg2))
		return NULL;
	pdr2s( arg1 , arg2 );
	INCREF(None);
	return None;
}

/* void pmv2s short s short s */

static object *
gl_pmv2s(self, args)
	object *self;
	object *args;
{
	short arg1 ;
	short arg2 ;
	if (!getishortarg(args, 2, 0, &arg1))
		return NULL;
	if (!getishortarg(args, 2, 1, &arg2))
		return NULL;
	pmv2s( arg1 , arg2 );
	INCREF(None);
	return None;
}

/* void pnt2s short s short s */

static object *
gl_pnt2s(self, args)
	object *self;
	object *args;
{
	short arg1 ;
	short arg2 ;
	if (!getishortarg(args, 2, 0, &arg1))
		return NULL;
	if (!getishortarg(args, 2, 1, &arg2))
		return NULL;
	pnt2s( arg1 , arg2 );
	INCREF(None);
	return None;
}

/* void rdr2s short s short s */

static object *
gl_rdr2s(self, args)
	object *self;
	object *args;
{
	short arg1 ;
	short arg2 ;
	if (!getishortarg(args, 2, 0, &arg1))
		return NULL;
	if (!getishortarg(args, 2, 1, &arg2))
		return NULL;
	rdr2s( arg1 , arg2 );
	INCREF(None);
	return None;
}

/* void rmv2s short s short s */

static object *
gl_rmv2s(self, args)
	object *self;
	object *args;
{
	short arg1 ;
	short arg2 ;
	if (!getishortarg(args, 2, 0, &arg1))
		return NULL;
	if (!getishortarg(args, 2, 1, &arg2))
		return NULL;
	rmv2s( arg1 , arg2 );
	INCREF(None);
	return None;
}

/* void rpdr2s short s short s */

static object *
gl_rpdr2s(self, args)
	object *self;
	object *args;
{
	short arg1 ;
	short arg2 ;
	if (!getishortarg(args, 2, 0, &arg1))
		return NULL;
	if (!getishortarg(args, 2, 1, &arg2))
		return NULL;
	rpdr2s( arg1 , arg2 );
	INCREF(None);
	return None;
}

/* void rpmv2s short s short s */

static object *
gl_rpmv2s(self, args)
	object *self;
	object *args;
{
	short arg1 ;
	short arg2 ;
	if (!getishortarg(args, 2, 0, &arg1))
		return NULL;
	if (!getishortarg(args, 2, 1, &arg2))
		return NULL;
	rpmv2s( arg1 , arg2 );
	INCREF(None);
	return None;
}

/* void xfpt2s short s short s */

static object *
gl_xfpt2s(self, args)
	object *self;
	object *args;
{
	short arg1 ;
	short arg2 ;
	if (!getishortarg(args, 2, 0, &arg1))
		return NULL;
	if (!getishortarg(args, 2, 1, &arg2))
		return NULL;
	xfpt2s( arg1 , arg2 );
	INCREF(None);
	return None;
}

/* void cmov2 float s float s */

static object *
gl_cmov2(self, args)
	object *self;
	object *args;
{
	float arg1 ;
	float arg2 ;
	if (!getifloatarg(args, 2, 0, &arg1))
		return NULL;
	if (!getifloatarg(args, 2, 1, &arg2))
		return NULL;
	cmov2( arg1 , arg2 );
	INCREF(None);
	return None;
}

/* void draw2 float s float s */

static object *
gl_draw2(self, args)
	object *self;
	object *args;
{
	float arg1 ;
	float arg2 ;
	if (!getifloatarg(args, 2, 0, &arg1))
		return NULL;
	if (!getifloatarg(args, 2, 1, &arg2))
		return NULL;
	draw2( arg1 , arg2 );
	INCREF(None);
	return None;
}

/* void move2 float s float s */

static object *
gl_move2(self, args)
	object *self;
	object *args;
{
	float arg1 ;
	float arg2 ;
	if (!getifloatarg(args, 2, 0, &arg1))
		return NULL;
	if (!getifloatarg(args, 2, 1, &arg2))
		return NULL;
	move2( arg1 , arg2 );
	INCREF(None);
	return None;
}

/* void pnt2 float s float s */

static object *
gl_pnt2(self, args)
	object *self;
	object *args;
{
	float arg1 ;
	float arg2 ;
	if (!getifloatarg(args, 2, 0, &arg1))
		return NULL;
	if (!getifloatarg(args, 2, 1, &arg2))
		return NULL;
	pnt2( arg1 , arg2 );
	INCREF(None);
	return None;
}

/* void pdr2 float s float s */

static object *
gl_pdr2(self, args)
	object *self;
	object *args;
{
	float arg1 ;
	float arg2 ;
	if (!getifloatarg(args, 2, 0, &arg1))
		return NULL;
	if (!getifloatarg(args, 2, 1, &arg2))
		return NULL;
	pdr2( arg1 , arg2 );
	INCREF(None);
	return None;
}

/* void pmv2 float s float s */

static object *
gl_pmv2(self, args)
	object *self;
	object *args;
{
	float arg1 ;
	float arg2 ;
	if (!getifloatarg(args, 2, 0, &arg1))
		return NULL;
	if (!getifloatarg(args, 2, 1, &arg2))
		return NULL;
	pmv2( arg1 , arg2 );
	INCREF(None);
	return None;
}

/* void rdr2 float s float s */

static object *
gl_rdr2(self, args)
	object *self;
	object *args;
{
	float arg1 ;
	float arg2 ;
	if (!getifloatarg(args, 2, 0, &arg1))
		return NULL;
	if (!getifloatarg(args, 2, 1, &arg2))
		return NULL;
	rdr2( arg1 , arg2 );
	INCREF(None);
	return None;
}

/* void rmv2 float s float s */

static object *
gl_rmv2(self, args)
	object *self;
	object *args;
{
	float arg1 ;
	float arg2 ;
	if (!getifloatarg(args, 2, 0, &arg1))
		return NULL;
	if (!getifloatarg(args, 2, 1, &arg2))
		return NULL;
	rmv2( arg1 , arg2 );
	INCREF(None);
	return None;
}

/* void rpdr2 float s float s */

static object *
gl_rpdr2(self, args)
	object *self;
	object *args;
{
	float arg1 ;
	float arg2 ;
	if (!getifloatarg(args, 2, 0, &arg1))
		return NULL;
	if (!getifloatarg(args, 2, 1, &arg2))
		return NULL;
	rpdr2( arg1 , arg2 );
	INCREF(None);
	return None;
}

/* void rpmv2 float s float s */

static object *
gl_rpmv2(self, args)
	object *self;
	object *args;
{
	float arg1 ;
	float arg2 ;
	if (!getifloatarg(args, 2, 0, &arg1))
		return NULL;
	if (!getifloatarg(args, 2, 1, &arg2))
		return NULL;
	rpmv2( arg1 , arg2 );
	INCREF(None);
	return None;
}

/* void xfpt2 float s float s */

static object *
gl_xfpt2(self, args)
	object *self;
	object *args;
{
	float arg1 ;
	float arg2 ;
	if (!getifloatarg(args, 2, 0, &arg1))
		return NULL;
	if (!getifloatarg(args, 2, 1, &arg2))
		return NULL;
	xfpt2( arg1 , arg2 );
	INCREF(None);
	return None;
}

/* void loadmatrix float s[16] */

static object *
gl_loadmatrix(self, args)
	object *self;
	object *args;
{
	float arg1 [ 16 ] ;
	if (!getifloatarray(args, 1, 0, 16 , arg1))
		return NULL;
	loadmatrix( arg1 );
	INCREF(None);
	return None;
}

/* void multmatrix float s[16] */

static object *
gl_multmatrix(self, args)
	object *self;
	object *args;
{
	float arg1 [ 16 ] ;
	if (!getifloatarray(args, 1, 0, 16 , arg1))
		return NULL;
	multmatrix( arg1 );
	INCREF(None);
	return None;
}

/* void crv float s[16] */

static object *
gl_crv(self, args)
	object *self;
	object *args;
{
	float arg1 [ 16 ] ;
	if (!getifloatarray(args, 1, 0, 16 , arg1))
		return NULL;
	crv( arg1 );
	INCREF(None);
	return None;
}

/* void rcrv float s[16] */

static object *
gl_rcrv(self, args)
	object *self;
	object *args;
{
	float arg1 [ 16 ] ;
	if (!getifloatarray(args, 1, 0, 16 , arg1))
		return NULL;
	rcrv( arg1 );
	INCREF(None);
	return None;
}

/* void addtopup long s char *s long s */

static object *
gl_addtopup(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	string arg2 ;
	long arg3 ;
	if (!getilongarg(args, 3, 0, &arg1))
		return NULL;
	if (!getistringarg(args, 3, 1, &arg2))
		return NULL;
	if (!getilongarg(args, 3, 2, &arg3))
		return NULL;
	addtopup( arg1 , arg2 , arg3 );
	INCREF(None);
	return None;
}

/* void charstr char *s */

static object *
gl_charstr(self, args)
	object *self;
	object *args;
{
	string arg1 ;
	if (!getistringarg(args, 1, 0, &arg1))
		return NULL;
	charstr( arg1 );
	INCREF(None);
	return None;
}

/* void getport char *s */

static object *
gl_getport(self, args)
	object *self;
	object *args;
{
	string arg1 ;
	if (!getistringarg(args, 1, 0, &arg1))
		return NULL;
	getport( arg1 );
	INCREF(None);
	return None;
}

/* long strwidth char *s */

static object *
gl_strwidth(self, args)
	object *self;
	object *args;
{
	long retval;
	string arg1 ;
	if (!getistringarg(args, 1, 0, &arg1))
		return NULL;
	retval = strwidth( arg1 );
	return mknewlongobject(retval);
}

/* long winopen char *s */

static object *
gl_winopen(self, args)
	object *self;
	object *args;
{
	long retval;
	string arg1 ;
	if (!getistringarg(args, 1, 0, &arg1))
		return NULL;
	retval = winopen( arg1 );
	return mknewlongobject(retval);
}

/* void wintitle char *s */

static object *
gl_wintitle(self, args)
	object *self;
	object *args;
{
	string arg1 ;
	if (!getistringarg(args, 1, 0, &arg1))
		return NULL;
	wintitle( arg1 );
	INCREF(None);
	return None;
}

/* void polf long s float s[3*arg1] */

static object *
gl_polf(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	float * arg2 ;
	if (!getilongarraysize(args, 1, 0, &arg1))
		return NULL;
	arg1 = arg1 / 3;
	if ((arg2 = NEW(float, 3 * arg1 )) == NULL)
		return err_nomem();
	if (!getifloatarray(args, 1, 0, 3 * arg1 , arg2))
		return NULL;
	polf( arg1 , arg2 );
	DEL(arg2);
	INCREF(None);
	return None;
}

/* void polf2 long s float s[2*arg1] */

static object *
gl_polf2(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	float * arg2 ;
	if (!getilongarraysize(args, 1, 0, &arg1))
		return NULL;
	arg1 = arg1 / 2;
	if ((arg2 = NEW(float, 2 * arg1 )) == NULL)
		return err_nomem();
	if (!getifloatarray(args, 1, 0, 2 * arg1 , arg2))
		return NULL;
	polf2( arg1 , arg2 );
	DEL(arg2);
	INCREF(None);
	return None;
}

/* void poly long s float s[3*arg1] */

static object *
gl_poly(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	float * arg2 ;
	if (!getilongarraysize(args, 1, 0, &arg1))
		return NULL;
	arg1 = arg1 / 3;
	if ((arg2 = NEW(float, 3 * arg1 )) == NULL)
		return err_nomem();
	if (!getifloatarray(args, 1, 0, 3 * arg1 , arg2))
		return NULL;
	poly( arg1 , arg2 );
	DEL(arg2);
	INCREF(None);
	return None;
}

/* void poly2 long s float s[2*arg1] */

static object *
gl_poly2(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	float * arg2 ;
	if (!getilongarraysize(args, 1, 0, &arg1))
		return NULL;
	arg1 = arg1 / 2;
	if ((arg2 = NEW(float, 2 * arg1 )) == NULL)
		return err_nomem();
	if (!getifloatarray(args, 1, 0, 2 * arg1 , arg2))
		return NULL;
	poly2( arg1 , arg2 );
	DEL(arg2);
	INCREF(None);
	return None;
}

/* void crvn long s float s[3*arg1] */

static object *
gl_crvn(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	float * arg2 ;
	if (!getilongarraysize(args, 1, 0, &arg1))
		return NULL;
	arg1 = arg1 / 3;
	if ((arg2 = NEW(float, 3 * arg1 )) == NULL)
		return err_nomem();
	if (!getifloatarray(args, 1, 0, 3 * arg1 , arg2))
		return NULL;
	crvn( arg1 , arg2 );
	DEL(arg2);
	INCREF(None);
	return None;
}

/* void rcrvn long s float s[4*arg1] */

static object *
gl_rcrvn(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	float * arg2 ;
	if (!getilongarraysize(args, 1, 0, &arg1))
		return NULL;
	arg1 = arg1 / 4;
	if ((arg2 = NEW(float, 4 * arg1 )) == NULL)
		return err_nomem();
	if (!getifloatarray(args, 1, 0, 4 * arg1 , arg2))
		return NULL;
	rcrvn( arg1 , arg2 );
	DEL(arg2);
	INCREF(None);
	return None;
}

/* void polf2i long s long s[2*arg1] */

static object *
gl_polf2i(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	long * arg2 ;
	if (!getilongarraysize(args, 1, 0, &arg1))
		return NULL;
	arg1 = arg1 / 2;
	if ((arg2 = NEW(long, 2 * arg1 )) == NULL)
		return err_nomem();
	if (!getilongarray(args, 1, 0, 2 * arg1 , arg2))
		return NULL;
	polf2i( arg1 , arg2 );
	DEL(arg2);
	INCREF(None);
	return None;
}

/* void polfi long s long s[3*arg1] */

static object *
gl_polfi(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	long * arg2 ;
	if (!getilongarraysize(args, 1, 0, &arg1))
		return NULL;
	arg1 = arg1 / 3;
	if ((arg2 = NEW(long, 3 * arg1 )) == NULL)
		return err_nomem();
	if (!getilongarray(args, 1, 0, 3 * arg1 , arg2))
		return NULL;
	polfi( arg1 , arg2 );
	DEL(arg2);
	INCREF(None);
	return None;
}

/* void poly2i long s long s[2*arg1] */

static object *
gl_poly2i(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	long * arg2 ;
	if (!getilongarraysize(args, 1, 0, &arg1))
		return NULL;
	arg1 = arg1 / 2;
	if ((arg2 = NEW(long, 2 * arg1 )) == NULL)
		return err_nomem();
	if (!getilongarray(args, 1, 0, 2 * arg1 , arg2))
		return NULL;
	poly2i( arg1 , arg2 );
	DEL(arg2);
	INCREF(None);
	return None;
}

/* void polyi long s long s[3*arg1] */

static object *
gl_polyi(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	long * arg2 ;
	if (!getilongarraysize(args, 1, 0, &arg1))
		return NULL;
	arg1 = arg1 / 3;
	if ((arg2 = NEW(long, 3 * arg1 )) == NULL)
		return err_nomem();
	if (!getilongarray(args, 1, 0, 3 * arg1 , arg2))
		return NULL;
	polyi( arg1 , arg2 );
	DEL(arg2);
	INCREF(None);
	return None;
}

/* void polf2s long s short s[2*arg1] */

static object *
gl_polf2s(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	short * arg2 ;
	if (!getilongarraysize(args, 1, 0, &arg1))
		return NULL;
	arg1 = arg1 / 2;
	if ((arg2 = NEW(short, 2 * arg1 )) == NULL)
		return err_nomem();
	if (!getishortarray(args, 1, 0, 2 * arg1 , arg2))
		return NULL;
	polf2s( arg1 , arg2 );
	DEL(arg2);
	INCREF(None);
	return None;
}

/* void polfs long s short s[3*arg1] */

static object *
gl_polfs(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	short * arg2 ;
	if (!getilongarraysize(args, 1, 0, &arg1))
		return NULL;
	arg1 = arg1 / 3;
	if ((arg2 = NEW(short, 3 * arg1 )) == NULL)
		return err_nomem();
	if (!getishortarray(args, 1, 0, 3 * arg1 , arg2))
		return NULL;
	polfs( arg1 , arg2 );
	DEL(arg2);
	INCREF(None);
	return None;
}

/* void polys long s short s[3*arg1] */

static object *
gl_polys(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	short * arg2 ;
	if (!getilongarraysize(args, 1, 0, &arg1))
		return NULL;
	arg1 = arg1 / 3;
	if ((arg2 = NEW(short, 3 * arg1 )) == NULL)
		return err_nomem();
	if (!getishortarray(args, 1, 0, 3 * arg1 , arg2))
		return NULL;
	polys( arg1 , arg2 );
	DEL(arg2);
	INCREF(None);
	return None;
}

/* void poly2s long s short s[2*arg1] */

static object *
gl_poly2s(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	short * arg2 ;
	if (!getilongarraysize(args, 1, 0, &arg1))
		return NULL;
	arg1 = arg1 / 2;
	if ((arg2 = NEW(short, 2 * arg1 )) == NULL)
		return err_nomem();
	if (!getishortarray(args, 1, 0, 2 * arg1 , arg2))
		return NULL;
	poly2s( arg1 , arg2 );
	DEL(arg2);
	INCREF(None);
	return None;
}

/* void defcursor short s short s[16] */

static object *
gl_defcursor(self, args)
	object *self;
	object *args;
{
	short arg1 ;
	short arg2 [ 16 ] ;
	if (!getishortarg(args, 2, 0, &arg1))
		return NULL;
	if (!getishortarray(args, 2, 1, 16 , arg2))
		return NULL;
	defcursor( arg1 , arg2 );
	INCREF(None);
	return None;
}

/* void writepixels short s short s[arg1] */

static object *
gl_writepixels(self, args)
	object *self;
	object *args;
{
	short arg1 ;
	short * arg2 ;
	if (!getishortarraysize(args, 1, 0, &arg1))
		return NULL;
	if ((arg2 = NEW(short, arg1 )) == NULL)
		return err_nomem();
	if (!getishortarray(args, 1, 0, arg1 , arg2))
		return NULL;
	writepixels( arg1 , arg2 );
	DEL(arg2);
	INCREF(None);
	return None;
}

/* void defbasis long s float s[16] */

static object *
gl_defbasis(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	float arg2 [ 16 ] ;
	if (!getilongarg(args, 2, 0, &arg1))
		return NULL;
	if (!getifloatarray(args, 2, 1, 16 , arg2))
		return NULL;
	defbasis( arg1 , arg2 );
	INCREF(None);
	return None;
}

/* void gewrite short s short s[arg1] */

static object *
gl_gewrite(self, args)
	object *self;
	object *args;
{
	short arg1 ;
	short * arg2 ;
	if (!getishortarraysize(args, 1, 0, &arg1))
		return NULL;
	if ((arg2 = NEW(short, arg1 )) == NULL)
		return err_nomem();
	if (!getishortarray(args, 1, 0, arg1 , arg2))
		return NULL;
	gewrite( arg1 , arg2 );
	DEL(arg2);
	INCREF(None);
	return None;
}

/* void rotate short s char s */

static object *
gl_rotate(self, args)
	object *self;
	object *args;
{
	short arg1 ;
	char arg2 ;
	if (!getishortarg(args, 2, 0, &arg1))
		return NULL;
	if (!getichararg(args, 2, 1, &arg2))
		return NULL;
	rotate( arg1 , arg2 );
	INCREF(None);
	return None;
}

/* void rot float s char s */

static object *
gl_rot(self, args)
	object *self;
	object *args;
{
	float arg1 ;
	char arg2 ;
	if (!getifloatarg(args, 2, 0, &arg1))
		return NULL;
	if (!getichararg(args, 2, 1, &arg2))
		return NULL;
	rot( arg1 , arg2 );
	INCREF(None);
	return None;
}

/* void circfi long s long s long s */

static object *
gl_circfi(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	long arg2 ;
	long arg3 ;
	if (!getilongarg(args, 3, 0, &arg1))
		return NULL;
	if (!getilongarg(args, 3, 1, &arg2))
		return NULL;
	if (!getilongarg(args, 3, 2, &arg3))
		return NULL;
	circfi( arg1 , arg2 , arg3 );
	INCREF(None);
	return None;
}

/* void circi long s long s long s */

static object *
gl_circi(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	long arg2 ;
	long arg3 ;
	if (!getilongarg(args, 3, 0, &arg1))
		return NULL;
	if (!getilongarg(args, 3, 1, &arg2))
		return NULL;
	if (!getilongarg(args, 3, 2, &arg3))
		return NULL;
	circi( arg1 , arg2 , arg3 );
	INCREF(None);
	return None;
}

/* void cmovi long s long s long s */

static object *
gl_cmovi(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	long arg2 ;
	long arg3 ;
	if (!getilongarg(args, 3, 0, &arg1))
		return NULL;
	if (!getilongarg(args, 3, 1, &arg2))
		return NULL;
	if (!getilongarg(args, 3, 2, &arg3))
		return NULL;
	cmovi( arg1 , arg2 , arg3 );
	INCREF(None);
	return None;
}

/* void drawi long s long s long s */

static object *
gl_drawi(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	long arg2 ;
	long arg3 ;
	if (!getilongarg(args, 3, 0, &arg1))
		return NULL;
	if (!getilongarg(args, 3, 1, &arg2))
		return NULL;
	if (!getilongarg(args, 3, 2, &arg3))
		return NULL;
	drawi( arg1 , arg2 , arg3 );
	INCREF(None);
	return None;
}

/* void movei long s long s long s */

static object *
gl_movei(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	long arg2 ;
	long arg3 ;
	if (!getilongarg(args, 3, 0, &arg1))
		return NULL;
	if (!getilongarg(args, 3, 1, &arg2))
		return NULL;
	if (!getilongarg(args, 3, 2, &arg3))
		return NULL;
	movei( arg1 , arg2 , arg3 );
	INCREF(None);
	return None;
}

/* void pnti long s long s long s */

static object *
gl_pnti(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	long arg2 ;
	long arg3 ;
	if (!getilongarg(args, 3, 0, &arg1))
		return NULL;
	if (!getilongarg(args, 3, 1, &arg2))
		return NULL;
	if (!getilongarg(args, 3, 2, &arg3))
		return NULL;
	pnti( arg1 , arg2 , arg3 );
	INCREF(None);
	return None;
}

/* void newtag long s long s long s */

static object *
gl_newtag(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	long arg2 ;
	long arg3 ;
	if (!getilongarg(args, 3, 0, &arg1))
		return NULL;
	if (!getilongarg(args, 3, 1, &arg2))
		return NULL;
	if (!getilongarg(args, 3, 2, &arg3))
		return NULL;
	newtag( arg1 , arg2 , arg3 );
	INCREF(None);
	return None;
}

/* void pdri long s long s long s */

static object *
gl_pdri(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	long arg2 ;
	long arg3 ;
	if (!getilongarg(args, 3, 0, &arg1))
		return NULL;
	if (!getilongarg(args, 3, 1, &arg2))
		return NULL;
	if (!getilongarg(args, 3, 2, &arg3))
		return NULL;
	pdri( arg1 , arg2 , arg3 );
	INCREF(None);
	return None;
}

/* void pmvi long s long s long s */

static object *
gl_pmvi(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	long arg2 ;
	long arg3 ;
	if (!getilongarg(args, 3, 0, &arg1))
		return NULL;
	if (!getilongarg(args, 3, 1, &arg2))
		return NULL;
	if (!getilongarg(args, 3, 2, &arg3))
		return NULL;
	pmvi( arg1 , arg2 , arg3 );
	INCREF(None);
	return None;
}

/* void rdri long s long s long s */

static object *
gl_rdri(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	long arg2 ;
	long arg3 ;
	if (!getilongarg(args, 3, 0, &arg1))
		return NULL;
	if (!getilongarg(args, 3, 1, &arg2))
		return NULL;
	if (!getilongarg(args, 3, 2, &arg3))
		return NULL;
	rdri( arg1 , arg2 , arg3 );
	INCREF(None);
	return None;
}

/* void rmvi long s long s long s */

static object *
gl_rmvi(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	long arg2 ;
	long arg3 ;
	if (!getilongarg(args, 3, 0, &arg1))
		return NULL;
	if (!getilongarg(args, 3, 1, &arg2))
		return NULL;
	if (!getilongarg(args, 3, 2, &arg3))
		return NULL;
	rmvi( arg1 , arg2 , arg3 );
	INCREF(None);
	return None;
}

/* void rpdri long s long s long s */

static object *
gl_rpdri(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	long arg2 ;
	long arg3 ;
	if (!getilongarg(args, 3, 0, &arg1))
		return NULL;
	if (!getilongarg(args, 3, 1, &arg2))
		return NULL;
	if (!getilongarg(args, 3, 2, &arg3))
		return NULL;
	rpdri( arg1 , arg2 , arg3 );
	INCREF(None);
	return None;
}

/* void rpmvi long s long s long s */

static object *
gl_rpmvi(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	long arg2 ;
	long arg3 ;
	if (!getilongarg(args, 3, 0, &arg1))
		return NULL;
	if (!getilongarg(args, 3, 1, &arg2))
		return NULL;
	if (!getilongarg(args, 3, 2, &arg3))
		return NULL;
	rpmvi( arg1 , arg2 , arg3 );
	INCREF(None);
	return None;
}

/* void xfpti long s long s long s */

static object *
gl_xfpti(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	long arg2 ;
	long arg3 ;
	if (!getilongarg(args, 3, 0, &arg1))
		return NULL;
	if (!getilongarg(args, 3, 1, &arg2))
		return NULL;
	if (!getilongarg(args, 3, 2, &arg3))
		return NULL;
	xfpti( arg1 , arg2 , arg3 );
	INCREF(None);
	return None;
}

/* void circ float s float s float s */

static object *
gl_circ(self, args)
	object *self;
	object *args;
{
	float arg1 ;
	float arg2 ;
	float arg3 ;
	if (!getifloatarg(args, 3, 0, &arg1))
		return NULL;
	if (!getifloatarg(args, 3, 1, &arg2))
		return NULL;
	if (!getifloatarg(args, 3, 2, &arg3))
		return NULL;
	circ( arg1 , arg2 , arg3 );
	INCREF(None);
	return None;
}

/* void circf float s float s float s */

static object *
gl_circf(self, args)
	object *self;
	object *args;
{
	float arg1 ;
	float arg2 ;
	float arg3 ;
	if (!getifloatarg(args, 3, 0, &arg1))
		return NULL;
	if (!getifloatarg(args, 3, 1, &arg2))
		return NULL;
	if (!getifloatarg(args, 3, 2, &arg3))
		return NULL;
	circf( arg1 , arg2 , arg3 );
	INCREF(None);
	return None;
}

/* void cmov float s float s float s */

static object *
gl_cmov(self, args)
	object *self;
	object *args;
{
	float arg1 ;
	float arg2 ;
	float arg3 ;
	if (!getifloatarg(args, 3, 0, &arg1))
		return NULL;
	if (!getifloatarg(args, 3, 1, &arg2))
		return NULL;
	if (!getifloatarg(args, 3, 2, &arg3))
		return NULL;
	cmov( arg1 , arg2 , arg3 );
	INCREF(None);
	return None;
}

/* void draw float s float s float s */

static object *
gl_draw(self, args)
	object *self;
	object *args;
{
	float arg1 ;
	float arg2 ;
	float arg3 ;
	if (!getifloatarg(args, 3, 0, &arg1))
		return NULL;
	if (!getifloatarg(args, 3, 1, &arg2))
		return NULL;
	if (!getifloatarg(args, 3, 2, &arg3))
		return NULL;
	draw( arg1 , arg2 , arg3 );
	INCREF(None);
	return None;
}

/* void move float s float s float s */

static object *
gl_move(self, args)
	object *self;
	object *args;
{
	float arg1 ;
	float arg2 ;
	float arg3 ;
	if (!getifloatarg(args, 3, 0, &arg1))
		return NULL;
	if (!getifloatarg(args, 3, 1, &arg2))
		return NULL;
	if (!getifloatarg(args, 3, 2, &arg3))
		return NULL;
	move( arg1 , arg2 , arg3 );
	INCREF(None);
	return None;
}

/* void pnt float s float s float s */

static object *
gl_pnt(self, args)
	object *self;
	object *args;
{
	float arg1 ;
	float arg2 ;
	float arg3 ;
	if (!getifloatarg(args, 3, 0, &arg1))
		return NULL;
	if (!getifloatarg(args, 3, 1, &arg2))
		return NULL;
	if (!getifloatarg(args, 3, 2, &arg3))
		return NULL;
	pnt( arg1 , arg2 , arg3 );
	INCREF(None);
	return None;
}

/* void scale float s float s float s */

static object *
gl_scale(self, args)
	object *self;
	object *args;
{
	float arg1 ;
	float arg2 ;
	float arg3 ;
	if (!getifloatarg(args, 3, 0, &arg1))
		return NULL;
	if (!getifloatarg(args, 3, 1, &arg2))
		return NULL;
	if (!getifloatarg(args, 3, 2, &arg3))
		return NULL;
	scale( arg1 , arg2 , arg3 );
	INCREF(None);
	return None;
}

/* void translate float s float s float s */

static object *
gl_translate(self, args)
	object *self;
	object *args;
{
	float arg1 ;
	float arg2 ;
	float arg3 ;
	if (!getifloatarg(args, 3, 0, &arg1))
		return NULL;
	if (!getifloatarg(args, 3, 1, &arg2))
		return NULL;
	if (!getifloatarg(args, 3, 2, &arg3))
		return NULL;
	translate( arg1 , arg2 , arg3 );
	INCREF(None);
	return None;
}

/* void pdr float s float s float s */

static object *
gl_pdr(self, args)
	object *self;
	object *args;
{
	float arg1 ;
	float arg2 ;
	float arg3 ;
	if (!getifloatarg(args, 3, 0, &arg1))
		return NULL;
	if (!getifloatarg(args, 3, 1, &arg2))
		return NULL;
	if (!getifloatarg(args, 3, 2, &arg3))
		return NULL;
	pdr( arg1 , arg2 , arg3 );
	INCREF(None);
	return None;
}

/* void pmv float s float s float s */

static object *
gl_pmv(self, args)
	object *self;
	object *args;
{
	float arg1 ;
	float arg2 ;
	float arg3 ;
	if (!getifloatarg(args, 3, 0, &arg1))
		return NULL;
	if (!getifloatarg(args, 3, 1, &arg2))
		return NULL;
	if (!getifloatarg(args, 3, 2, &arg3))
		return NULL;
	pmv( arg1 , arg2 , arg3 );
	INCREF(None);
	return None;
}

/* void rdr float s float s float s */

static object *
gl_rdr(self, args)
	object *self;
	object *args;
{
	float arg1 ;
	float arg2 ;
	float arg3 ;
	if (!getifloatarg(args, 3, 0, &arg1))
		return NULL;
	if (!getifloatarg(args, 3, 1, &arg2))
		return NULL;
	if (!getifloatarg(args, 3, 2, &arg3))
		return NULL;
	rdr( arg1 , arg2 , arg3 );
	INCREF(None);
	return None;
}

/* void rmv float s float s float s */

static object *
gl_rmv(self, args)
	object *self;
	object *args;
{
	float arg1 ;
	float arg2 ;
	float arg3 ;
	if (!getifloatarg(args, 3, 0, &arg1))
		return NULL;
	if (!getifloatarg(args, 3, 1, &arg2))
		return NULL;
	if (!getifloatarg(args, 3, 2, &arg3))
		return NULL;
	rmv( arg1 , arg2 , arg3 );
	INCREF(None);
	return None;
}

/* void rpdr float s float s float s */

static object *
gl_rpdr(self, args)
	object *self;
	object *args;
{
	float arg1 ;
	float arg2 ;
	float arg3 ;
	if (!getifloatarg(args, 3, 0, &arg1))
		return NULL;
	if (!getifloatarg(args, 3, 1, &arg2))
		return NULL;
	if (!getifloatarg(args, 3, 2, &arg3))
		return NULL;
	rpdr( arg1 , arg2 , arg3 );
	INCREF(None);
	return None;
}

/* void rpmv float s float s float s */

static object *
gl_rpmv(self, args)
	object *self;
	object *args;
{
	float arg1 ;
	float arg2 ;
	float arg3 ;
	if (!getifloatarg(args, 3, 0, &arg1))
		return NULL;
	if (!getifloatarg(args, 3, 1, &arg2))
		return NULL;
	if (!getifloatarg(args, 3, 2, &arg3))
		return NULL;
	rpmv( arg1 , arg2 , arg3 );
	INCREF(None);
	return None;
}

/* void xfpt float s float s float s */

static object *
gl_xfpt(self, args)
	object *self;
	object *args;
{
	float arg1 ;
	float arg2 ;
	float arg3 ;
	if (!getifloatarg(args, 3, 0, &arg1))
		return NULL;
	if (!getifloatarg(args, 3, 1, &arg2))
		return NULL;
	if (!getifloatarg(args, 3, 2, &arg3))
		return NULL;
	xfpt( arg1 , arg2 , arg3 );
	INCREF(None);
	return None;
}

/* void RGBcolor short s short s short s */

static object *
gl_RGBcolor(self, args)
	object *self;
	object *args;
{
	short arg1 ;
	short arg2 ;
	short arg3 ;
	if (!getishortarg(args, 3, 0, &arg1))
		return NULL;
	if (!getishortarg(args, 3, 1, &arg2))
		return NULL;
	if (!getishortarg(args, 3, 2, &arg3))
		return NULL;
	RGBcolor( arg1 , arg2 , arg3 );
	INCREF(None);
	return None;
}

/* void RGBwritemask short s short s short s */

static object *
gl_RGBwritemask(self, args)
	object *self;
	object *args;
{
	short arg1 ;
	short arg2 ;
	short arg3 ;
	if (!getishortarg(args, 3, 0, &arg1))
		return NULL;
	if (!getishortarg(args, 3, 1, &arg2))
		return NULL;
	if (!getishortarg(args, 3, 2, &arg3))
		return NULL;
	RGBwritemask( arg1 , arg2 , arg3 );
	INCREF(None);
	return None;
}

/* void setcursor short s short s short s */

static object *
gl_setcursor(self, args)
	object *self;
	object *args;
{
	short arg1 ;
	short arg2 ;
	short arg3 ;
	if (!getishortarg(args, 3, 0, &arg1))
		return NULL;
	if (!getishortarg(args, 3, 1, &arg2))
		return NULL;
	if (!getishortarg(args, 3, 2, &arg3))
		return NULL;
	setcursor( arg1 , arg2 , arg3 );
	INCREF(None);
	return None;
}

/* void tie short s short s short s */

static object *
gl_tie(self, args)
	object *self;
	object *args;
{
	short arg1 ;
	short arg2 ;
	short arg3 ;
	if (!getishortarg(args, 3, 0, &arg1))
		return NULL;
	if (!getishortarg(args, 3, 1, &arg2))
		return NULL;
	if (!getishortarg(args, 3, 2, &arg3))
		return NULL;
	tie( arg1 , arg2 , arg3 );
	INCREF(None);
	return None;
}

/* void circfs short s short s short s */

static object *
gl_circfs(self, args)
	object *self;
	object *args;
{
	short arg1 ;
	short arg2 ;
	short arg3 ;
	if (!getishortarg(args, 3, 0, &arg1))
		return NULL;
	if (!getishortarg(args, 3, 1, &arg2))
		return NULL;
	if (!getishortarg(args, 3, 2, &arg3))
		return NULL;
	circfs( arg1 , arg2 , arg3 );
	INCREF(None);
	return None;
}

/* void circs short s short s short s */

static object *
gl_circs(self, args)
	object *self;
	object *args;
{
	short arg1 ;
	short arg2 ;
	short arg3 ;
	if (!getishortarg(args, 3, 0, &arg1))
		return NULL;
	if (!getishortarg(args, 3, 1, &arg2))
		return NULL;
	if (!getishortarg(args, 3, 2, &arg3))
		return NULL;
	circs( arg1 , arg2 , arg3 );
	INCREF(None);
	return None;
}

/* void cmovs short s short s short s */

static object *
gl_cmovs(self, args)
	object *self;
	object *args;
{
	short arg1 ;
	short arg2 ;
	short arg3 ;
	if (!getishortarg(args, 3, 0, &arg1))
		return NULL;
	if (!getishortarg(args, 3, 1, &arg2))
		return NULL;
	if (!getishortarg(args, 3, 2, &arg3))
		return NULL;
	cmovs( arg1 , arg2 , arg3 );
	INCREF(None);
	return None;
}

/* void draws short s short s short s */

static object *
gl_draws(self, args)
	object *self;
	object *args;
{
	short arg1 ;
	short arg2 ;
	short arg3 ;
	if (!getishortarg(args, 3, 0, &arg1))
		return NULL;
	if (!getishortarg(args, 3, 1, &arg2))
		return NULL;
	if (!getishortarg(args, 3, 2, &arg3))
		return NULL;
	draws( arg1 , arg2 , arg3 );
	INCREF(None);
	return None;
}

/* void moves short s short s short s */

static object *
gl_moves(self, args)
	object *self;
	object *args;
{
	short arg1 ;
	short arg2 ;
	short arg3 ;
	if (!getishortarg(args, 3, 0, &arg1))
		return NULL;
	if (!getishortarg(args, 3, 1, &arg2))
		return NULL;
	if (!getishortarg(args, 3, 2, &arg3))
		return NULL;
	moves( arg1 , arg2 , arg3 );
	INCREF(None);
	return None;
}

/* void pdrs short s short s short s */

static object *
gl_pdrs(self, args)
	object *self;
	object *args;
{
	short arg1 ;
	short arg2 ;
	short arg3 ;
	if (!getishortarg(args, 3, 0, &arg1))
		return NULL;
	if (!getishortarg(args, 3, 1, &arg2))
		return NULL;
	if (!getishortarg(args, 3, 2, &arg3))
		return NULL;
	pdrs( arg1 , arg2 , arg3 );
	INCREF(None);
	return None;
}

/* void pmvs short s short s short s */

static object *
gl_pmvs(self, args)
	object *self;
	object *args;
{
	short arg1 ;
	short arg2 ;
	short arg3 ;
	if (!getishortarg(args, 3, 0, &arg1))
		return NULL;
	if (!getishortarg(args, 3, 1, &arg2))
		return NULL;
	if (!getishortarg(args, 3, 2, &arg3))
		return NULL;
	pmvs( arg1 , arg2 , arg3 );
	INCREF(None);
	return None;
}

/* void pnts short s short s short s */

static object *
gl_pnts(self, args)
	object *self;
	object *args;
{
	short arg1 ;
	short arg2 ;
	short arg3 ;
	if (!getishortarg(args, 3, 0, &arg1))
		return NULL;
	if (!getishortarg(args, 3, 1, &arg2))
		return NULL;
	if (!getishortarg(args, 3, 2, &arg3))
		return NULL;
	pnts( arg1 , arg2 , arg3 );
	INCREF(None);
	return None;
}

/* void rdrs short s short s short s */

static object *
gl_rdrs(self, args)
	object *self;
	object *args;
{
	short arg1 ;
	short arg2 ;
	short arg3 ;
	if (!getishortarg(args, 3, 0, &arg1))
		return NULL;
	if (!getishortarg(args, 3, 1, &arg2))
		return NULL;
	if (!getishortarg(args, 3, 2, &arg3))
		return NULL;
	rdrs( arg1 , arg2 , arg3 );
	INCREF(None);
	return None;
}

/* void rmvs short s short s short s */

static object *
gl_rmvs(self, args)
	object *self;
	object *args;
{
	short arg1 ;
	short arg2 ;
	short arg3 ;
	if (!getishortarg(args, 3, 0, &arg1))
		return NULL;
	if (!getishortarg(args, 3, 1, &arg2))
		return NULL;
	if (!getishortarg(args, 3, 2, &arg3))
		return NULL;
	rmvs( arg1 , arg2 , arg3 );
	INCREF(None);
	return None;
}

/* void rpdrs short s short s short s */

static object *
gl_rpdrs(self, args)
	object *self;
	object *args;
{
	short arg1 ;
	short arg2 ;
	short arg3 ;
	if (!getishortarg(args, 3, 0, &arg1))
		return NULL;
	if (!getishortarg(args, 3, 1, &arg2))
		return NULL;
	if (!getishortarg(args, 3, 2, &arg3))
		return NULL;
	rpdrs( arg1 , arg2 , arg3 );
	INCREF(None);
	return None;
}

/* void rpmvs short s short s short s */

static object *
gl_rpmvs(self, args)
	object *self;
	object *args;
{
	short arg1 ;
	short arg2 ;
	short arg3 ;
	if (!getishortarg(args, 3, 0, &arg1))
		return NULL;
	if (!getishortarg(args, 3, 1, &arg2))
		return NULL;
	if (!getishortarg(args, 3, 2, &arg3))
		return NULL;
	rpmvs( arg1 , arg2 , arg3 );
	INCREF(None);
	return None;
}

/* void xfpts short s short s short s */

static object *
gl_xfpts(self, args)
	object *self;
	object *args;
{
	short arg1 ;
	short arg2 ;
	short arg3 ;
	if (!getishortarg(args, 3, 0, &arg1))
		return NULL;
	if (!getishortarg(args, 3, 1, &arg2))
		return NULL;
	if (!getishortarg(args, 3, 2, &arg3))
		return NULL;
	xfpts( arg1 , arg2 , arg3 );
	INCREF(None);
	return None;
}

/* void curorigin short s short s short s */

static object *
gl_curorigin(self, args)
	object *self;
	object *args;
{
	short arg1 ;
	short arg2 ;
	short arg3 ;
	if (!getishortarg(args, 3, 0, &arg1))
		return NULL;
	if (!getishortarg(args, 3, 1, &arg2))
		return NULL;
	if (!getishortarg(args, 3, 2, &arg3))
		return NULL;
	curorigin( arg1 , arg2 , arg3 );
	INCREF(None);
	return None;
}

/* void cyclemap short s short s short s */

static object *
gl_cyclemap(self, args)
	object *self;
	object *args;
{
	short arg1 ;
	short arg2 ;
	short arg3 ;
	if (!getishortarg(args, 3, 0, &arg1))
		return NULL;
	if (!getishortarg(args, 3, 1, &arg2))
		return NULL;
	if (!getishortarg(args, 3, 2, &arg3))
		return NULL;
	cyclemap( arg1 , arg2 , arg3 );
	INCREF(None);
	return None;
}

/* void patch float s[16] float s[16] float s[16] */

static object *
gl_patch(self, args)
	object *self;
	object *args;
{
	float arg1 [ 16 ] ;
	float arg2 [ 16 ] ;
	float arg3 [ 16 ] ;
	if (!getifloatarray(args, 3, 0, 16 , arg1))
		return NULL;
	if (!getifloatarray(args, 3, 1, 16 , arg2))
		return NULL;
	if (!getifloatarray(args, 3, 2, 16 , arg3))
		return NULL;
	patch( arg1 , arg2 , arg3 );
	INCREF(None);
	return None;
}

/* void splf long s float s[3*arg1] short s[arg1] */

static object *
gl_splf(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	float * arg2 ;
	short * arg3 ;
	if (!getilongarraysize(args, 2, 0, &arg1))
		return NULL;
	arg1 = arg1 / 3;
	if ((arg2 = NEW(float, 3 * arg1 )) == NULL)
		return err_nomem();
	if (!getifloatarray(args, 2, 0, 3 * arg1 , arg2))
		return NULL;
	if ((arg3 = NEW(short, arg1 )) == NULL)
		return err_nomem();
	if (!getishortarray(args, 2, 1, arg1 , arg3))
		return NULL;
	splf( arg1 , arg2 , arg3 );
	DEL(arg2);
	DEL(arg3);
	INCREF(None);
	return None;
}

/* void splf2 long s float s[2*arg1] short s[arg1] */

static object *
gl_splf2(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	float * arg2 ;
	short * arg3 ;
	if (!getilongarraysize(args, 2, 0, &arg1))
		return NULL;
	arg1 = arg1 / 2;
	if ((arg2 = NEW(float, 2 * arg1 )) == NULL)
		return err_nomem();
	if (!getifloatarray(args, 2, 0, 2 * arg1 , arg2))
		return NULL;
	if ((arg3 = NEW(short, arg1 )) == NULL)
		return err_nomem();
	if (!getishortarray(args, 2, 1, arg1 , arg3))
		return NULL;
	splf2( arg1 , arg2 , arg3 );
	DEL(arg2);
	DEL(arg3);
	INCREF(None);
	return None;
}

/* void splfi long s long s[3*arg1] short s[arg1] */

static object *
gl_splfi(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	long * arg2 ;
	short * arg3 ;
	if (!getilongarraysize(args, 2, 0, &arg1))
		return NULL;
	arg1 = arg1 / 3;
	if ((arg2 = NEW(long, 3 * arg1 )) == NULL)
		return err_nomem();
	if (!getilongarray(args, 2, 0, 3 * arg1 , arg2))
		return NULL;
	if ((arg3 = NEW(short, arg1 )) == NULL)
		return err_nomem();
	if (!getishortarray(args, 2, 1, arg1 , arg3))
		return NULL;
	splfi( arg1 , arg2 , arg3 );
	DEL(arg2);
	DEL(arg3);
	INCREF(None);
	return None;
}

/* void splf2i long s long s[2*arg1] short s[arg1] */

static object *
gl_splf2i(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	long * arg2 ;
	short * arg3 ;
	if (!getilongarraysize(args, 2, 0, &arg1))
		return NULL;
	arg1 = arg1 / 2;
	if ((arg2 = NEW(long, 2 * arg1 )) == NULL)
		return err_nomem();
	if (!getilongarray(args, 2, 0, 2 * arg1 , arg2))
		return NULL;
	if ((arg3 = NEW(short, arg1 )) == NULL)
		return err_nomem();
	if (!getishortarray(args, 2, 1, arg1 , arg3))
		return NULL;
	splf2i( arg1 , arg2 , arg3 );
	DEL(arg2);
	DEL(arg3);
	INCREF(None);
	return None;
}

/* void splfs long s short s[3*arg1] short s[arg1] */

static object *
gl_splfs(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	short * arg2 ;
	short * arg3 ;
	if (!getilongarraysize(args, 2, 0, &arg1))
		return NULL;
	arg1 = arg1 / 3;
	if ((arg2 = NEW(short, 3 * arg1 )) == NULL)
		return err_nomem();
	if (!getishortarray(args, 2, 0, 3 * arg1 , arg2))
		return NULL;
	if ((arg3 = NEW(short, arg1 )) == NULL)
		return err_nomem();
	if (!getishortarray(args, 2, 1, arg1 , arg3))
		return NULL;
	splfs( arg1 , arg2 , arg3 );
	DEL(arg2);
	DEL(arg3);
	INCREF(None);
	return None;
}

/* void splf2s long s short s[2*arg1] short s[arg1] */

static object *
gl_splf2s(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	short * arg2 ;
	short * arg3 ;
	if (!getilongarraysize(args, 2, 0, &arg1))
		return NULL;
	arg1 = arg1 / 2;
	if ((arg2 = NEW(short, 2 * arg1 )) == NULL)
		return err_nomem();
	if (!getishortarray(args, 2, 0, 2 * arg1 , arg2))
		return NULL;
	if ((arg3 = NEW(short, arg1 )) == NULL)
		return err_nomem();
	if (!getishortarray(args, 2, 1, arg1 , arg3))
		return NULL;
	splf2s( arg1 , arg2 , arg3 );
	DEL(arg2);
	DEL(arg3);
	INCREF(None);
	return None;
}

/* void rpatch float s[16] float s[16] float s[16] float s[16] */

static object *
gl_rpatch(self, args)
	object *self;
	object *args;
{
	float arg1 [ 16 ] ;
	float arg2 [ 16 ] ;
	float arg3 [ 16 ] ;
	float arg4 [ 16 ] ;
	if (!getifloatarray(args, 4, 0, 16 , arg1))
		return NULL;
	if (!getifloatarray(args, 4, 1, 16 , arg2))
		return NULL;
	if (!getifloatarray(args, 4, 2, 16 , arg3))
		return NULL;
	if (!getifloatarray(args, 4, 3, 16 , arg4))
		return NULL;
	rpatch( arg1 , arg2 , arg3 , arg4 );
	INCREF(None);
	return None;
}

/* void ortho2 float s float s float s float s */

static object *
gl_ortho2(self, args)
	object *self;
	object *args;
{
	float arg1 ;
	float arg2 ;
	float arg3 ;
	float arg4 ;
	if (!getifloatarg(args, 4, 0, &arg1))
		return NULL;
	if (!getifloatarg(args, 4, 1, &arg2))
		return NULL;
	if (!getifloatarg(args, 4, 2, &arg3))
		return NULL;
	if (!getifloatarg(args, 4, 3, &arg4))
		return NULL;
	ortho2( arg1 , arg2 , arg3 , arg4 );
	INCREF(None);
	return None;
}

/* void rect float s float s float s float s */

static object *
gl_rect(self, args)
	object *self;
	object *args;
{
	float arg1 ;
	float arg2 ;
	float arg3 ;
	float arg4 ;
	if (!getifloatarg(args, 4, 0, &arg1))
		return NULL;
	if (!getifloatarg(args, 4, 1, &arg2))
		return NULL;
	if (!getifloatarg(args, 4, 2, &arg3))
		return NULL;
	if (!getifloatarg(args, 4, 3, &arg4))
		return NULL;
	rect( arg1 , arg2 , arg3 , arg4 );
	INCREF(None);
	return None;
}

/* void rectf float s float s float s float s */

static object *
gl_rectf(self, args)
	object *self;
	object *args;
{
	float arg1 ;
	float arg2 ;
	float arg3 ;
	float arg4 ;
	if (!getifloatarg(args, 4, 0, &arg1))
		return NULL;
	if (!getifloatarg(args, 4, 1, &arg2))
		return NULL;
	if (!getifloatarg(args, 4, 2, &arg3))
		return NULL;
	if (!getifloatarg(args, 4, 3, &arg4))
		return NULL;
	rectf( arg1 , arg2 , arg3 , arg4 );
	INCREF(None);
	return None;
}

/* void xfpt4 float s float s float s float s */

static object *
gl_xfpt4(self, args)
	object *self;
	object *args;
{
	float arg1 ;
	float arg2 ;
	float arg3 ;
	float arg4 ;
	if (!getifloatarg(args, 4, 0, &arg1))
		return NULL;
	if (!getifloatarg(args, 4, 1, &arg2))
		return NULL;
	if (!getifloatarg(args, 4, 2, &arg3))
		return NULL;
	if (!getifloatarg(args, 4, 3, &arg4))
		return NULL;
	xfpt4( arg1 , arg2 , arg3 , arg4 );
	INCREF(None);
	return None;
}

/* void textport short s short s short s short s */

static object *
gl_textport(self, args)
	object *self;
	object *args;
{
	short arg1 ;
	short arg2 ;
	short arg3 ;
	short arg4 ;
	if (!getishortarg(args, 4, 0, &arg1))
		return NULL;
	if (!getishortarg(args, 4, 1, &arg2))
		return NULL;
	if (!getishortarg(args, 4, 2, &arg3))
		return NULL;
	if (!getishortarg(args, 4, 3, &arg4))
		return NULL;
	textport( arg1 , arg2 , arg3 , arg4 );
	INCREF(None);
	return None;
}

/* void mapcolor short s short s short s short s */

static object *
gl_mapcolor(self, args)
	object *self;
	object *args;
{
	short arg1 ;
	short arg2 ;
	short arg3 ;
	short arg4 ;
	if (!getishortarg(args, 4, 0, &arg1))
		return NULL;
	if (!getishortarg(args, 4, 1, &arg2))
		return NULL;
	if (!getishortarg(args, 4, 2, &arg3))
		return NULL;
	if (!getishortarg(args, 4, 3, &arg4))
		return NULL;
	mapcolor( arg1 , arg2 , arg3 , arg4 );
	INCREF(None);
	return None;
}

/* void scrmask short s short s short s short s */

static object *
gl_scrmask(self, args)
	object *self;
	object *args;
{
	short arg1 ;
	short arg2 ;
	short arg3 ;
	short arg4 ;
	if (!getishortarg(args, 4, 0, &arg1))
		return NULL;
	if (!getishortarg(args, 4, 1, &arg2))
		return NULL;
	if (!getishortarg(args, 4, 2, &arg3))
		return NULL;
	if (!getishortarg(args, 4, 3, &arg4))
		return NULL;
	scrmask( arg1 , arg2 , arg3 , arg4 );
	INCREF(None);
	return None;
}

/* void setvaluator short s short s short s short s */

static object *
gl_setvaluator(self, args)
	object *self;
	object *args;
{
	short arg1 ;
	short arg2 ;
	short arg3 ;
	short arg4 ;
	if (!getishortarg(args, 4, 0, &arg1))
		return NULL;
	if (!getishortarg(args, 4, 1, &arg2))
		return NULL;
	if (!getishortarg(args, 4, 2, &arg3))
		return NULL;
	if (!getishortarg(args, 4, 3, &arg4))
		return NULL;
	setvaluator( arg1 , arg2 , arg3 , arg4 );
	INCREF(None);
	return None;
}

/* void viewport short s short s short s short s */

static object *
gl_viewport(self, args)
	object *self;
	object *args;
{
	short arg1 ;
	short arg2 ;
	short arg3 ;
	short arg4 ;
	if (!getishortarg(args, 4, 0, &arg1))
		return NULL;
	if (!getishortarg(args, 4, 1, &arg2))
		return NULL;
	if (!getishortarg(args, 4, 2, &arg3))
		return NULL;
	if (!getishortarg(args, 4, 3, &arg4))
		return NULL;
	viewport( arg1 , arg2 , arg3 , arg4 );
	INCREF(None);
	return None;
}

/* void shaderange short s short s short s short s */

static object *
gl_shaderange(self, args)
	object *self;
	object *args;
{
	short arg1 ;
	short arg2 ;
	short arg3 ;
	short arg4 ;
	if (!getishortarg(args, 4, 0, &arg1))
		return NULL;
	if (!getishortarg(args, 4, 1, &arg2))
		return NULL;
	if (!getishortarg(args, 4, 2, &arg3))
		return NULL;
	if (!getishortarg(args, 4, 3, &arg4))
		return NULL;
	shaderange( arg1 , arg2 , arg3 , arg4 );
	INCREF(None);
	return None;
}

/* void xfpt4s short s short s short s short s */

static object *
gl_xfpt4s(self, args)
	object *self;
	object *args;
{
	short arg1 ;
	short arg2 ;
	short arg3 ;
	short arg4 ;
	if (!getishortarg(args, 4, 0, &arg1))
		return NULL;
	if (!getishortarg(args, 4, 1, &arg2))
		return NULL;
	if (!getishortarg(args, 4, 2, &arg3))
		return NULL;
	if (!getishortarg(args, 4, 3, &arg4))
		return NULL;
	xfpt4s( arg1 , arg2 , arg3 , arg4 );
	INCREF(None);
	return None;
}

/* void rectfi long s long s long s long s */

static object *
gl_rectfi(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	long arg2 ;
	long arg3 ;
	long arg4 ;
	if (!getilongarg(args, 4, 0, &arg1))
		return NULL;
	if (!getilongarg(args, 4, 1, &arg2))
		return NULL;
	if (!getilongarg(args, 4, 2, &arg3))
		return NULL;
	if (!getilongarg(args, 4, 3, &arg4))
		return NULL;
	rectfi( arg1 , arg2 , arg3 , arg4 );
	INCREF(None);
	return None;
}

/* void recti long s long s long s long s */

static object *
gl_recti(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	long arg2 ;
	long arg3 ;
	long arg4 ;
	if (!getilongarg(args, 4, 0, &arg1))
		return NULL;
	if (!getilongarg(args, 4, 1, &arg2))
		return NULL;
	if (!getilongarg(args, 4, 2, &arg3))
		return NULL;
	if (!getilongarg(args, 4, 3, &arg4))
		return NULL;
	recti( arg1 , arg2 , arg3 , arg4 );
	INCREF(None);
	return None;
}

/* void xfpt4i long s long s long s long s */

static object *
gl_xfpt4i(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	long arg2 ;
	long arg3 ;
	long arg4 ;
	if (!getilongarg(args, 4, 0, &arg1))
		return NULL;
	if (!getilongarg(args, 4, 1, &arg2))
		return NULL;
	if (!getilongarg(args, 4, 2, &arg3))
		return NULL;
	if (!getilongarg(args, 4, 3, &arg4))
		return NULL;
	xfpt4i( arg1 , arg2 , arg3 , arg4 );
	INCREF(None);
	return None;
}

/* void prefposition long s long s long s long s */

static object *
gl_prefposition(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	long arg2 ;
	long arg3 ;
	long arg4 ;
	if (!getilongarg(args, 4, 0, &arg1))
		return NULL;
	if (!getilongarg(args, 4, 1, &arg2))
		return NULL;
	if (!getilongarg(args, 4, 2, &arg3))
		return NULL;
	if (!getilongarg(args, 4, 3, &arg4))
		return NULL;
	prefposition( arg1 , arg2 , arg3 , arg4 );
	INCREF(None);
	return None;
}

/* void arc float s float s float s short s short s */

static object *
gl_arc(self, args)
	object *self;
	object *args;
{
	float arg1 ;
	float arg2 ;
	float arg3 ;
	short arg4 ;
	short arg5 ;
	if (!getifloatarg(args, 5, 0, &arg1))
		return NULL;
	if (!getifloatarg(args, 5, 1, &arg2))
		return NULL;
	if (!getifloatarg(args, 5, 2, &arg3))
		return NULL;
	if (!getishortarg(args, 5, 3, &arg4))
		return NULL;
	if (!getishortarg(args, 5, 4, &arg5))
		return NULL;
	arc( arg1 , arg2 , arg3 , arg4 , arg5 );
	INCREF(None);
	return None;
}

/* void arcf float s float s float s short s short s */

static object *
gl_arcf(self, args)
	object *self;
	object *args;
{
	float arg1 ;
	float arg2 ;
	float arg3 ;
	short arg4 ;
	short arg5 ;
	if (!getifloatarg(args, 5, 0, &arg1))
		return NULL;
	if (!getifloatarg(args, 5, 1, &arg2))
		return NULL;
	if (!getifloatarg(args, 5, 2, &arg3))
		return NULL;
	if (!getishortarg(args, 5, 3, &arg4))
		return NULL;
	if (!getishortarg(args, 5, 4, &arg5))
		return NULL;
	arcf( arg1 , arg2 , arg3 , arg4 , arg5 );
	INCREF(None);
	return None;
}

/* void arcfi long s long s long s short s short s */

static object *
gl_arcfi(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	long arg2 ;
	long arg3 ;
	short arg4 ;
	short arg5 ;
	if (!getilongarg(args, 5, 0, &arg1))
		return NULL;
	if (!getilongarg(args, 5, 1, &arg2))
		return NULL;
	if (!getilongarg(args, 5, 2, &arg3))
		return NULL;
	if (!getishortarg(args, 5, 3, &arg4))
		return NULL;
	if (!getishortarg(args, 5, 4, &arg5))
		return NULL;
	arcfi( arg1 , arg2 , arg3 , arg4 , arg5 );
	INCREF(None);
	return None;
}

/* void arci long s long s long s short s short s */

static object *
gl_arci(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	long arg2 ;
	long arg3 ;
	short arg4 ;
	short arg5 ;
	if (!getilongarg(args, 5, 0, &arg1))
		return NULL;
	if (!getilongarg(args, 5, 1, &arg2))
		return NULL;
	if (!getilongarg(args, 5, 2, &arg3))
		return NULL;
	if (!getishortarg(args, 5, 3, &arg4))
		return NULL;
	if (!getishortarg(args, 5, 4, &arg5))
		return NULL;
	arci( arg1 , arg2 , arg3 , arg4 , arg5 );
	INCREF(None);
	return None;
}

/* void bbox2 short s short s float s float s float s float s */

static object *
gl_bbox2(self, args)
	object *self;
	object *args;
{
	short arg1 ;
	short arg2 ;
	float arg3 ;
	float arg4 ;
	float arg5 ;
	float arg6 ;
	if (!getishortarg(args, 6, 0, &arg1))
		return NULL;
	if (!getishortarg(args, 6, 1, &arg2))
		return NULL;
	if (!getifloatarg(args, 6, 2, &arg3))
		return NULL;
	if (!getifloatarg(args, 6, 3, &arg4))
		return NULL;
	if (!getifloatarg(args, 6, 4, &arg5))
		return NULL;
	if (!getifloatarg(args, 6, 5, &arg6))
		return NULL;
	bbox2( arg1 , arg2 , arg3 , arg4 , arg5 , arg6 );
	INCREF(None);
	return None;
}

/* void bbox2i short s short s long s long s long s long s */

static object *
gl_bbox2i(self, args)
	object *self;
	object *args;
{
	short arg1 ;
	short arg2 ;
	long arg3 ;
	long arg4 ;
	long arg5 ;
	long arg6 ;
	if (!getishortarg(args, 6, 0, &arg1))
		return NULL;
	if (!getishortarg(args, 6, 1, &arg2))
		return NULL;
	if (!getilongarg(args, 6, 2, &arg3))
		return NULL;
	if (!getilongarg(args, 6, 3, &arg4))
		return NULL;
	if (!getilongarg(args, 6, 4, &arg5))
		return NULL;
	if (!getilongarg(args, 6, 5, &arg6))
		return NULL;
	bbox2i( arg1 , arg2 , arg3 , arg4 , arg5 , arg6 );
	INCREF(None);
	return None;
}

/* void bbox2s short s short s short s short s short s short s */

static object *
gl_bbox2s(self, args)
	object *self;
	object *args;
{
	short arg1 ;
	short arg2 ;
	short arg3 ;
	short arg4 ;
	short arg5 ;
	short arg6 ;
	if (!getishortarg(args, 6, 0, &arg1))
		return NULL;
	if (!getishortarg(args, 6, 1, &arg2))
		return NULL;
	if (!getishortarg(args, 6, 2, &arg3))
		return NULL;
	if (!getishortarg(args, 6, 3, &arg4))
		return NULL;
	if (!getishortarg(args, 6, 4, &arg5))
		return NULL;
	if (!getishortarg(args, 6, 5, &arg6))
		return NULL;
	bbox2s( arg1 , arg2 , arg3 , arg4 , arg5 , arg6 );
	INCREF(None);
	return None;
}

/* void blink short s short s short s short s short s */

static object *
gl_blink(self, args)
	object *self;
	object *args;
{
	short arg1 ;
	short arg2 ;
	short arg3 ;
	short arg4 ;
	short arg5 ;
	if (!getishortarg(args, 5, 0, &arg1))
		return NULL;
	if (!getishortarg(args, 5, 1, &arg2))
		return NULL;
	if (!getishortarg(args, 5, 2, &arg3))
		return NULL;
	if (!getishortarg(args, 5, 3, &arg4))
		return NULL;
	if (!getishortarg(args, 5, 4, &arg5))
		return NULL;
	blink( arg1 , arg2 , arg3 , arg4 , arg5 );
	INCREF(None);
	return None;
}

/* void ortho float s float s float s float s float s float s */

static object *
gl_ortho(self, args)
	object *self;
	object *args;
{
	float arg1 ;
	float arg2 ;
	float arg3 ;
	float arg4 ;
	float arg5 ;
	float arg6 ;
	if (!getifloatarg(args, 6, 0, &arg1))
		return NULL;
	if (!getifloatarg(args, 6, 1, &arg2))
		return NULL;
	if (!getifloatarg(args, 6, 2, &arg3))
		return NULL;
	if (!getifloatarg(args, 6, 3, &arg4))
		return NULL;
	if (!getifloatarg(args, 6, 4, &arg5))
		return NULL;
	if (!getifloatarg(args, 6, 5, &arg6))
		return NULL;
	ortho( arg1 , arg2 , arg3 , arg4 , arg5 , arg6 );
	INCREF(None);
	return None;
}

/* void window float s float s float s float s float s float s */

static object *
gl_window(self, args)
	object *self;
	object *args;
{
	float arg1 ;
	float arg2 ;
	float arg3 ;
	float arg4 ;
	float arg5 ;
	float arg6 ;
	if (!getifloatarg(args, 6, 0, &arg1))
		return NULL;
	if (!getifloatarg(args, 6, 1, &arg2))
		return NULL;
	if (!getifloatarg(args, 6, 2, &arg3))
		return NULL;
	if (!getifloatarg(args, 6, 3, &arg4))
		return NULL;
	if (!getifloatarg(args, 6, 4, &arg5))
		return NULL;
	if (!getifloatarg(args, 6, 5, &arg6))
		return NULL;
	window( arg1 , arg2 , arg3 , arg4 , arg5 , arg6 );
	INCREF(None);
	return None;
}

/* void lookat float s float s float s float s float s float s short s */

static object *
gl_lookat(self, args)
	object *self;
	object *args;
{
	float arg1 ;
	float arg2 ;
	float arg3 ;
	float arg4 ;
	float arg5 ;
	float arg6 ;
	short arg7 ;
	if (!getifloatarg(args, 7, 0, &arg1))
		return NULL;
	if (!getifloatarg(args, 7, 1, &arg2))
		return NULL;
	if (!getifloatarg(args, 7, 2, &arg3))
		return NULL;
	if (!getifloatarg(args, 7, 3, &arg4))
		return NULL;
	if (!getifloatarg(args, 7, 4, &arg5))
		return NULL;
	if (!getifloatarg(args, 7, 5, &arg6))
		return NULL;
	if (!getishortarg(args, 7, 6, &arg7))
		return NULL;
	lookat( arg1 , arg2 , arg3 , arg4 , arg5 , arg6 , arg7 );
	INCREF(None);
	return None;
}

/* void perspective short s float s float s float s */

static object *
gl_perspective(self, args)
	object *self;
	object *args;
{
	short arg1 ;
	float arg2 ;
	float arg3 ;
	float arg4 ;
	if (!getishortarg(args, 4, 0, &arg1))
		return NULL;
	if (!getifloatarg(args, 4, 1, &arg2))
		return NULL;
	if (!getifloatarg(args, 4, 2, &arg3))
		return NULL;
	if (!getifloatarg(args, 4, 3, &arg4))
		return NULL;
	perspective( arg1 , arg2 , arg3 , arg4 );
	INCREF(None);
	return None;
}

/* void polarview float s short s short s short s */

static object *
gl_polarview(self, args)
	object *self;
	object *args;
{
	float arg1 ;
	short arg2 ;
	short arg3 ;
	short arg4 ;
	if (!getifloatarg(args, 4, 0, &arg1))
		return NULL;
	if (!getishortarg(args, 4, 1, &arg2))
		return NULL;
	if (!getishortarg(args, 4, 2, &arg3))
		return NULL;
	if (!getishortarg(args, 4, 3, &arg4))
		return NULL;
	polarview( arg1 , arg2 , arg3 , arg4 );
	INCREF(None);
	return None;
}

/* void arcfs short s short s short s short s short s */

static object *
gl_arcfs(self, args)
	object *self;
	object *args;
{
	short arg1 ;
	short arg2 ;
	short arg3 ;
	short arg4 ;
	short arg5 ;
	if (!getishortarg(args, 5, 0, &arg1))
		return NULL;
	if (!getishortarg(args, 5, 1, &arg2))
		return NULL;
	if (!getishortarg(args, 5, 2, &arg3))
		return NULL;
	if (!getishortarg(args, 5, 3, &arg4))
		return NULL;
	if (!getishortarg(args, 5, 4, &arg5))
		return NULL;
	arcfs( arg1 , arg2 , arg3 , arg4 , arg5 );
	INCREF(None);
	return None;
}

/* void arcs short s short s short s short s short s */

static object *
gl_arcs(self, args)
	object *self;
	object *args;
{
	short arg1 ;
	short arg2 ;
	short arg3 ;
	short arg4 ;
	short arg5 ;
	if (!getishortarg(args, 5, 0, &arg1))
		return NULL;
	if (!getishortarg(args, 5, 1, &arg2))
		return NULL;
	if (!getishortarg(args, 5, 2, &arg3))
		return NULL;
	if (!getishortarg(args, 5, 3, &arg4))
		return NULL;
	if (!getishortarg(args, 5, 4, &arg5))
		return NULL;
	arcs( arg1 , arg2 , arg3 , arg4 , arg5 );
	INCREF(None);
	return None;
}

/* void rectcopy short s short s short s short s short s short s */

static object *
gl_rectcopy(self, args)
	object *self;
	object *args;
{
	short arg1 ;
	short arg2 ;
	short arg3 ;
	short arg4 ;
	short arg5 ;
	short arg6 ;
	if (!getishortarg(args, 6, 0, &arg1))
		return NULL;
	if (!getishortarg(args, 6, 1, &arg2))
		return NULL;
	if (!getishortarg(args, 6, 2, &arg3))
		return NULL;
	if (!getishortarg(args, 6, 3, &arg4))
		return NULL;
	if (!getishortarg(args, 6, 4, &arg5))
		return NULL;
	if (!getishortarg(args, 6, 5, &arg6))
		return NULL;
	rectcopy( arg1 , arg2 , arg3 , arg4 , arg5 , arg6 );
	INCREF(None);
	return None;
}

/* void RGBcursor short s short s short s short s short s short s short s */

static object *
gl_RGBcursor(self, args)
	object *self;
	object *args;
{
	short arg1 ;
	short arg2 ;
	short arg3 ;
	short arg4 ;
	short arg5 ;
	short arg6 ;
	short arg7 ;
	if (!getishortarg(args, 7, 0, &arg1))
		return NULL;
	if (!getishortarg(args, 7, 1, &arg2))
		return NULL;
	if (!getishortarg(args, 7, 2, &arg3))
		return NULL;
	if (!getishortarg(args, 7, 3, &arg4))
		return NULL;
	if (!getishortarg(args, 7, 4, &arg5))
		return NULL;
	if (!getishortarg(args, 7, 5, &arg6))
		return NULL;
	if (!getishortarg(args, 7, 6, &arg7))
		return NULL;
	RGBcursor( arg1 , arg2 , arg3 , arg4 , arg5 , arg6 , arg7 );
	INCREF(None);
	return None;
}

/* long getbutton short s */

static object *
gl_getbutton(self, args)
	object *self;
	object *args;
{
	long retval;
	short arg1 ;
	if (!getishortarg(args, 1, 0, &arg1))
		return NULL;
	retval = getbutton( arg1 );
	return mknewlongobject(retval);
}

/* long getcmmode */

static object *
gl_getcmmode(self, args)
	object *self;
	object *args;
{
	long retval;
	retval = getcmmode( );
	return mknewlongobject(retval);
}

/* long getlsbackup */

static object *
gl_getlsbackup(self, args)
	object *self;
	object *args;
{
	long retval;
	retval = getlsbackup( );
	return mknewlongobject(retval);
}

/* long getresetls */

static object *
gl_getresetls(self, args)
	object *self;
	object *args;
{
	long retval;
	retval = getresetls( );
	return mknewlongobject(retval);
}

/* long getdcm */

static object *
gl_getdcm(self, args)
	object *self;
	object *args;
{
	long retval;
	retval = getdcm( );
	return mknewlongobject(retval);
}

/* long getzbuffer */

static object *
gl_getzbuffer(self, args)
	object *self;
	object *args;
{
	long retval;
	retval = getzbuffer( );
	return mknewlongobject(retval);
}

/* long ismex */

static object *
gl_ismex(self, args)
	object *self;
	object *args;
{
	long retval;
	retval = ismex( );
	return mknewlongobject(retval);
}

/* long isobj long s */

static object *
gl_isobj(self, args)
	object *self;
	object *args;
{
	long retval;
	long arg1 ;
	if (!getilongarg(args, 1, 0, &arg1))
		return NULL;
	retval = isobj( arg1 );
	return mknewlongobject(retval);
}

/* long isqueued short s */

static object *
gl_isqueued(self, args)
	object *self;
	object *args;
{
	long retval;
	short arg1 ;
	if (!getishortarg(args, 1, 0, &arg1))
		return NULL;
	retval = isqueued( arg1 );
	return mknewlongobject(retval);
}

/* long istag long s */

static object *
gl_istag(self, args)
	object *self;
	object *args;
{
	long retval;
	long arg1 ;
	if (!getilongarg(args, 1, 0, &arg1))
		return NULL;
	retval = istag( arg1 );
	return mknewlongobject(retval);
}

/* long genobj */

static object *
gl_genobj(self, args)
	object *self;
	object *args;
{
	long retval;
	retval = genobj( );
	return mknewlongobject(retval);
}

/* long gentag */

static object *
gl_gentag(self, args)
	object *self;
	object *args;
{
	long retval;
	retval = gentag( );
	return mknewlongobject(retval);
}

/* long getbuffer */

static object *
gl_getbuffer(self, args)
	object *self;
	object *args;
{
	long retval;
	retval = getbuffer( );
	return mknewlongobject(retval);
}

/* long getcolor */

static object *
gl_getcolor(self, args)
	object *self;
	object *args;
{
	long retval;
	retval = getcolor( );
	return mknewlongobject(retval);
}

/* long getdisplaymode */

static object *
gl_getdisplaymode(self, args)
	object *self;
	object *args;
{
	long retval;
	retval = getdisplaymode( );
	return mknewlongobject(retval);
}

/* long getfont */

static object *
gl_getfont(self, args)
	object *self;
	object *args;
{
	long retval;
	retval = getfont( );
	return mknewlongobject(retval);
}

/* long getheight */

static object *
gl_getheight(self, args)
	object *self;
	object *args;
{
	long retval;
	retval = getheight( );
	return mknewlongobject(retval);
}

/* long gethitcode */

static object *
gl_gethitcode(self, args)
	object *self;
	object *args;
{
	long retval;
	retval = gethitcode( );
	return mknewlongobject(retval);
}

/* long getlstyle */

static object *
gl_getlstyle(self, args)
	object *self;
	object *args;
{
	long retval;
	retval = getlstyle( );
	return mknewlongobject(retval);
}

/* long getlwidth */

static object *
gl_getlwidth(self, args)
	object *self;
	object *args;
{
	long retval;
	retval = getlwidth( );
	return mknewlongobject(retval);
}

/* long getmap */

static object *
gl_getmap(self, args)
	object *self;
	object *args;
{
	long retval;
	retval = getmap( );
	return mknewlongobject(retval);
}

/* long getplanes */

static object *
gl_getplanes(self, args)
	object *self;
	object *args;
{
	long retval;
	retval = getplanes( );
	return mknewlongobject(retval);
}

/* long getwritemask */

static object *
gl_getwritemask(self, args)
	object *self;
	object *args;
{
	long retval;
	retval = getwritemask( );
	return mknewlongobject(retval);
}

/* long qtest */

static object *
gl_qtest(self, args)
	object *self;
	object *args;
{
	long retval;
	retval = qtest( );
	return mknewlongobject(retval);
}

/* long getlsrepeat */

static object *
gl_getlsrepeat(self, args)
	object *self;
	object *args;
{
	long retval;
	retval = getlsrepeat( );
	return mknewlongobject(retval);
}

/* long getmonitor */

static object *
gl_getmonitor(self, args)
	object *self;
	object *args;
{
	long retval;
	retval = getmonitor( );
	return mknewlongobject(retval);
}

/* long getopenobj */

static object *
gl_getopenobj(self, args)
	object *self;
	object *args;
{
	long retval;
	retval = getopenobj( );
	return mknewlongobject(retval);
}

/* long getpattern */

static object *
gl_getpattern(self, args)
	object *self;
	object *args;
{
	long retval;
	retval = getpattern( );
	return mknewlongobject(retval);
}

/* long winget */

static object *
gl_winget(self, args)
	object *self;
	object *args;
{
	long retval;
	retval = winget( );
	return mknewlongobject(retval);
}

/* long winattach */

static object *
gl_winattach(self, args)
	object *self;
	object *args;
{
	long retval;
	retval = winattach( );
	return mknewlongobject(retval);
}

/* long getothermonitor */

static object *
gl_getothermonitor(self, args)
	object *self;
	object *args;
{
	long retval;
	retval = getothermonitor( );
	return mknewlongobject(retval);
}

/* long newpup */

static object *
gl_newpup(self, args)
	object *self;
	object *args;
{
	long retval;
	retval = newpup( );
	return mknewlongobject(retval);
}

/* long getvaluator short s */

static object *
gl_getvaluator(self, args)
	object *self;
	object *args;
{
	long retval;
	short arg1 ;
	if (!getishortarg(args, 1, 0, &arg1))
		return NULL;
	retval = getvaluator( arg1 );
	return mknewlongobject(retval);
}

/* void winset long s */

static object *
gl_winset(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	if (!getilongarg(args, 1, 0, &arg1))
		return NULL;
	winset( arg1 );
	INCREF(None);
	return None;
}

/* long dopup long s */

static object *
gl_dopup(self, args)
	object *self;
	object *args;
{
	long retval;
	long arg1 ;
	if (!getilongarg(args, 1, 0, &arg1))
		return NULL;
	retval = dopup( arg1 );
	return mknewlongobject(retval);
}

/* void getdepth short r short r */

static object *
gl_getdepth(self, args)
	object *self;
	object *args;
{
	short arg1 ;
	short arg2 ;
	getdepth( & arg1 , & arg2 );
	{ object *v = newtupleobject( 2 );
	  if (v == NULL) return NULL;
	  settupleitem(v, 0, mknewshortobject(arg1));
	  settupleitem(v, 1, mknewshortobject(arg2));
	  return v;
	}
}

/* void getcpos short r short r */

static object *
gl_getcpos(self, args)
	object *self;
	object *args;
{
	short arg1 ;
	short arg2 ;
	getcpos( & arg1 , & arg2 );
	{ object *v = newtupleobject( 2 );
	  if (v == NULL) return NULL;
	  settupleitem(v, 0, mknewshortobject(arg1));
	  settupleitem(v, 1, mknewshortobject(arg2));
	  return v;
	}
}

/* void getsize long r long r */

static object *
gl_getsize(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	long arg2 ;
	getsize( & arg1 , & arg2 );
	{ object *v = newtupleobject( 2 );
	  if (v == NULL) return NULL;
	  settupleitem(v, 0, mknewlongobject(arg1));
	  settupleitem(v, 1, mknewlongobject(arg2));
	  return v;
	}
}

/* void getorigin long r long r */

static object *
gl_getorigin(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	long arg2 ;
	getorigin( & arg1 , & arg2 );
	{ object *v = newtupleobject( 2 );
	  if (v == NULL) return NULL;
	  settupleitem(v, 0, mknewlongobject(arg1));
	  settupleitem(v, 1, mknewlongobject(arg2));
	  return v;
	}
}

/* void getviewport short r short r short r short r */

static object *
gl_getviewport(self, args)
	object *self;
	object *args;
{
	short arg1 ;
	short arg2 ;
	short arg3 ;
	short arg4 ;
	getviewport( & arg1 , & arg2 , & arg3 , & arg4 );
	{ object *v = newtupleobject( 4 );
	  if (v == NULL) return NULL;
	  settupleitem(v, 0, mknewshortobject(arg1));
	  settupleitem(v, 1, mknewshortobject(arg2));
	  settupleitem(v, 2, mknewshortobject(arg3));
	  settupleitem(v, 3, mknewshortobject(arg4));
	  return v;
	}
}

/* void gettp short r short r short r short r */

static object *
gl_gettp(self, args)
	object *self;
	object *args;
{
	short arg1 ;
	short arg2 ;
	short arg3 ;
	short arg4 ;
	gettp( & arg1 , & arg2 , & arg3 , & arg4 );
	{ object *v = newtupleobject( 4 );
	  if (v == NULL) return NULL;
	  settupleitem(v, 0, mknewshortobject(arg1));
	  settupleitem(v, 1, mknewshortobject(arg2));
	  settupleitem(v, 2, mknewshortobject(arg3));
	  settupleitem(v, 3, mknewshortobject(arg4));
	  return v;
	}
}

/* void getgpos float r float r float r float r */

static object *
gl_getgpos(self, args)
	object *self;
	object *args;
{
	float arg1 ;
	float arg2 ;
	float arg3 ;
	float arg4 ;
	getgpos( & arg1 , & arg2 , & arg3 , & arg4 );
	{ object *v = newtupleobject( 4 );
	  if (v == NULL) return NULL;
	  settupleitem(v, 0, mknewfloatobject(arg1));
	  settupleitem(v, 1, mknewfloatobject(arg2));
	  settupleitem(v, 2, mknewfloatobject(arg3));
	  settupleitem(v, 3, mknewfloatobject(arg4));
	  return v;
	}
}

/* void winposition long s long s long s long s */

static object *
gl_winposition(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	long arg2 ;
	long arg3 ;
	long arg4 ;
	if (!getilongarg(args, 4, 0, &arg1))
		return NULL;
	if (!getilongarg(args, 4, 1, &arg2))
		return NULL;
	if (!getilongarg(args, 4, 2, &arg3))
		return NULL;
	if (!getilongarg(args, 4, 3, &arg4))
		return NULL;
	winposition( arg1 , arg2 , arg3 , arg4 );
	INCREF(None);
	return None;
}

/* void gRGBcolor short r short r short r */

static object *
gl_gRGBcolor(self, args)
	object *self;
	object *args;
{
	short arg1 ;
	short arg2 ;
	short arg3 ;
	gRGBcolor( & arg1 , & arg2 , & arg3 );
	{ object *v = newtupleobject( 3 );
	  if (v == NULL) return NULL;
	  settupleitem(v, 0, mknewshortobject(arg1));
	  settupleitem(v, 1, mknewshortobject(arg2));
	  settupleitem(v, 2, mknewshortobject(arg3));
	  return v;
	}
}

/* void gRGBmask short r short r short r */

static object *
gl_gRGBmask(self, args)
	object *self;
	object *args;
{
	short arg1 ;
	short arg2 ;
	short arg3 ;
	gRGBmask( & arg1 , & arg2 , & arg3 );
	{ object *v = newtupleobject( 3 );
	  if (v == NULL) return NULL;
	  settupleitem(v, 0, mknewshortobject(arg1));
	  settupleitem(v, 1, mknewshortobject(arg2));
	  settupleitem(v, 2, mknewshortobject(arg3));
	  return v;
	}
}

/* void getscrmask short r short r short r short r */

static object *
gl_getscrmask(self, args)
	object *self;
	object *args;
{
	short arg1 ;
	short arg2 ;
	short arg3 ;
	short arg4 ;
	getscrmask( & arg1 , & arg2 , & arg3 , & arg4 );
	{ object *v = newtupleobject( 4 );
	  if (v == NULL) return NULL;
	  settupleitem(v, 0, mknewshortobject(arg1));
	  settupleitem(v, 1, mknewshortobject(arg2));
	  settupleitem(v, 2, mknewshortobject(arg3));
	  settupleitem(v, 3, mknewshortobject(arg4));
	  return v;
	}
}

/* void getmcolor short s short r short r short r */

static object *
gl_getmcolor(self, args)
	object *self;
	object *args;
{
	short arg1 ;
	short arg2 ;
	short arg3 ;
	short arg4 ;
	if (!getishortarg(args, 1, 0, &arg1))
		return NULL;
	getmcolor( arg1 , & arg2 , & arg3 , & arg4 );
	{ object *v = newtupleobject( 3 );
	  if (v == NULL) return NULL;
	  settupleitem(v, 0, mknewshortobject(arg2));
	  settupleitem(v, 1, mknewshortobject(arg3));
	  settupleitem(v, 2, mknewshortobject(arg4));
	  return v;
	}
}

/* void mapw long s short s short s float r float r float r float r float r float r */

static object *
gl_mapw(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	short arg2 ;
	short arg3 ;
	float arg4 ;
	float arg5 ;
	float arg6 ;
	float arg7 ;
	float arg8 ;
	float arg9 ;
	if (!getilongarg(args, 3, 0, &arg1))
		return NULL;
	if (!getishortarg(args, 3, 1, &arg2))
		return NULL;
	if (!getishortarg(args, 3, 2, &arg3))
		return NULL;
	mapw( arg1 , arg2 , arg3 , & arg4 , & arg5 , & arg6 , & arg7 , & arg8 , & arg9 );
	{ object *v = newtupleobject( 6 );
	  if (v == NULL) return NULL;
	  settupleitem(v, 0, mknewfloatobject(arg4));
	  settupleitem(v, 1, mknewfloatobject(arg5));
	  settupleitem(v, 2, mknewfloatobject(arg6));
	  settupleitem(v, 3, mknewfloatobject(arg7));
	  settupleitem(v, 4, mknewfloatobject(arg8));
	  settupleitem(v, 5, mknewfloatobject(arg9));
	  return v;
	}
}

/* void mapw2 long s short s short s float r float r */

static object *
gl_mapw2(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	short arg2 ;
	short arg3 ;
	float arg4 ;
	float arg5 ;
	if (!getilongarg(args, 3, 0, &arg1))
		return NULL;
	if (!getishortarg(args, 3, 1, &arg2))
		return NULL;
	if (!getishortarg(args, 3, 2, &arg3))
		return NULL;
	mapw2( arg1 , arg2 , arg3 , & arg4 , & arg5 );
	{ object *v = newtupleobject( 2 );
	  if (v == NULL) return NULL;
	  settupleitem(v, 0, mknewfloatobject(arg4));
	  settupleitem(v, 1, mknewfloatobject(arg5));
	  return v;
	}
}

/* long qread short r */

static object *
gl_qread(self, args)
	object *self;
	object *args;
{
	long retval;
	short arg1 ;
	retval = qread( & arg1 );
	{ object *v = newtupleobject( 2 );
	  if (v == NULL) return NULL;
	  settupleitem(v, 0, mknewlongobject(retval));
	  settupleitem(v, 1, mknewshortobject(arg1));
	  return v;
	}
}

/* void getcursor short r short r short r long r */

static object *
gl_getcursor(self, args)
	object *self;
	object *args;
{
	short arg1 ;
	short arg2 ;
	short arg3 ;
	long arg4 ;
	getcursor( & arg1 , & arg2 , & arg3 , & arg4 );
	{ object *v = newtupleobject( 4 );
	  if (v == NULL) return NULL;
	  settupleitem(v, 0, mknewshortobject(arg1));
	  settupleitem(v, 1, mknewshortobject(arg2));
	  settupleitem(v, 2, mknewshortobject(arg3));
	  settupleitem(v, 3, mknewlongobject(arg4));
	  return v;
	}
}

/* void getdev long s short s[arg1] short r[arg1] */
/* XXX Too complicated to generate code for */

/* long readpixels short s short r[retval] */
/* XXX Too complicated to generate code for */

/* long readRGB short s char r[retval] char r[retval] char r[retval] */
/* XXX Too complicated to generate code for */

/* long blkqread short s short r[arg1] */
/* XXX Too complicated to generate code for */

/* void cmode */

static object *
gl_cmode(self, args)
	object *self;
	object *args;
{
	cmode( );
	INCREF(None);
	return None;
}

/* void concave long s */

static object *
gl_concave(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	if (!getilongarg(args, 1, 0, &arg1))
		return NULL;
	concave( arg1 );
	INCREF(None);
	return None;
}

/* void curstype long s */

static object *
gl_curstype(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	if (!getilongarg(args, 1, 0, &arg1))
		return NULL;
	curstype( arg1 );
	INCREF(None);
	return None;
}

/* void drawmode long s */

static object *
gl_drawmode(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	if (!getilongarg(args, 1, 0, &arg1))
		return NULL;
	drawmode( arg1 );
	INCREF(None);
	return None;
}

/* void gammaramp short s[256] short s[256] short s[256] */

static object *
gl_gammaramp(self, args)
	object *self;
	object *args;
{
	short arg1 [ 256 ] ;
	short arg2 [ 256 ] ;
	short arg3 [ 256 ] ;
	if (!getishortarray(args, 3, 0, 256 , arg1))
		return NULL;
	if (!getishortarray(args, 3, 1, 256 , arg2))
		return NULL;
	if (!getishortarray(args, 3, 2, 256 , arg3))
		return NULL;
	gammaramp( arg1 , arg2 , arg3 );
	INCREF(None);
	return None;
}

/* long getbackface */

static object *
gl_getbackface(self, args)
	object *self;
	object *args;
{
	long retval;
	retval = getbackface( );
	return mknewlongobject(retval);
}

/* long getdescender */

static object *
gl_getdescender(self, args)
	object *self;
	object *args;
{
	long retval;
	retval = getdescender( );
	return mknewlongobject(retval);
}

/* long getdrawmode */

static object *
gl_getdrawmode(self, args)
	object *self;
	object *args;
{
	long retval;
	retval = getdrawmode( );
	return mknewlongobject(retval);
}

/* long getmmode */

static object *
gl_getmmode(self, args)
	object *self;
	object *args;
{
	long retval;
	retval = getmmode( );
	return mknewlongobject(retval);
}

/* long getsm */

static object *
gl_getsm(self, args)
	object *self;
	object *args;
{
	long retval;
	retval = getsm( );
	return mknewlongobject(retval);
}

/* long getvideo long s */

static object *
gl_getvideo(self, args)
	object *self;
	object *args;
{
	long retval;
	long arg1 ;
	if (!getilongarg(args, 1, 0, &arg1))
		return NULL;
	retval = getvideo( arg1 );
	return mknewlongobject(retval);
}

/* void imakebackground */

static object *
gl_imakebackground(self, args)
	object *self;
	object *args;
{
	imakebackground( );
	INCREF(None);
	return None;
}

/* void lmbind short s short s */

static object *
gl_lmbind(self, args)
	object *self;
	object *args;
{
	short arg1 ;
	short arg2 ;
	if (!getishortarg(args, 2, 0, &arg1))
		return NULL;
	if (!getishortarg(args, 2, 1, &arg2))
		return NULL;
	lmbind( arg1 , arg2 );
	INCREF(None);
	return None;
}

/* void lmdef long s long s long s float s[arg3] */

static object *
gl_lmdef(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	long arg2 ;
	long arg3 ;
	float * arg4 ;
	if (!getilongarg(args, 3, 0, &arg1))
		return NULL;
	if (!getilongarg(args, 3, 1, &arg2))
		return NULL;
	if (!getilongarraysize(args, 3, 2, &arg3))
		return NULL;
	if ((arg4 = NEW(float, arg3 )) == NULL)
		return err_nomem();
	if (!getifloatarray(args, 3, 2, arg3 , arg4))
		return NULL;
	lmdef( arg1 , arg2 , arg3 , arg4 );
	DEL(arg4);
	INCREF(None);
	return None;
}

/* void mmode long s */

static object *
gl_mmode(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	if (!getilongarg(args, 1, 0, &arg1))
		return NULL;
	mmode( arg1 );
	INCREF(None);
	return None;
}

/* void normal float s[3] */

static object *
gl_normal(self, args)
	object *self;
	object *args;
{
	float arg1 [ 3 ] ;
	if (!getifloatarray(args, 1, 0, 3 , arg1))
		return NULL;
	normal( arg1 );
	INCREF(None);
	return None;
}

/* void overlay long s */

static object *
gl_overlay(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	if (!getilongarg(args, 1, 0, &arg1))
		return NULL;
	overlay( arg1 );
	INCREF(None);
	return None;
}

/* void RGBrange short s short s short s short s short s short s short s short s */

static object *
gl_RGBrange(self, args)
	object *self;
	object *args;
{
	short arg1 ;
	short arg2 ;
	short arg3 ;
	short arg4 ;
	short arg5 ;
	short arg6 ;
	short arg7 ;
	short arg8 ;
	if (!getishortarg(args, 8, 0, &arg1))
		return NULL;
	if (!getishortarg(args, 8, 1, &arg2))
		return NULL;
	if (!getishortarg(args, 8, 2, &arg3))
		return NULL;
	if (!getishortarg(args, 8, 3, &arg4))
		return NULL;
	if (!getishortarg(args, 8, 4, &arg5))
		return NULL;
	if (!getishortarg(args, 8, 5, &arg6))
		return NULL;
	if (!getishortarg(args, 8, 6, &arg7))
		return NULL;
	if (!getishortarg(args, 8, 7, &arg8))
		return NULL;
	RGBrange( arg1 , arg2 , arg3 , arg4 , arg5 , arg6 , arg7 , arg8 );
	INCREF(None);
	return None;
}

/* void setvideo long s long s */

static object *
gl_setvideo(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	long arg2 ;
	if (!getilongarg(args, 2, 0, &arg1))
		return NULL;
	if (!getilongarg(args, 2, 1, &arg2))
		return NULL;
	setvideo( arg1 , arg2 );
	INCREF(None);
	return None;
}

/* void shademodel long s */

static object *
gl_shademodel(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	if (!getilongarg(args, 1, 0, &arg1))
		return NULL;
	shademodel( arg1 );
	INCREF(None);
	return None;
}

/* void underlay long s */

static object *
gl_underlay(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	if (!getilongarg(args, 1, 0, &arg1))
		return NULL;
	underlay( arg1 );
	INCREF(None);
	return None;
}

/* void bgnclosedline */

static object *
gl_bgnclosedline(self, args)
	object *self;
	object *args;
{
	bgnclosedline( );
	INCREF(None);
	return None;
}

/* void bgnline */

static object *
gl_bgnline(self, args)
	object *self;
	object *args;
{
	bgnline( );
	INCREF(None);
	return None;
}

/* void bgnpoint */

static object *
gl_bgnpoint(self, args)
	object *self;
	object *args;
{
	bgnpoint( );
	INCREF(None);
	return None;
}

/* void bgnpolygon */

static object *
gl_bgnpolygon(self, args)
	object *self;
	object *args;
{
	bgnpolygon( );
	INCREF(None);
	return None;
}

/* void bgnsurface */

static object *
gl_bgnsurface(self, args)
	object *self;
	object *args;
{
	bgnsurface( );
	INCREF(None);
	return None;
}

/* void bgntmesh */

static object *
gl_bgntmesh(self, args)
	object *self;
	object *args;
{
	bgntmesh( );
	INCREF(None);
	return None;
}

/* void bgntrim */

static object *
gl_bgntrim(self, args)
	object *self;
	object *args;
{
	bgntrim( );
	INCREF(None);
	return None;
}

/* void endclosedline */

static object *
gl_endclosedline(self, args)
	object *self;
	object *args;
{
	endclosedline( );
	INCREF(None);
	return None;
}

/* void endline */

static object *
gl_endline(self, args)
	object *self;
	object *args;
{
	endline( );
	INCREF(None);
	return None;
}

/* void endpoint */

static object *
gl_endpoint(self, args)
	object *self;
	object *args;
{
	endpoint( );
	INCREF(None);
	return None;
}

/* void endpolygon */

static object *
gl_endpolygon(self, args)
	object *self;
	object *args;
{
	endpolygon( );
	INCREF(None);
	return None;
}

/* void endsurface */

static object *
gl_endsurface(self, args)
	object *self;
	object *args;
{
	endsurface( );
	INCREF(None);
	return None;
}

/* void endtmesh */

static object *
gl_endtmesh(self, args)
	object *self;
	object *args;
{
	endtmesh( );
	INCREF(None);
	return None;
}

/* void endtrim */

static object *
gl_endtrim(self, args)
	object *self;
	object *args;
{
	endtrim( );
	INCREF(None);
	return None;
}

/* void blendfunction long s long s */

static object *
gl_blendfunction(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	long arg2 ;
	if (!getilongarg(args, 2, 0, &arg1))
		return NULL;
	if (!getilongarg(args, 2, 1, &arg2))
		return NULL;
	blendfunction( arg1 , arg2 );
	INCREF(None);
	return None;
}

/* void c3f float s[3] */

static object *
gl_c3f(self, args)
	object *self;
	object *args;
{
	float arg1 [ 3 ] ;
	if (!getifloatarray(args, 1, 0, 3 , arg1))
		return NULL;
	c3f( arg1 );
	INCREF(None);
	return None;
}

/* void c3i long s[3] */

static object *
gl_c3i(self, args)
	object *self;
	object *args;
{
	long arg1 [ 3 ] ;
	if (!getilongarray(args, 1, 0, 3 , arg1))
		return NULL;
	c3i( arg1 );
	INCREF(None);
	return None;
}

/* void c3s short s[3] */

static object *
gl_c3s(self, args)
	object *self;
	object *args;
{
	short arg1 [ 3 ] ;
	if (!getishortarray(args, 1, 0, 3 , arg1))
		return NULL;
	c3s( arg1 );
	INCREF(None);
	return None;
}

/* void c4f float s[4] */

static object *
gl_c4f(self, args)
	object *self;
	object *args;
{
	float arg1 [ 4 ] ;
	if (!getifloatarray(args, 1, 0, 4 , arg1))
		return NULL;
	c4f( arg1 );
	INCREF(None);
	return None;
}

/* void c4i long s[4] */

static object *
gl_c4i(self, args)
	object *self;
	object *args;
{
	long arg1 [ 4 ] ;
	if (!getilongarray(args, 1, 0, 4 , arg1))
		return NULL;
	c4i( arg1 );
	INCREF(None);
	return None;
}

/* void c4s short s[4] */

static object *
gl_c4s(self, args)
	object *self;
	object *args;
{
	short arg1 [ 4 ] ;
	if (!getishortarray(args, 1, 0, 4 , arg1))
		return NULL;
	c4s( arg1 );
	INCREF(None);
	return None;
}

/* void colorf float s */

static object *
gl_colorf(self, args)
	object *self;
	object *args;
{
	float arg1 ;
	if (!getifloatarg(args, 1, 0, &arg1))
		return NULL;
	colorf( arg1 );
	INCREF(None);
	return None;
}

/* void cpack long s */

static object *
gl_cpack(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	if (!getilongarg(args, 1, 0, &arg1))
		return NULL;
	cpack( arg1 );
	INCREF(None);
	return None;
}

/* void czclear long s long s */

static object *
gl_czclear(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	long arg2 ;
	if (!getilongarg(args, 2, 0, &arg1))
		return NULL;
	if (!getilongarg(args, 2, 1, &arg2))
		return NULL;
	czclear( arg1 , arg2 );
	INCREF(None);
	return None;
}

/* void dglclose long s */

static object *
gl_dglclose(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	if (!getilongarg(args, 1, 0, &arg1))
		return NULL;
	dglclose( arg1 );
	INCREF(None);
	return None;
}

/* long dglopen char *s long s */

static object *
gl_dglopen(self, args)
	object *self;
	object *args;
{
	long retval;
	string arg1 ;
	long arg2 ;
	if (!getistringarg(args, 2, 0, &arg1))
		return NULL;
	if (!getilongarg(args, 2, 1, &arg2))
		return NULL;
	retval = dglopen( arg1 , arg2 );
	return mknewlongobject(retval);
}

/* long getgdesc long s */

static object *
gl_getgdesc(self, args)
	object *self;
	object *args;
{
	long retval;
	long arg1 ;
	if (!getilongarg(args, 1, 0, &arg1))
		return NULL;
	retval = getgdesc( arg1 );
	return mknewlongobject(retval);
}

/* void getnurbsproperty long s float r */

static object *
gl_getnurbsproperty(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	float arg2 ;
	if (!getilongarg(args, 1, 0, &arg1))
		return NULL;
	getnurbsproperty( arg1 , & arg2 );
	return mknewfloatobject(arg2);
}

/* void glcompat long s long s */

static object *
gl_glcompat(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	long arg2 ;
	if (!getilongarg(args, 2, 0, &arg1))
		return NULL;
	if (!getilongarg(args, 2, 1, &arg2))
		return NULL;
	glcompat( arg1 , arg2 );
	INCREF(None);
	return None;
}

/* void iconsize long s long s */

static object *
gl_iconsize(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	long arg2 ;
	if (!getilongarg(args, 2, 0, &arg1))
		return NULL;
	if (!getilongarg(args, 2, 1, &arg2))
		return NULL;
	iconsize( arg1 , arg2 );
	INCREF(None);
	return None;
}

/* void icontitle char *s */

static object *
gl_icontitle(self, args)
	object *self;
	object *args;
{
	string arg1 ;
	if (!getistringarg(args, 1, 0, &arg1))
		return NULL;
	icontitle( arg1 );
	INCREF(None);
	return None;
}

/* void lRGBrange short s short s short s short s short s short s long s long s */

static object *
gl_lRGBrange(self, args)
	object *self;
	object *args;
{
	short arg1 ;
	short arg2 ;
	short arg3 ;
	short arg4 ;
	short arg5 ;
	short arg6 ;
	long arg7 ;
	long arg8 ;
	if (!getishortarg(args, 8, 0, &arg1))
		return NULL;
	if (!getishortarg(args, 8, 1, &arg2))
		return NULL;
	if (!getishortarg(args, 8, 2, &arg3))
		return NULL;
	if (!getishortarg(args, 8, 3, &arg4))
		return NULL;
	if (!getishortarg(args, 8, 4, &arg5))
		return NULL;
	if (!getishortarg(args, 8, 5, &arg6))
		return NULL;
	if (!getilongarg(args, 8, 6, &arg7))
		return NULL;
	if (!getilongarg(args, 8, 7, &arg8))
		return NULL;
	lRGBrange( arg1 , arg2 , arg3 , arg4 , arg5 , arg6 , arg7 , arg8 );
	INCREF(None);
	return None;
}

/* void linesmooth long s */

static object *
gl_linesmooth(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	if (!getilongarg(args, 1, 0, &arg1))
		return NULL;
	linesmooth( arg1 );
	INCREF(None);
	return None;
}

/* void lmcolor long s */

static object *
gl_lmcolor(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	if (!getilongarg(args, 1, 0, &arg1))
		return NULL;
	lmcolor( arg1 );
	INCREF(None);
	return None;
}

/* void logicop long s */

static object *
gl_logicop(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	if (!getilongarg(args, 1, 0, &arg1))
		return NULL;
	logicop( arg1 );
	INCREF(None);
	return None;
}

/* long lrectread short s short s short s short s long r[retval] */
/* XXX Too complicated to generate code for */

/* long rectread short s short s short s short s short r[retval] */
/* XXX Too complicated to generate code for */

/* void lsetdepth long s long s */

static object *
gl_lsetdepth(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	long arg2 ;
	if (!getilongarg(args, 2, 0, &arg1))
		return NULL;
	if (!getilongarg(args, 2, 1, &arg2))
		return NULL;
	lsetdepth( arg1 , arg2 );
	INCREF(None);
	return None;
}

/* void lshaderange short s short s long s long s */

static object *
gl_lshaderange(self, args)
	object *self;
	object *args;
{
	short arg1 ;
	short arg2 ;
	long arg3 ;
	long arg4 ;
	if (!getishortarg(args, 4, 0, &arg1))
		return NULL;
	if (!getishortarg(args, 4, 1, &arg2))
		return NULL;
	if (!getilongarg(args, 4, 2, &arg3))
		return NULL;
	if (!getilongarg(args, 4, 3, &arg4))
		return NULL;
	lshaderange( arg1 , arg2 , arg3 , arg4 );
	INCREF(None);
	return None;
}

/* void n3f float s[3] */

static object *
gl_n3f(self, args)
	object *self;
	object *args;
{
	float arg1 [ 3 ] ;
	if (!getifloatarray(args, 1, 0, 3 , arg1))
		return NULL;
	n3f( arg1 );
	INCREF(None);
	return None;
}

/* void noborder */

static object *
gl_noborder(self, args)
	object *self;
	object *args;
{
	noborder( );
	INCREF(None);
	return None;
}

/* void pntsmooth long s */

static object *
gl_pntsmooth(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	if (!getilongarg(args, 1, 0, &arg1))
		return NULL;
	pntsmooth( arg1 );
	INCREF(None);
	return None;
}

/* void readsource long s */

static object *
gl_readsource(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	if (!getilongarg(args, 1, 0, &arg1))
		return NULL;
	readsource( arg1 );
	INCREF(None);
	return None;
}

/* void rectzoom float s float s */

static object *
gl_rectzoom(self, args)
	object *self;
	object *args;
{
	float arg1 ;
	float arg2 ;
	if (!getifloatarg(args, 2, 0, &arg1))
		return NULL;
	if (!getifloatarg(args, 2, 1, &arg2))
		return NULL;
	rectzoom( arg1 , arg2 );
	INCREF(None);
	return None;
}

/* void sbox float s float s float s float s */

static object *
gl_sbox(self, args)
	object *self;
	object *args;
{
	float arg1 ;
	float arg2 ;
	float arg3 ;
	float arg4 ;
	if (!getifloatarg(args, 4, 0, &arg1))
		return NULL;
	if (!getifloatarg(args, 4, 1, &arg2))
		return NULL;
	if (!getifloatarg(args, 4, 2, &arg3))
		return NULL;
	if (!getifloatarg(args, 4, 3, &arg4))
		return NULL;
	sbox( arg1 , arg2 , arg3 , arg4 );
	INCREF(None);
	return None;
}

/* void sboxi long s long s long s long s */

static object *
gl_sboxi(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	long arg2 ;
	long arg3 ;
	long arg4 ;
	if (!getilongarg(args, 4, 0, &arg1))
		return NULL;
	if (!getilongarg(args, 4, 1, &arg2))
		return NULL;
	if (!getilongarg(args, 4, 2, &arg3))
		return NULL;
	if (!getilongarg(args, 4, 3, &arg4))
		return NULL;
	sboxi( arg1 , arg2 , arg3 , arg4 );
	INCREF(None);
	return None;
}

/* void sboxs short s short s short s short s */

static object *
gl_sboxs(self, args)
	object *self;
	object *args;
{
	short arg1 ;
	short arg2 ;
	short arg3 ;
	short arg4 ;
	if (!getishortarg(args, 4, 0, &arg1))
		return NULL;
	if (!getishortarg(args, 4, 1, &arg2))
		return NULL;
	if (!getishortarg(args, 4, 2, &arg3))
		return NULL;
	if (!getishortarg(args, 4, 3, &arg4))
		return NULL;
	sboxs( arg1 , arg2 , arg3 , arg4 );
	INCREF(None);
	return None;
}

/* void sboxf float s float s float s float s */

static object *
gl_sboxf(self, args)
	object *self;
	object *args;
{
	float arg1 ;
	float arg2 ;
	float arg3 ;
	float arg4 ;
	if (!getifloatarg(args, 4, 0, &arg1))
		return NULL;
	if (!getifloatarg(args, 4, 1, &arg2))
		return NULL;
	if (!getifloatarg(args, 4, 2, &arg3))
		return NULL;
	if (!getifloatarg(args, 4, 3, &arg4))
		return NULL;
	sboxf( arg1 , arg2 , arg3 , arg4 );
	INCREF(None);
	return None;
}

/* void sboxfi long s long s long s long s */

static object *
gl_sboxfi(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	long arg2 ;
	long arg3 ;
	long arg4 ;
	if (!getilongarg(args, 4, 0, &arg1))
		return NULL;
	if (!getilongarg(args, 4, 1, &arg2))
		return NULL;
	if (!getilongarg(args, 4, 2, &arg3))
		return NULL;
	if (!getilongarg(args, 4, 3, &arg4))
		return NULL;
	sboxfi( arg1 , arg2 , arg3 , arg4 );
	INCREF(None);
	return None;
}

/* void sboxfs short s short s short s short s */

static object *
gl_sboxfs(self, args)
	object *self;
	object *args;
{
	short arg1 ;
	short arg2 ;
	short arg3 ;
	short arg4 ;
	if (!getishortarg(args, 4, 0, &arg1))
		return NULL;
	if (!getishortarg(args, 4, 1, &arg2))
		return NULL;
	if (!getishortarg(args, 4, 2, &arg3))
		return NULL;
	if (!getishortarg(args, 4, 3, &arg4))
		return NULL;
	sboxfs( arg1 , arg2 , arg3 , arg4 );
	INCREF(None);
	return None;
}

/* void setnurbsproperty long s float s */

static object *
gl_setnurbsproperty(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	float arg2 ;
	if (!getilongarg(args, 2, 0, &arg1))
		return NULL;
	if (!getifloatarg(args, 2, 1, &arg2))
		return NULL;
	setnurbsproperty( arg1 , arg2 );
	INCREF(None);
	return None;
}

/* void setpup long s long s long s */

static object *
gl_setpup(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	long arg2 ;
	long arg3 ;
	if (!getilongarg(args, 3, 0, &arg1))
		return NULL;
	if (!getilongarg(args, 3, 1, &arg2))
		return NULL;
	if (!getilongarg(args, 3, 2, &arg3))
		return NULL;
	setpup( arg1 , arg2 , arg3 );
	INCREF(None);
	return None;
}

/* void smoothline long s */

static object *
gl_smoothline(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	if (!getilongarg(args, 1, 0, &arg1))
		return NULL;
	smoothline( arg1 );
	INCREF(None);
	return None;
}

/* void subpixel long s */

static object *
gl_subpixel(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	if (!getilongarg(args, 1, 0, &arg1))
		return NULL;
	subpixel( arg1 );
	INCREF(None);
	return None;
}

/* void swaptmesh */

static object *
gl_swaptmesh(self, args)
	object *self;
	object *args;
{
	swaptmesh( );
	INCREF(None);
	return None;
}

/* long swinopen long s */

static object *
gl_swinopen(self, args)
	object *self;
	object *args;
{
	long retval;
	long arg1 ;
	if (!getilongarg(args, 1, 0, &arg1))
		return NULL;
	retval = swinopen( arg1 );
	return mknewlongobject(retval);
}

/* void v2f float s[2] */

static object *
gl_v2f(self, args)
	object *self;
	object *args;
{
	float arg1 [ 2 ] ;
	if (!getifloatarray(args, 1, 0, 2 , arg1))
		return NULL;
	v2f( arg1 );
	INCREF(None);
	return None;
}

/* void v2i long s[2] */

static object *
gl_v2i(self, args)
	object *self;
	object *args;
{
	long arg1 [ 2 ] ;
	if (!getilongarray(args, 1, 0, 2 , arg1))
		return NULL;
	v2i( arg1 );
	INCREF(None);
	return None;
}

/* void v2s short s[2] */

static object *
gl_v2s(self, args)
	object *self;
	object *args;
{
	short arg1 [ 2 ] ;
	if (!getishortarray(args, 1, 0, 2 , arg1))
		return NULL;
	v2s( arg1 );
	INCREF(None);
	return None;
}

/* void v3f float s[3] */

static object *
gl_v3f(self, args)
	object *self;
	object *args;
{
	float arg1 [ 3 ] ;
	if (!getifloatarray(args, 1, 0, 3 , arg1))
		return NULL;
	v3f( arg1 );
	INCREF(None);
	return None;
}

/* void v3i long s[3] */

static object *
gl_v3i(self, args)
	object *self;
	object *args;
{
	long arg1 [ 3 ] ;
	if (!getilongarray(args, 1, 0, 3 , arg1))
		return NULL;
	v3i( arg1 );
	INCREF(None);
	return None;
}

/* void v3s short s[3] */

static object *
gl_v3s(self, args)
	object *self;
	object *args;
{
	short arg1 [ 3 ] ;
	if (!getishortarray(args, 1, 0, 3 , arg1))
		return NULL;
	v3s( arg1 );
	INCREF(None);
	return None;
}

/* void v4f float s[4] */

static object *
gl_v4f(self, args)
	object *self;
	object *args;
{
	float arg1 [ 4 ] ;
	if (!getifloatarray(args, 1, 0, 4 , arg1))
		return NULL;
	v4f( arg1 );
	INCREF(None);
	return None;
}

/* void v4i long s[4] */

static object *
gl_v4i(self, args)
	object *self;
	object *args;
{
	long arg1 [ 4 ] ;
	if (!getilongarray(args, 1, 0, 4 , arg1))
		return NULL;
	v4i( arg1 );
	INCREF(None);
	return None;
}

/* void v4s short s[4] */

static object *
gl_v4s(self, args)
	object *self;
	object *args;
{
	short arg1 [ 4 ] ;
	if (!getishortarray(args, 1, 0, 4 , arg1))
		return NULL;
	v4s( arg1 );
	INCREF(None);
	return None;
}

/* void videocmd long s */

static object *
gl_videocmd(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	if (!getilongarg(args, 1, 0, &arg1))
		return NULL;
	videocmd( arg1 );
	INCREF(None);
	return None;
}

/* long windepth long s */

static object *
gl_windepth(self, args)
	object *self;
	object *args;
{
	long retval;
	long arg1 ;
	if (!getilongarg(args, 1, 0, &arg1))
		return NULL;
	retval = windepth( arg1 );
	return mknewlongobject(retval);
}

/* void wmpack long s */

static object *
gl_wmpack(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	if (!getilongarg(args, 1, 0, &arg1))
		return NULL;
	wmpack( arg1 );
	INCREF(None);
	return None;
}

/* void zdraw long s */

static object *
gl_zdraw(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	if (!getilongarg(args, 1, 0, &arg1))
		return NULL;
	zdraw( arg1 );
	INCREF(None);
	return None;
}

/* void zfunction long s */

static object *
gl_zfunction(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	if (!getilongarg(args, 1, 0, &arg1))
		return NULL;
	zfunction( arg1 );
	INCREF(None);
	return None;
}

/* void zsource long s */

static object *
gl_zsource(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	if (!getilongarg(args, 1, 0, &arg1))
		return NULL;
	zsource( arg1 );
	INCREF(None);
	return None;
}

/* void zwritemask long s */

static object *
gl_zwritemask(self, args)
	object *self;
	object *args;
{
	long arg1 ;
	if (!getilongarg(args, 1, 0, &arg1))
		return NULL;
	zwritemask( arg1 );
	INCREF(None);
	return None;
}

/* void v2d double s[2] */

static object *
gl_v2d(self, args)
	object *self;
	object *args;
{
	double arg1 [ 2 ] ;
	if (!getidoublearray(args, 1, 0, 2 , arg1))
		return NULL;
	v2d( arg1 );
	INCREF(None);
	return None;
}

/* void v3d double s[3] */

static object *
gl_v3d(self, args)
	object *self;
	object *args;
{
	double arg1 [ 3 ] ;
	if (!getidoublearray(args, 1, 0, 3 , arg1))
		return NULL;
	v3d( arg1 );
	INCREF(None);
	return None;
}

/* void v4d double s[4] */

static object *
gl_v4d(self, args)
	object *self;
	object *args;
{
	double arg1 [ 4 ] ;
	if (!getidoublearray(args, 1, 0, 4 , arg1))
		return NULL;
	v4d( arg1 );
	INCREF(None);
	return None;
}

static struct methodlist gl_methods[] = {
	{"varray", gl_varray},
	{"nvarray", gl_nvarray},
	{"vnarray", gl_vnarray},
	{"nurbssurface", gl_nurbssurface},
	{"nurbscurve", gl_nurbscurve},
	{"pwlcurve", gl_pwlcurve},
	{"pick", gl_pick},
	{"endpick", gl_endpick},
	{"gselect", gl_gselect},
	{"endselect", gl_endselect},
	{"getmatrix", gl_getmatrix},
	{"getshade", gl_getshade},
	{"devport", gl_devport},
	{"rdr2i", gl_rdr2i},
	{"rectfs", gl_rectfs},
	{"rects", gl_rects},
	{"rmv2i", gl_rmv2i},
	{"noport", gl_noport},
	{"popviewport", gl_popviewport},
	{"clear", gl_clear},
	{"clearhitcode", gl_clearhitcode},
	{"closeobj", gl_closeobj},
	{"cursoff", gl_cursoff},
	{"curson", gl_curson},
	{"doublebuffer", gl_doublebuffer},
	{"finish", gl_finish},
	{"gconfig", gl_gconfig},
	{"ginit", gl_ginit},
	{"greset", gl_greset},
	{"multimap", gl_multimap},
	{"onemap", gl_onemap},
	{"popattributes", gl_popattributes},
	{"popmatrix", gl_popmatrix},
	{"pushattributes", gl_pushattributes},
	{"pushmatrix", gl_pushmatrix},
	{"pushviewport", gl_pushviewport},
	{"qreset", gl_qreset},
	{"RGBmode", gl_RGBmode},
	{"singlebuffer", gl_singlebuffer},
	{"swapbuffers", gl_swapbuffers},
	{"gsync", gl_gsync},
	{"tpon", gl_tpon},
	{"tpoff", gl_tpoff},
	{"clkon", gl_clkon},
	{"clkoff", gl_clkoff},
	{"ringbell", gl_ringbell},
	{"gbegin", gl_gbegin},
	{"textinit", gl_textinit},
	{"initnames", gl_initnames},
	{"pclos", gl_pclos},
	{"popname", gl_popname},
	{"spclos", gl_spclos},
	{"zclear", gl_zclear},
	{"screenspace", gl_screenspace},
	{"reshapeviewport", gl_reshapeviewport},
	{"winpush", gl_winpush},
	{"winpop", gl_winpop},
	{"foreground", gl_foreground},
	{"endfullscrn", gl_endfullscrn},
	{"endpupmode", gl_endpupmode},
	{"fullscrn", gl_fullscrn},
	{"pupmode", gl_pupmode},
	{"winconstraints", gl_winconstraints},
	{"pagecolor", gl_pagecolor},
	{"textcolor", gl_textcolor},
	{"color", gl_color},
	{"curveit", gl_curveit},
	{"font", gl_font},
	{"linewidth", gl_linewidth},
	{"setlinestyle", gl_setlinestyle},
	{"setmap", gl_setmap},
	{"swapinterval", gl_swapinterval},
	{"writemask", gl_writemask},
	{"textwritemask", gl_textwritemask},
	{"qdevice", gl_qdevice},
	{"unqdevice", gl_unqdevice},
	{"curvebasis", gl_curvebasis},
	{"curveprecision", gl_curveprecision},
	{"loadname", gl_loadname},
	{"passthrough", gl_passthrough},
	{"pushname", gl_pushname},
	{"setmonitor", gl_setmonitor},
	{"setshade", gl_setshade},
	{"setpattern", gl_setpattern},
	{"pagewritemask", gl_pagewritemask},
	{"callobj", gl_callobj},
	{"delobj", gl_delobj},
	{"editobj", gl_editobj},
	{"makeobj", gl_makeobj},
	{"maketag", gl_maketag},
	{"chunksize", gl_chunksize},
	{"compactify", gl_compactify},
	{"deltag", gl_deltag},
	{"lsrepeat", gl_lsrepeat},
	{"objinsert", gl_objinsert},
	{"objreplace", gl_objreplace},
	{"winclose", gl_winclose},
	{"blanktime", gl_blanktime},
	{"freepup", gl_freepup},
	{"backbuffer", gl_backbuffer},
	{"frontbuffer", gl_frontbuffer},
	{"lsbackup", gl_lsbackup},
	{"resetls", gl_resetls},
	{"lampon", gl_lampon},
	{"lampoff", gl_lampoff},
	{"setbell", gl_setbell},
	{"blankscreen", gl_blankscreen},
	{"depthcue", gl_depthcue},
	{"zbuffer", gl_zbuffer},
	{"backface", gl_backface},
	{"cmov2i", gl_cmov2i},
	{"draw2i", gl_draw2i},
	{"move2i", gl_move2i},
	{"pnt2i", gl_pnt2i},
	{"patchbasis", gl_patchbasis},
	{"patchprecision", gl_patchprecision},
	{"pdr2i", gl_pdr2i},
	{"pmv2i", gl_pmv2i},
	{"rpdr2i", gl_rpdr2i},
	{"rpmv2i", gl_rpmv2i},
	{"xfpt2i", gl_xfpt2i},
	{"objdelete", gl_objdelete},
	{"patchcurves", gl_patchcurves},
	{"minsize", gl_minsize},
	{"maxsize", gl_maxsize},
	{"keepaspect", gl_keepaspect},
	{"prefsize", gl_prefsize},
	{"stepunit", gl_stepunit},
	{"fudge", gl_fudge},
	{"winmove", gl_winmove},
	{"attachcursor", gl_attachcursor},
	{"deflinestyle", gl_deflinestyle},
	{"noise", gl_noise},
	{"picksize", gl_picksize},
	{"qenter", gl_qenter},
	{"setdepth", gl_setdepth},
	{"cmov2s", gl_cmov2s},
	{"draw2s", gl_draw2s},
	{"move2s", gl_move2s},
	{"pdr2s", gl_pdr2s},
	{"pmv2s", gl_pmv2s},
	{"pnt2s", gl_pnt2s},
	{"rdr2s", gl_rdr2s},
	{"rmv2s", gl_rmv2s},
	{"rpdr2s", gl_rpdr2s},
	{"rpmv2s", gl_rpmv2s},
	{"xfpt2s", gl_xfpt2s},
	{"cmov2", gl_cmov2},
	{"draw2", gl_draw2},
	{"move2", gl_move2},
	{"pnt2", gl_pnt2},
	{"pdr2", gl_pdr2},
	{"pmv2", gl_pmv2},
	{"rdr2", gl_rdr2},
	{"rmv2", gl_rmv2},
	{"rpdr2", gl_rpdr2},
	{"rpmv2", gl_rpmv2},
	{"xfpt2", gl_xfpt2},
	{"loadmatrix", gl_loadmatrix},
	{"multmatrix", gl_multmatrix},
	{"crv", gl_crv},
	{"rcrv", gl_rcrv},
	{"addtopup", gl_addtopup},
	{"charstr", gl_charstr},
	{"getport", gl_getport},
	{"strwidth", gl_strwidth},
	{"winopen", gl_winopen},
	{"wintitle", gl_wintitle},
	{"polf", gl_polf},
	{"polf2", gl_polf2},
	{"poly", gl_poly},
	{"poly2", gl_poly2},
	{"crvn", gl_crvn},
	{"rcrvn", gl_rcrvn},
	{"polf2i", gl_polf2i},
	{"polfi", gl_polfi},
	{"poly2i", gl_poly2i},
	{"polyi", gl_polyi},
	{"polf2s", gl_polf2s},
	{"polfs", gl_polfs},
	{"polys", gl_polys},
	{"poly2s", gl_poly2s},
	{"defcursor", gl_defcursor},
	{"writepixels", gl_writepixels},
	{"defbasis", gl_defbasis},
	{"gewrite", gl_gewrite},
	{"rotate", gl_rotate},
	{"rot", gl_rot},
	{"circfi", gl_circfi},
	{"circi", gl_circi},
	{"cmovi", gl_cmovi},
	{"drawi", gl_drawi},
	{"movei", gl_movei},
	{"pnti", gl_pnti},
	{"newtag", gl_newtag},
	{"pdri", gl_pdri},
	{"pmvi", gl_pmvi},
	{"rdri", gl_rdri},
	{"rmvi", gl_rmvi},
	{"rpdri", gl_rpdri},
	{"rpmvi", gl_rpmvi},
	{"xfpti", gl_xfpti},
	{"circ", gl_circ},
	{"circf", gl_circf},
	{"cmov", gl_cmov},
	{"draw", gl_draw},
	{"move", gl_move},
	{"pnt", gl_pnt},
	{"scale", gl_scale},
	{"translate", gl_translate},
	{"pdr", gl_pdr},
	{"pmv", gl_pmv},
	{"rdr", gl_rdr},
	{"rmv", gl_rmv},
	{"rpdr", gl_rpdr},
	{"rpmv", gl_rpmv},
	{"xfpt", gl_xfpt},
	{"RGBcolor", gl_RGBcolor},
	{"RGBwritemask", gl_RGBwritemask},
	{"setcursor", gl_setcursor},
	{"tie", gl_tie},
	{"circfs", gl_circfs},
	{"circs", gl_circs},
	{"cmovs", gl_cmovs},
	{"draws", gl_draws},
	{"moves", gl_moves},
	{"pdrs", gl_pdrs},
	{"pmvs", gl_pmvs},
	{"pnts", gl_pnts},
	{"rdrs", gl_rdrs},
	{"rmvs", gl_rmvs},
	{"rpdrs", gl_rpdrs},
	{"rpmvs", gl_rpmvs},
	{"xfpts", gl_xfpts},
	{"curorigin", gl_curorigin},
	{"cyclemap", gl_cyclemap},
	{"patch", gl_patch},
	{"splf", gl_splf},
	{"splf2", gl_splf2},
	{"splfi", gl_splfi},
	{"splf2i", gl_splf2i},
	{"splfs", gl_splfs},
	{"splf2s", gl_splf2s},
	{"rpatch", gl_rpatch},
	{"ortho2", gl_ortho2},
	{"rect", gl_rect},
	{"rectf", gl_rectf},
	{"xfpt4", gl_xfpt4},
	{"textport", gl_textport},
	{"mapcolor", gl_mapcolor},
	{"scrmask", gl_scrmask},
	{"setvaluator", gl_setvaluator},
	{"viewport", gl_viewport},
	{"shaderange", gl_shaderange},
	{"xfpt4s", gl_xfpt4s},
	{"rectfi", gl_rectfi},
	{"recti", gl_recti},
	{"xfpt4i", gl_xfpt4i},
	{"prefposition", gl_prefposition},
	{"arc", gl_arc},
	{"arcf", gl_arcf},
	{"arcfi", gl_arcfi},
	{"arci", gl_arci},
	{"bbox2", gl_bbox2},
	{"bbox2i", gl_bbox2i},
	{"bbox2s", gl_bbox2s},
	{"blink", gl_blink},
	{"ortho", gl_ortho},
	{"window", gl_window},
	{"lookat", gl_lookat},
	{"perspective", gl_perspective},
	{"polarview", gl_polarview},
	{"arcfs", gl_arcfs},
	{"arcs", gl_arcs},
	{"rectcopy", gl_rectcopy},
	{"RGBcursor", gl_RGBcursor},
	{"getbutton", gl_getbutton},
	{"getcmmode", gl_getcmmode},
	{"getlsbackup", gl_getlsbackup},
	{"getresetls", gl_getresetls},
	{"getdcm", gl_getdcm},
	{"getzbuffer", gl_getzbuffer},
	{"ismex", gl_ismex},
	{"isobj", gl_isobj},
	{"isqueued", gl_isqueued},
	{"istag", gl_istag},
	{"genobj", gl_genobj},
	{"gentag", gl_gentag},
	{"getbuffer", gl_getbuffer},
	{"getcolor", gl_getcolor},
	{"getdisplaymode", gl_getdisplaymode},
	{"getfont", gl_getfont},
	{"getheight", gl_getheight},
	{"gethitcode", gl_gethitcode},
	{"getlstyle", gl_getlstyle},
	{"getlwidth", gl_getlwidth},
	{"getmap", gl_getmap},
	{"getplanes", gl_getplanes},
	{"getwritemask", gl_getwritemask},
	{"qtest", gl_qtest},
	{"getlsrepeat", gl_getlsrepeat},
	{"getmonitor", gl_getmonitor},
	{"getopenobj", gl_getopenobj},
	{"getpattern", gl_getpattern},
	{"winget", gl_winget},
Line 783 : ('bad subscript', 'arg2*arg2/16')
Line 877 : ('bad arg mode', '*')
Line 881 : ('bad type', 'Fontchar')
Function getdev too complicated: short r  arg1
Function readpixels too complicated: short r  retval
Function readRGB too complicated: char r  retval
Function blkqread too complicated: short r  arg1
Function lrectread too complicated: long r  retval
Line 956 : ('bad subscript', '(arg2-arg1+1)*(arg4-arg3+1)')
Function rectread too complicated: short r  retval
Line 958 : ('bad subscript', '(arg2-arg1+1)*(arg4-arg3+1)')
	{"winattach", gl_winattach},
	{"getothermonitor", gl_getothermonitor},
	{"newpup", gl_newpup},
	{"getvaluator", gl_getvaluator},
	{"winset", gl_winset},
	{"dopup", gl_dopup},
	{"getdepth", gl_getdepth},
	{"getcpos", gl_getcpos},
	{"getsize", gl_getsize},
	{"getorigin", gl_getorigin},
	{"getviewport", gl_getviewport},
	{"gettp", gl_gettp},
	{"getgpos", gl_getgpos},
	{"winposition", gl_winposition},
	{"gRGBcolor", gl_gRGBcolor},
	{"gRGBmask", gl_gRGBmask},
	{"getscrmask", gl_getscrmask},
	{"getmcolor", gl_getmcolor},
	{"mapw", gl_mapw},
	{"mapw2", gl_mapw2},
	{"qread", gl_qread},
	{"getcursor", gl_getcursor},
	{"cmode", gl_cmode},
	{"concave", gl_concave},
	{"curstype", gl_curstype},
	{"drawmode", gl_drawmode},
	{"gammaramp", gl_gammaramp},
	{"getbackface", gl_getbackface},
	{"getdescender", gl_getdescender},
	{"getdrawmode", gl_getdrawmode},
	{"getmmode", gl_getmmode},
	{"getsm", gl_getsm},
	{"getvideo", gl_getvideo},
	{"imakebackground", gl_imakebackground},
	{"lmbind", gl_lmbind},
	{"lmdef", gl_lmdef},
	{"mmode", gl_mmode},
	{"normal", gl_normal},
	{"overlay", gl_overlay},
	{"RGBrange", gl_RGBrange},
	{"setvideo", gl_setvideo},
	{"shademodel", gl_shademodel},
	{"underlay", gl_underlay},
	{"bgnclosedline", gl_bgnclosedline},
	{"bgnline", gl_bgnline},
	{"bgnpoint", gl_bgnpoint},
	{"bgnpolygon", gl_bgnpolygon},
	{"bgnsurface", gl_bgnsurface},
	{"bgntmesh", gl_bgntmesh},
	{"bgntrim", gl_bgntrim},
	{"endclosedline", gl_endclosedline},
	{"endline", gl_endline},
	{"endpoint", gl_endpoint},
	{"endpolygon", gl_endpolygon},
	{"endsurface", gl_endsurface},
	{"endtmesh", gl_endtmesh},
	{"endtrim", gl_endtrim},
	{"blendfunction", gl_blendfunction},
	{"c3f", gl_c3f},
	{"c3i", gl_c3i},
	{"c3s", gl_c3s},
	{"c4f", gl_c4f},
	{"c4i", gl_c4i},
	{"c4s", gl_c4s},
	{"colorf", gl_colorf},
	{"cpack", gl_cpack},
	{"czclear", gl_czclear},
	{"dglclose", gl_dglclose},
	{"dglopen", gl_dglopen},
	{"getgdesc", gl_getgdesc},
	{"getnurbsproperty", gl_getnurbsproperty},
	{"glcompat", gl_glcompat},
	{"iconsize", gl_iconsize},
	{"icontitle", gl_icontitle},
	{"lRGBrange", gl_lRGBrange},
	{"linesmooth", gl_linesmooth},
	{"lmcolor", gl_lmcolor},
	{"logicop", gl_logicop},
	{"lsetdepth", gl_lsetdepth},
	{"lshaderange", gl_lshaderange},
	{"n3f", gl_n3f},
	{"noborder", gl_noborder},
	{"pntsmooth", gl_pntsmooth},
	{"readsource", gl_readsource},
	{"rectzoom", gl_rectzoom},
	{"sbox", gl_sbox},
	{"sboxi", gl_sboxi},
	{"sboxs", gl_sboxs},
	{"sboxf", gl_sboxf},
	{"sboxfi", gl_sboxfi},
	{"sboxfs", gl_sboxfs},
	{"setnurbsproperty", gl_setnurbsproperty},
	{"setpup", gl_setpup},
	{"smoothline", gl_smoothline},
	{"subpixel", gl_subpixel},
	{"swaptmesh", gl_swaptmesh},
	{"swinopen", gl_swinopen},
	{"v2f", gl_v2f},
	{"v2i", gl_v2i},
	{"v2s", gl_v2s},
	{"v3f", gl_v3f},
	{"v3i", gl_v3i},
	{"v3s", gl_v3s},
	{"v4f", gl_v4f},
	{"v4i", gl_v4i},
	{"v4s", gl_v4s},
	{"videocmd", gl_videocmd},
	{"windepth", gl_windepth},
	{"wmpack", gl_wmpack},
	{"zdraw", gl_zdraw},
	{"zfunction", gl_zfunction},
	{"zsource", gl_zsource},
	{"zwritemask", gl_zwritemask},
	{"v2d", gl_v2d},
	{"v3d", gl_v3d},
	{"v4d", gl_v4d},
	{NULL, NULL} /* Sentinel */
};

initgl()
{
	initmodule("gl", gl_methods);
}
