// =====================================================================================
//  spacetime_hybrid_pbr.cpp
//
//  Renders a Mandelbulb and a Mandelbox, each sitting on a point mass, blended by the
//  space-time geometry of the two masses, shaded with a physically based surface model,
//  2x2 supersampled, multithreaded with OpenMP. Run with no arguments it reproduces
//  a default (all params preset) 1440x1440 image.
//
//  BUILD      g++ -O3 -march=native -ffast-math -fopenmp spacetime_hybrid_pbr.cpp -o shpbr
//  RUN        ./shpbr                         (the reference image; about 30 min on 2 cores;
//                                              rows are rendered in parallel, so more cores = faster)
//             ./shpbr key=value key=value ... (any parameter of SECTION A, see below)
//             ./shpbr help=1                  (prints the parameter list and current values)
//
//  QUICK RECIPES
//    fast preview of any view (well under a minute on 2 cores):
//        ./shpbr w=360 h=360 aa=1 refl=0 maxit=24 out=preview.png
//    find empty places to put the camera inside the globe:
//        ./shpbr pockets=1
//    move the camera and look somewhere else (world coordinates, see "GEOMETRY" below):
//        ./shpbr cx=1.01 cy=-0.40 cz=0.28  lx=-1.3 ly=0.1 lz=0   fov=100
//    change the physics (mass ratio, mixing law, bulb power):
//        ./shpbr m1=0.03 m2=0.06 wa=1 wb=3 pn=3 ...   (then use pockets=1 again: the globe moves)
//
//  OUTPUT     A .png (uncompressed, written by the small encoder at the end of this file)
//             when the file name ends in .png, otherwise a binary .ppm.
//
//  HOW THE FILE IS ORGANISED
//    SECTION A  run-time parameters  (command line, no recompilation)
//    SECTION B  look constants       (edit and recompile)
//    SECTION C  vector maths
//    SECTION D  the space-time field: mixing weight and time dilation
//    SECTION E  one iteration ("tick") of the hybrid fractal
//    SECTION F  distance estimate (how far a point is from the fractal surface)
//    SECTION G  sky
//    SECTION H  ray marching with gravitational light bending
//    SECTION I  physically based shading
//    SECTION J  pocket finder, PNG writer
//    SECTION K  main: parameters, camera, render loop
//
//  GEOMETRY (world coordinates, y is up)
//    The barycentre of the two masses is the origin. With the default circular orbit of
//    separation 2.6 and equal masses, mass 1 (Mandelbulb) is at (-1.3, 0, 0) and mass 2
//    (Mandelbox, the "globe") is at (+1.3, 0, 0). The Mandelbulb is about 1.2 in radius.
//    The globe has a core of radius about 0.3 and broken shells out to about 0.8.
//    The program prints both centres when it starts.
//
// =====================================================================================

#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <vector>
#include <algorithm>
#include <omp.h>

struct V { double x, y, z; };          // a point, a direction or an RGB colour

// =====================================================================================
//  SECTION A — RUN-TIME PARAMETERS
//  Every variable here can be set on the command line as  name=value  (the name is given
//  in [brackets]). Defaults reproduce the reference image.
// =====================================================================================

// ---- A1. Image -----------------------------------------------------------------------
static int    IMG_W = 1440;    // [w]   image width in pixels
static int    IMG_H = 1440;    // [h]   image height in pixels. Cost grows with w*h.
static int    AA    = 2;       // [aa]  supersampling per axis: 1 = one ray per pixel (fast, jagged),
                               //       2 = 2x2 = 4 rays per pixel (reference), 3 = 9 rays per pixel
                               //       (2.25x the cost of aa=2). Cost grows with aa*aa.
static const char *OUT_FILE = "inside_the_globe.png";   // [out]  .png or .ppm

// ---- A2. Camera ----------------------------------------------------------------------
static V      CAM  = { 1.66, -0.36, 0.20 };  // [cx cy cz]  camera position. Must be in empty space:
                               //       the program prints "camera clearance"; below ~0.001 the camera
                               //       is inside or touching a surface and the image is one flat colour.
                               //       Use pockets=1 to list good positions.
static V      LOOK = { 0.30,  0.30, 0.50 };  // [lx ly lz]  the point the camera looks at (centre of image)
static double FOV  = 100;      // [fov] vertical field of view in degrees. 100 is a wide, immersive lens;
                               //       60 is "normal"; 20-30 is a telephoto close-up. Wide angles stretch
                               //       objects near the image corners.

// ---- A3. The two masses and their orbit (this decides where the two fractals sit) ----
static double M1 = 0.04;       // [m1]  mass under the Mandelbulb, in units where G = c = 1.
static double M2 = 0.04;       // [m2]  mass under the Mandelbox (the globe).
                               //       The RATIO m2/m1 controls how much of the box survives the bulb's
                               //       pull (with the default law: ratio 1 = thin shells around a core,
                               //       ratio 4 = a merged body with a neck, ratio >= 8 = solid box).
                               //       The TOTAL controls relativistic strength: light bending, time
                               //       dilation, and the size of each black-hole shadow (radius about
                               //       5 x mass). 0.01-0.05 is gentle, 0.15 is extreme.
static double ORBIT_A     = 2.6;  // [a]     semi-major axis = separation of the centres for e = 0.
                                  //         Smaller = the fractals interpenetrate; larger = they separate.
static double ORBIT_E     = 0.0;  // [e]     eccentricity 0..0.9. With e > 0 the separation depends on phase.
static double ORBIT_PHASE = 0.0;  // [phase] mean anomaly in degrees: where on the orbit the pair is.
                                  //         0 = closest approach (separation a(1-e)), 180 = farthest.
static double ORBIT_INCL  = 0.0;  // [incl]  tilt of the orbital plane in degrees (0 = the x-z plane).

// ---- A4. The mixing law (this replaces the usual constant blend "tau") ---------------
//   weight of fractal i at a point  ~  m_i^wa / (distance to mass i)^wb
static double W_MASS_POW = 2;  // [wa]  exponent of the mass
static double W_DIST_POW = 6;  // [wb]  exponent of the distance. Larger = each fractal owns the space
                               //       around its own mass more sharply.
                               //         wa=1 wb=1  share of gravitational potential (box is destroyed)
                               //         wa=1 wb=2  share of field strength          (box is destroyed)
                               //         wa=1 wb=3  share of tidal field             (box survives if pn is low)
                               //         wa=2 wb=6  share of curvature (default; shells and core)
                               //         wb=0       constant blend, the classical "tau" hybrid
static double LAPSE = 1;       // [lapse] strength of gravitational time dilation on the iteration, 0..1.
                               //       1 = orbits slow down near each mass and freeze at the horizon;
                               //       0 = off. Changes the shapes close to both centres.
static double LENS  = 1;       // [lens]  strength of light bending for camera rays, 0..1 (values above 1
                               //       exaggerate it). 0 = straight rays, no black-hole shadow distortion.

// ---- A5. The two fractals ------------------------------------------------------------
static double BULB_POWER = 8;    // [pn]  Mandelbulb power. 8 = the classic bulb. 2-4 = smoother, blobbier,
                                 //       and far gentler on the box (the box survives much better).
static double BULB_SIZE  = 1.0;  // [sa]  Mandelbulb scale factor (radius is about 1.2 x this).
static double BOX_SCALE  = 2.0;  // [bs]  Mandelbox scale. 2 = standard. Negative values (-1.5) give a
                                 //       very different, more organic box. Between -1 and 1 there is
                                 //       no fractal (nothing escapes).
static double BOX_SIZE   = 0.22; // [sb]  Mandelbox scale factor in world units. Its bounding half-width
                                 //       is 6 x this for bs = 2; most of its volume is within 2.9 x this.
static double BOX_MINR2  = 0.25; // [minr2] Mandelbox inner fold radius squared (standard 0.25).
                                 //       Smaller = stronger sphere inversion, spikier detail.
static int    MAX_ITER   = 40;   // [maxit] iteration budget per point. The picture hardly changes
                                 //       between 12 and 80; lower is faster. 24 is fine for previews.
static double BAILOUT    = 1e9;  // [bail] a point has "escaped" when its orbit is farther than this
                                 //       from the origin. Keep it huge: the distance formula in SECTION F
                                 //       is only valid far away, and small values (tens) gave terraced
                                 //       surfaces in tests.

// ---- A6. Surface definition and ray-marching quality ---------------------------------
static double SURF_EPS = 4.3e-4; // [epsw] thickness of the rendered surface in world units: a ray "hits"
                                 //       where the estimated distance to the fractal drops below this.
                                 //       This is an artistic control as much as a quality one, because the
                                 //       globe's shells are thin sets: LARGER (2e-3) fattens them into
                                 //       solid blobs, SMALLER (1e-4) thins them to rims and threads and
                                 //       costs more time. Far from the camera the threshold is widened to
                                 //       half a sub-pixel automatically (PIXEL_EPS in SECTION B).
static double STEP_SAFETY = 0.45; // [fud] each march step is this fraction of the estimated distance.
                                 //       The estimate is not a guaranteed bound, so 1.0 overshoots and
                                 //       punches holes; 0.3 is safer and slower; 0.45-0.5 is a good balance.
static int    MAX_STEPS  = 1400; // [steps] maximum steps per camera ray. Rays that run out are drawn as
                                 //       sky; if thin structures look eaten away, raise this.

// ---- A7. Lighting (radiometric: values multiply, then tone mapping compresses) -------
static V      SUN_DIR  = { 0.70, 0.35, -0.45 }; // [sx sy sz] direction TOWARDS the sun (any length).
                                 //       Put it roughly behind the camera to light what you see; put it
                                 //       in front for silhouettes and rim light.
static double SUN_POWER  = 7.0;  // [suni]  sun irradiance. 0 = no sun.
static V      LAMP_POS = { 1.55, -0.10, 0.50 }; // [px py pz] a small warm lamp inside the scene. Place it in
                                 //       empty space near what you want lit. [headlamp=1] ignores this and
                                 //       puts the lamp at the camera (flat but always-lit previews).
static bool   LAMP_SET = true;
static double LAMP_POWER = 0.5;  // [lampi] lamp intensity; its light falls off with distance squared, so
                                 //       0.5 lights things within about 0.5 units. 0 = no lamp.
static double AMBIENT    = 1.0;  // [ambi]  multiplier on the dim blue sky light that fills shadows.
static double EXPOSURE   = 1.0;  // [expo]  overall brightness before tone mapping (like camera exposure).
static int    REFLECT    = 1;    // [refl]  1 = trace one mirror bounce on glossy/metallic surfaces,
                                 //       0 = skip it (faster; surfaces look flatter).

static int    POCKETS = 0;       // [pockets] 1 = do not render; list empty spots near the globe instead.
static int    HELP    = 0;       // [help]    1 = print parameters and exit.

// =====================================================================================
//  SECTION B — LOOK CONSTANTS  (edit here and recompile)
//  These were fixed numbers in the reference render. They are constants on purpose, so
//  the compiler treats them exactly as before; change them freely for new looks.
// =====================================================================================

// ---- B1. Light colours (linear RGB, not gamma-corrected) ----
static constexpr V SUN_COLOR  = { 1.0, 0.96, 0.90 };   // slightly warm white
static constexpr V LAMP_COLOR = { 1.0, 0.70, 0.42 };   // tungsten orange; try {0.5,0.7,1.0} for a cold lamp
static constexpr V SKY_AMBIENT = { 0.055, 0.075, 0.12 }; // colour AND strength of the fill light in shadows

// ---- B2. Shadows ----
static constexpr double SUN_SHADOW_RANGE = 5.0;   // how far a sun shadow ray travels before giving up
static constexpr double SUN_SHADOW_SHARP = 28.0;  // penumbra: larger = harder shadow edges (sun = small source)
static constexpr double LAMP_SHADOW_SHARP = 12.0; // lamp is a bigger source: softer edges
static constexpr int    SHADOW_STEPS = 64;        // more = shadows resolved through finer gaps, slower

// ---- B3. Ambient occlusion (darkening of creases; 5 probes along the surface normal) ----
static constexpr double AO_START = 0.008, AO_STEP = 0.035; // probe distances: START + STEP*i, i = 1..5.
                                                           // Scale both with the size of the detail you view.
static constexpr double AO_FALLOFF = 0.72;        // weight of each further probe
static constexpr double AO_STRENGTH = 4.0;        // larger = darker creases
static constexpr double AO_FLOOR = 0.03;          // never darker than this fraction

// ---- B4. Material: how the fractal's own data becomes a surface ----
// Each surface point remembers three numbers from its orbit (SECTION F):
//   wavg  = average weight of the Mandelbulb along the orbit: 1 = bulb-governed, 0 = box-governed
//   t1,t2 = closest approach of the orbit to mass 1 / mass 2 (in each fractal's own units)
// Base colour comes from two cosine palettes  colour(t) = A + B*cos(2*pi*(C*t + D))  driven by t1, t2.
static constexpr double BULB_PAL_FREQ = 0.55, BULB_PAL_PHASE = 0.10;      // how fast bulb colour cycles with t1
static constexpr V BULB_PAL_A = { 0.62, 0.42, 0.28 }, BULB_PAL_B = { 0.38, 0.32, 0.25 }, BULB_PAL_D = { 0.00, 0.10, 0.22 };
static constexpr double BOX_PAL_FREQ = 0.22, BOX_PAL_PHASE = 0.05;        // same for the box with t2
static constexpr V BOX_PAL_A = { 0.28, 0.48, 0.60 }, BOX_PAL_B = { 0.25, 0.32, 0.36 }, BOX_PAL_D = { 0.55, 0.30, 0.18 };
//   A = average colour, B = swing around it, D = per-channel phase (shifts the hue order).
//   Warm A/B for the bulb and cool for the box is what makes ownership readable.
static constexpr V WEAVE_COLOR = { 0.86, 0.50, 0.95 };  // tint where both fractals share the orbit equally
static constexpr double WEAVE_TINT = 0.55;              // strength of that tint
static constexpr double ALBEDO_MIN = 0.03, ALBEDO_MAX = 0.9;  // physical reflectance range
static constexpr double METAL_MAX = 0.85;     // shared ("woven") regions become metallic up to this amount;
                                              // 0 = everything is glazed ceramic, no metal
static constexpr double ROUGH_BASE = 0.42;    // roughness of bulb-governed surface (0.1 mirror-like .. 0.7 chalky)
static constexpr double ROUGH_BOX_DROP = 0.20;// box-governed surface is this much glossier
static constexpr double ROUGH_VAR = 0.12, ROUGH_VAR_FREQ = 9; // banded variation of gloss along t2
static constexpr double ROUGH_MIN = 0.14, ROUGH_MAX = 0.6;

// ---- B5. Mirror bounce ----
static constexpr double REFL_MIN_WEIGHT = 0.02; // skip the bounce when it would contribute less than this
static constexpr double REFL_RANGE = 3.5;       // how far a reflected ray looks
static constexpr int    REFL_STEPS = 180;
static constexpr double REFL_EPS_MUL = 3.0;     // reflected rays use a 3x coarser surface (cheaper, less noise)

// ---- B6. Sky ----
static constexpr double STAR_BRIGHTNESS = 1.0;  // 0 = black sky; 2 = dense bright star field
static constexpr V SUN_DISC_COLOR = { 1.0, 0.95, 0.85 };
static constexpr double SUN_DISC_PEAK = 40, SUN_DISC_SHARP = 3000; // brightness and tightness of the sun's disc
static constexpr double SUN_HALO = 0.25, SUN_HALO_SHARP = 80;      // soft glow around it

// ---- B7. Marching details ----
static constexpr double STEP_CAP = 0.22;   // longest single step. Light bending is integrated step by step,
                                           // so shorter steps bend rays more accurately (in a one-mass test
                                           // the total deflection changed by under 0.1% between 0.01 and 0.22).
static constexpr double PIXEL_EPS = 0.5;   // far away, the surface threshold grows to this many sub-pixels
static constexpr double PRIMARY_RANGE = 12.0; // camera rays stop after this path length
static constexpr double FAR_RADIUS = 4.2;  // ...or when they leave this sphere around the origin heading out.
                                           // Must enclose both fractals (raise it if you raise the separation).
static constexpr double U_CAP = 0.9;       // numerical guard for bending very close to a horizon

// ---- B8. Final image ----
static constexpr double GAMMA = 2.2;       // display gamma
static constexpr double VIGNETTE = 0.35;   // darkening toward the corners (0 = none)
// Gravitational redshift cue: surface colour is multiplied by (alpha^R, alpha^G, alpha^B) where alpha < 1
// is the local slowing of time. Larger blue exponent = deeper red near the masses. {0,0,0} switches it off.
static constexpr double REDSHIFT_R = 0.6, REDSHIFT_G = 1.5, REDSHIFT_B = 2.4;

// ---- B9. Mandelbox outer fold radius squared (standard 1). Kept constant; change with care. ----
static constexpr double BOX_FIXR2 = 1.0;
// ---- B10. Step used to differentiate the orbit numerically (SECTION F). 1e-7 is near optimal in doubles.
static constexpr double DIFF_STEP = 1e-7;

// =====================================================================================
//  SECTION C — VECTOR MATHS
// =====================================================================================
static inline V v(double x, double y, double z) { V r = {x, y, z}; return r; }
static inline V add(V a, V b) { return v(a.x + b.x, a.y + b.y, a.z + b.z); }
static inline V sub(V a, V b) { return v(a.x - b.x, a.y - b.y, a.z - b.z); }
static inline V mul(V a, double s) { return v(a.x * s, a.y * s, a.z * s); }
static inline V vmul(V a, V b) { return v(a.x * b.x, a.y * b.y, a.z * b.z); }          // per-channel product
static inline double dot(V a, V b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
static inline double len(V a) { return sqrt(dot(a, a)); }
static inline V nrm(V a) { double l = len(a); return l > 0 ? mul(a, 1 / l) : a; }
static inline V cross(V a, V b) { return v(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x); }
static inline V mix(V a, V b, double t) { return add(mul(a, 1 - t), mul(b, t)); }       // blend a -> b
static inline double clampd(double x, double a, double b) { return x < a ? a : (x > b ? b : x); }
static inline double sstep(double a, double b, double x) { double t = clampd((x - a) / (b - a), 0, 1); return t * t * (3 - 2 * t); }

// Quantities derived from the parameters once, in main().
static V P1, P2, BARY;          // centre of the Mandelbulb, centre of the Mandelbox, barycentre (origin)
static double S1M, S2M;         // m1^wa and m2^wa
static double SUBPIX = 1e-3;    // angle covered by one sub-pixel (radians per unit image-plane distance)
static V SUN;                   // unit vector towards the sun

// =====================================================================================
//  SECTION D — THE SPACE-TIME FIELD
//  For a point z this returns the two things the fractal reads from the geometry of the
//  two masses (the Brill-Lindquist two-mass metric):
//    w1  in [0,1]: the Mandelbulb's share of the local field (the box gets 1 - w1).
//                  This is the position-dependent replacement for a constant blend factor.
//    al  in [0,1]: the lapse = rate at which time passes there. 1 far away, 0 on a horizon.
//  U = m1/(2 r1) + m2/(2 r2) is the gravitational potential term; the horizon is U = 1.
// =====================================================================================
static inline void field(V z, double *w1, double *al) {
    double r1 = len(sub(z, P1)) + 1e-9, r2 = len(sub(z, P2)) + 1e-9;   // distances to the masses (+tiny guard)
    double s1 = S1M / pow(r1, W_DIST_POW), s2 = S2M / pow(r2, W_DIST_POW);
    *w1 = s1 / (s1 + s2);
    double U = M1 / (2 * r1) + M2 / (2 * r2);
    double a = (1 - U) / (1 + U);          // exact for one mass (Schwarzschild), an approximation for two
    if (a < 0) a = 0;                      // inside a horizon: time stands still
    *al = 1 - LAPSE * (1 - a);             // LAPSE = 0 switches time dilation off
}

// =====================================================================================
//  SECTION E — ONE TICK OF THE HYBRID FRACTAL
//  z is the moving orbit point, x the sample point the orbit started from.
//    Mandelbulb step about mass 1:  a = (z - P1)^n + (x - P1)      (triplex power, pole = y axis)
//    Mandelbox  step about mass 2:  b = scale * ballfold(boxfold(z - P2)) + (x - P2)
//    blend with the local weight:   zn = w1 * a + (1 - w1) * b
//    advance by local proper time:  z <- z + al * (zn - z)
//  Outputs w1o, alo (the field at z) and ra, rb_ (distance of z from each centre in that
//  fractal's own units) are used for colouring.
// =====================================================================================
static inline V tick(V z, V x, double *w1o, double *alo, double *ra, double *rb_) {
    double w1, al; field(z, &w1, &al);
    // --- Mandelbulb: go to spherical coordinates, multiply both angles by n, raise the radius to n
    V q = mul(sub(z, P1), 1 / BULB_SIZE), cA = mul(sub(x, P1), 1 / BULB_SIZE); double r = len(q); V a;
    if (r < 1e-12) a = cA;
    else {
        double th = acos(clampd(q.y / r, -1, 1)) * BULB_POWER, ph = atan2(q.z, q.x) * BULB_POWER, rn = pow(r, BULB_POWER), st = sin(th);
        a = v(rn * st * cos(ph) + cA.x, rn * cos(th) + cA.y, rn * st * sin(ph) + cA.z);
    }
    a = add(P1, mul(a, BULB_SIZE));
    // --- Mandelbox: fold space back into the cube [-1,1]^3, then invert through a sphere, then scale
    V b = mul(sub(z, P2), 1 / BOX_SIZE), cB = mul(sub(x, P2), 1 / BOX_SIZE); double rb = len(b);
    b.x = clampd(b.x, -1, 1) * 2 - b.x; b.y = clampd(b.y, -1, 1) * 2 - b.y; b.z = clampd(b.z, -1, 1) * 2 - b.z;
    double r2 = dot(b, b), f = 1;
    if (r2 < BOX_MINR2) f = BOX_FIXR2 / BOX_MINR2; else if (r2 < BOX_FIXR2) f = BOX_FIXR2 / r2;
    b = add(P2, mul(add(mul(b, BOX_SCALE * f), cB), BOX_SIZE));
    // --- space-time mix, advanced by local proper time
    V zn = add(mul(a, w1), mul(b, 1 - w1));
    *w1o = w1; *alo = al; *ra = r; *rb_ = rb;
    return add(z, mul(sub(zn, z), al));
}

// =====================================================================================
//  SECTION F — DISTANCE ESTIMATE
//  Iterates the orbit of x until it escapes (|z| > BAILOUT) and estimates how far x is
//  from the fractal:   de = 0.5 * R * log(R) / |grad R| ,   R = final |z|.
//  The gradient is taken numerically: three neighbours x + DIFF_STEP along each axis are
//  iterated side by side and their final radii compared. Points that never escape
//  within MAX_ITER ticks are "inside": de = 0.
//  Also returns the colouring data described in B4.
// =====================================================================================
struct R { double de, wavg, t1, t2, w0, a0; int it, esc; };
//   de   distance estimate (0 = inside)          wavg  orbit average of the bulb weight
//   t1   closest approach to mass 1 (bulb units)  t2    closest approach to mass 2 (box units)
//   w0   bulb weight at x itself                  a0    lapse (time rate) at x itself
//   it   ticks used                               esc   1 if the orbit escaped

static R map(V x) {
    R o; V xs[4] = {x, v(x.x + DIFF_STEP, x.y, x.z), v(x.x, x.y + DIFF_STEP, x.z), v(x.x, x.y, x.z + DIFF_STEP)};
    V z[4] = {xs[0], xs[1], xs[2], xs[3]};
    double ws = 0, t1 = 1e9, t2 = 1e9, w1 = 0.5, al = 1, Rr = 0, ra, rb, d0, d1, d2, d3; int i, esc = 0;
    o.w0 = 0.5; o.a0 = 1;
    for (i = 0; i < MAX_ITER; i++) {
        z[0] = tick(z[0], xs[0], &w1, &al, &ra, &rb);
        if (i == 0) { o.w0 = w1; o.a0 = al; }
        ws += w1; if (ra < t1) t1 = ra; if (rb < t2) t2 = rb;
        z[1] = tick(z[1], xs[1], &d0, &d1, &d2, &d3);
        z[2] = tick(z[2], xs[2], &d0, &d1, &d2, &d3);
        z[3] = tick(z[3], xs[3], &d0, &d1, &d2, &d3);
        Rr = len(sub(z[0], BARY));
        if (Rr > BAILOUT) { esc = 1; i++; break; }
    }
    o.it = i; o.esc = esc; o.wavg = ws / (i > 0 ? i : 1); o.t1 = t1; o.t2 = t2;
    if (!esc) { o.de = 0; return o; }
    // gradient of the escape radius (numerical, so it includes how the weight and lapse vary in space)
    double g1 = len(sub(z[1], BARY)) - Rr, g2 = len(sub(z[2], BARY)) - Rr, g3 = len(sub(z[3], BARY)) - Rr;
    double dr = sqrt(g1 * g1 + g2 * g2 + g3 * g3) / DIFF_STEP;
    if (!(dr > 1e-12)) dr = 1e-12;
    o.de = 0.5 * Rr * log(Rr) / dr;
    return o;
}

// =====================================================================================
//  SECTION G — SKY
//  A procedural star field looked up by ray direction (two layers of stars placed by a
//  hash on the faces of a cube, plus a faint band), and the sun's disc and halo.
//  Because camera rays are bent by the masses, stars near the globe smear into arcs.
// =====================================================================================
static V pal(double t, V a, V b, V c, V d) {      // cosine palette  A + B*cos(2*pi*(C*t + D))
    return v(a.x + b.x * cos(6.2831853 * (c.x * t + d.x)), a.y + b.y * cos(6.2831853 * (c.y * t + d.y)), a.z + b.z * cos(6.2831853 * (c.z * t + d.z)));
}
static inline unsigned hsh(unsigned a, unsigned b, unsigned c) {
    unsigned h = a * 374761393u + b * 668265263u + c * 2147483647u; h = (h ^ (h >> 13)) * 1274126177u; return h ^ (h >> 16);
}
static V stars(V d) {
    V col = v(0, 0, 0);
    double ax = fabs(d.x), ay = fabs(d.y), az = fabs(d.z); int face; double u, w;
    if (ax >= ay && ax >= az) { face = d.x > 0 ? 0 : 1; u = d.y / ax; w = d.z / ax; }
    else if (ay >= az) { face = d.y > 0 ? 2 : 3; u = d.x / ay; w = d.z / ay; }
    else { face = d.z > 0 ? 4 : 5; u = d.x / az; w = d.y / az; }
    for (int layer = 0; layer < 2; layer++) {
        double N = layer ? 90 : 34;                       // cells per cube face: more = more, smaller stars
        double gu = (u * 0.5 + 0.5) * N, gw = (w * 0.5 + 0.5) * N;
        int iu = (int)floor(gu), iw = (int)floor(gw);
        for (int du = -1; du <= 1; du++) for (int dw = -1; dw <= 1; dw++) {
            unsigned h = hsh(iu + du + 1000, iw + dw + 1000, face * 7 + layer * 31);
            if ((h & 255) > (unsigned)(layer ? 70 : 46)) continue;   // fraction of cells that hold a star
            double sx = iu + du + ((h >> 8) & 1023) / 1023.0, sy = iw + dw + ((h >> 18) & 1023) / 1023.0;
            double dd = (gu - sx) * (gu - sx) + (gw - sy) * (gw - sy);
            double sz = layer ? 0.012 : 0.02, br = (layer ? 0.5 : 1.3) * (0.3 + ((h >> 28) & 15) / 15.0);  // size, brightness
            double I = br * exp(-dd / sz);
            double tint = ((h >> 4) & 15) / 15.0;                     // warm .. cool star colour
            col = add(col, mul(mix(v(1.0, 0.82, 0.62), v(0.65, 0.8, 1.0), tint), I));
        }
    }
    double band = exp(-pow((d.y * 0.8 + d.x * 0.35 - d.z * 0.2) * 2.6, 2));   // faint "milky way" band
    col = add(col, mul(v(0.022, 0.026, 0.05), 0.5 + band));                   // background sky colour
    return col;
}
static V sky(V d) {
    V c = mul(stars(d), STAR_BRIGHTNESS);
    double s = dot(d, SUN); if (s < 0) s = 0;
    c = add(c, mul(SUN_DISC_COLOR, SUN_DISC_PEAK * pow(s, SUN_DISC_SHARP) + SUN_HALO * pow(s, SUN_HALO_SHARP)));
    return c;
}
// Filmic tone curve (ACES fit): maps unbounded scene brightness into 0..1 with soft highlights.
static double aces(double x) { x *= 0.9; return clampd((x * (2.51 * x + 0.03)) / (x * (2.43 * x + 0.59) + 0.14), 0, 1); }

// =====================================================================================
//  SECTION H — RAY MARCHING WITH LIGHT BENDING
//  Walk along the ray; at each position ask SECTION F how far the surface is and step a
//  safe fraction of that. In the metric of the two masses, light behaves as in glass of
//  refractive index n = (1+U)^3/(1-U), so at every step the direction is turned toward
//  increasing n (toward the masses). A ray that reaches a horizon (r < m/2) is absorbed:
//  that is the black shadow at the heart of each mass.
//    bend    1 = bend the ray (camera rays), 0 = straight (cheap secondary rays)
//    epsmul  multiplier on the surface threshold
// =====================================================================================
struct Hit { int hit, absorbed, steps; V p, rd; double t; R r; };
//   hit = reached a surface; absorbed = fell into a horizon; p = end point; rd = final direction
//   (differs from the starting direction when bent); t = path length; r = surface data from SECTION F

static Hit march(V ro, V rd, double tmax, int maxsteps, int bend, double epsmul) {
    Hit h; memset(&h, 0, sizeof h); double t = 0; V p = ro; int s;
    for (s = 0; s < maxsteps; s++) {
        double d1 = len(sub(p, P1)), d2 = len(sub(p, P2));
        if (d1 < M1 * 0.5 || d2 < M2 * 0.5) { h.absorbed = 1; break; }        // crossed a horizon
        R r = map(p);
        double eps = fmax(SURF_EPS, PIXEL_EPS * SUBPIX * t) * epsmul;         // surface thickness here
        if (r.de < eps) { h.hit = 1; h.r = r; break; }
        double st = r.de * STEP_SAFETY; if (st > STEP_CAP) st = STEP_CAP;
        if (bend && LENS > 0) {
            V e1 = sub(p, P1), e2 = sub(p, P2);
            double r1 = d1 + 1e-9, r2 = d2 + 1e-9;
            double U = fmin(M1 / (2 * r1) + M2 / (2 * r2), U_CAP);
            V gU = add(mul(e1, -M1 / (2 * r1 * r1 * r1)), mul(e2, -M2 / (2 * r2 * r2 * r2)));   // gradient of U
            V gn = mul(gU, 3 / (1 + U) + 1 / (1 - U));                                          // gradient of n, over n
            V perp = sub(gn, mul(rd, dot(rd, gn)));                                             // part across the ray
            rd = nrm(add(rd, mul(perp, st * LENS)));
        }
        p = add(p, mul(rd, st)); t += st;
        if (t > tmax) break;
        V pb = sub(p, BARY); if (dot(pb, pb) > FAR_RADIUS * FAR_RADIUS && dot(pb, rd) > 0) break;  // left the scene
    }
    h.p = p; h.rd = rd; h.t = t; h.steps = s; return h;
}

// Soft shadow: march from the surface toward the light; the closest near-miss along the
// way, relative to the distance travelled, sets how deep in the penumbra the point is.
// Returns 0 (fully shadowed) .. 1 (fully lit). k = sharpness.
static double shadow(V ro, V ld, double maxt, double k) {
    double res = 1, t = 0.003;
    for (int i = 0; i < SHADOW_STEPS && t < maxt; i++) {
        double d = map(add(ro, mul(ld, t))).de;
        if (d < SURF_EPS * 0.4) return 0;
        res = fmin(res, k * d / t); t += clampd(d * 0.6, 0.003, 0.25);
    }
    return clampd(res, 0, 1);
}

// =====================================================================================
//  SECTION I — PHYSICALLY BASED SHADING
//  Standard metallic/roughness microfacet model (Cook-Torrance with GGX):
//    reflected light = (diffuse + specular) * incoming light * cos(angle to light)
//    diffuse  = albedo/pi, removed for metals and where light is reflected specularly
//    specular = D*G*F / (4 cos_v cos_l):  D = GGX distribution of microfacets (roughness),
//               G = Smith shadowing between microfacets, F = Schlick Fresnel (more mirror-
//               like at grazing angles; F0 = 4% for non-metals, = albedo for metals)
//  Light sources: the sun (parallel rays, soft shadow), the lamp (inverse-square falloff,
//  soft shadow), a dim sky from all directions scaled by ambient occlusion, and one
//  mirror-bounce ray for glossy reflections. It is not a path tracer: light that bounces
//  diffusely between surfaces is not simulated.
//    vdir  = unit vector from the surface back toward the viewer
//    depth = 0 for what the camera sees, 1 inside a reflection (no further bounce)
// =====================================================================================
static V shade_pbr(Hit h, V vdir, int depth) {
    V p = h.p; double eps = fmax(SURF_EPS, PIXEL_EPS * SUBPIX * h.t), hh = eps * 0.9;
    // ---- surface normal: gradient of the distance estimate from 4 probes on a tetrahedron
    V k1 = v(1, -1, -1), k2 = v(-1, -1, 1), k3 = v(-1, 1, -1), k4 = v(1, 1, 1);
    V n = add(add(mul(k1, map(add(p, mul(k1, hh))).de), mul(k2, map(add(p, mul(k2, hh))).de)),
              add(mul(k3, map(add(p, mul(k3, hh))).de), mul(k4, map(add(p, mul(k4, hh))).de)));
    n = nrm(n);
    if (dot(n, vdir) < 0) n = nrm(add(n, mul(vdir, -1.1 * dot(n, vdir))));   // never face away from the viewer
    V pp = add(p, mul(n, eps * 2.5));                                        // lift off the surface for secondary rays
    // ---- material from the orbit data (see B4)
    R r = h.r;
    V colA = pal(BULB_PAL_FREQ * r.t1 + BULB_PAL_PHASE, BULB_PAL_A, BULB_PAL_B, v(1, 1, 1), BULB_PAL_D);
    V colB = pal(BOX_PAL_FREQ * r.t2 + BOX_PAL_PHASE, BOX_PAL_A, BOX_PAL_B, v(1, 1, 1), BOX_PAL_D);
    double w = sstep(0.12, 0.88, r.wavg), weave = 4 * r.wavg * (1 - r.wavg);   // w: 0 box .. 1 bulb; weave: 1 where shared
    V alb = mix(colB, colA, w); alb = mix(alb, WEAVE_COLOR, WEAVE_TINT * weave * weave);
    alb = v(clampd(alb.x, ALBEDO_MIN, ALBEDO_MAX), clampd(alb.y, ALBEDO_MIN, ALBEDO_MAX), clampd(alb.z, ALBEDO_MIN, ALBEDO_MAX));
    alb = v(alb.x * alb.x * 0.6 + alb.x * 0.4, alb.y * alb.y * 0.6 + alb.y * 0.4, alb.z * alb.z * 0.6 + alb.z * 0.4); // deepen: display colour -> reflectance
    double metal = clampd(METAL_MAX * weave * weave, 0, METAL_MAX);
    double rough = clampd(ROUGH_BASE - ROUGH_BOX_DROP * (1 - w) + ROUGH_VAR * sin(ROUGH_VAR_FREQ * r.t2), ROUGH_MIN, ROUGH_MAX);
    V F0 = mix(v(0.04, 0.04, 0.04), alb, metal);                               // mirror reflectance at normal incidence
    double NV = clampd(dot(n, vdir), 1e-3, 1), a = rough * rough, a2 = a * a, kg = (rough + 1) * (rough + 1) / 8;
    // ---- ambient occlusion: is there geometry close above the surface?
    double ao = 0, sc = 1;
    for (int i = 1; i <= 5; i++) { double hr = AO_START + AO_STEP * i; double d = map(add(pp, mul(n, hr))).de; ao += (hr - fmin(d, hr)) * sc; sc *= AO_FALLOFF; }
    ao = clampd(1 - AO_STRENGTH * ao, AO_FLOOR, 1);
    V out = v(0, 0, 0);
    // ---- direct light: li = 0 sun, li = 1 lamp
    for (int li = 0; li < 2; li++) {
        V L; double dist = 1e9; V Li;                                          // direction to light, distance, incoming light
        if (li == 0) { L = SUN; Li = mul(SUN_COLOR, SUN_POWER); }
        else { V dl = sub(LAMP_POS, pp); dist = len(dl); L = mul(dl, 1 / dist); Li = mul(LAMP_COLOR, LAMP_POWER / (dist * dist + 1e-4)); }
        double NL = dot(n, L); if (NL <= 0) continue;                          // light is behind the surface
        double sh = shadow(pp, L, li == 0 ? SUN_SHADOW_RANGE : dist, li == 0 ? SUN_SHADOW_SHARP : LAMP_SHADOW_SHARP); if (sh <= 0) continue;
        V H = nrm(add(L, vdir)); double NH = clampd(dot(n, H), 0, 1), VH = clampd(dot(vdir, H), 0, 1);
        double dn = NH * NH * (a2 - 1) + 1, D = a2 / (M_PI * dn * dn);                       // GGX
        double G = (NV / (NV * (1 - kg) + kg)) * (NL / (NL * (1 - kg) + kg));                // Smith
        double f5 = pow(1 - VH, 5); V F = add(F0, mul(sub(v(1, 1, 1), F0), f5));             // Schlick
        V spec = mul(F, D * G / (4 * NV * NL + 1e-4));
        V kd = mul(sub(v(1, 1, 1), F), 1 - metal);
        V brdf = add(vmul(kd, mul(alb, 1 / M_PI)), spec);
        out = add(out, mul(vmul(brdf, Li), NL * sh));
    }
    // ---- sky light on the diffuse part, and one mirror bounce for the specular part
    double f5v = pow(1 - NV, 5); V Fv = add(F0, mul(sub(v(fmax(1 - rough, F0.x), fmax(1 - rough, F0.y), fmax(1 - rough, F0.z)), F0), f5v));
    V amb = mul(SKY_AMBIENT, AMBIENT);
    out = add(out, mul(vmul(vmul(sub(v(1, 1, 1), Fv), mul(alb, 1 - metal)), amb), ao));
    V rdir = sub(mul(n, 2 * dot(n, vdir)), vdir);                              // mirror direction
    double gloss = (1 - rough) * (1 - rough), wr = fmax(Fv.x, fmax(Fv.y, Fv.z)) * gloss;
    V env;
    if (REFLECT && depth == 0 && wr > REFL_MIN_WEIGHT) {
        Hit h2 = march(pp, rdir, REFL_RANGE, REFL_STEPS, 0, REFL_EPS_MUL);
        env = h2.hit ? shade_pbr(h2, mul(rdir, -1), 1) : (h2.absorbed ? v(0, 0, 0) : sky(h2.rd));
        out = add(out, mul(vmul(Fv, env), gloss));
        out = add(out, mul(vmul(Fv, amb), ao * (1 - gloss)));
    } else out = add(out, mul(vmul(Fv, amb), ao));
    // ---- gravitational redshift cue (B8): light from deep in a gravity well arrives redder and dimmer
    double al = clampd(r.a0, 0, 1);
    return vmul(out, v(pow(al, REDSHIFT_R), pow(al, REDSHIFT_G), pow(al, REDSHIFT_B)));
}

// =====================================================================================
//  SECTION J — UTILITIES
// =====================================================================================
static double rnd01(unsigned *s) { *s = *s * 1664525u + 1013904223u; return (*s >> 8) / 16777216.0; }

// Pocket finder: samples 60000 random points between 0.2 and 0.8 from the globe's centre
// and prints the ones with the most free space around them. "de" is the estimated
// clearance (an estimate, not a guarantee), "r" the distance from the globe's centre.
// Pick one as cx cy cz, choose a look-at point, and do a fast preview.
static void find_pockets() {
    struct Pk { V p; double de; }; std::vector<Pk> all; unsigned s = 7;
    for (int i = 0; i < 60000; i++) {
        V u = v(rnd01(&s) * 2 - 1, rnd01(&s) * 2 - 1, rnd01(&s) * 2 - 1); double l = len(u); if (l > 1 || l < 0.25) continue;
        V p = add(P2, mul(u, 0.8)); all.push_back({p, map(p).de});
    }
    std::sort(all.begin(), all.end(), [](const Pk &a, const Pk &b) { return a.de > b.de; });
    printf("globe centre (mass 2) at %.3f %.3f %.3f\n", P2.x, P2.y, P2.z);
    for (int q = 0; q < 20 && q < (int)all.size(); q++)
        printf("cx=%.3f cy=%.3f cz=%.3f   de=%.4f  r=%.3f\n", all[q].p.x, all[q].p.y, all[q].p.z, all[q].de, len(sub(all[q].p, P2)));
}

// Minimal PNG writer (8-bit RGB, no compression, no library needed).
static unsigned crc_tab[256];
static unsigned crc32(const unsigned char *d, size_t n, unsigned c) { c = ~c; for (size_t i = 0; i < n; i++) c = crc_tab[(c ^ d[i]) & 255] ^ (c >> 8); return ~c; }
static void put32(std::vector<unsigned char> &b, unsigned x) { b.push_back(x >> 24); b.push_back(x >> 16); b.push_back(x >> 8); b.push_back(x); }
static void chunk(FILE *f, const char *tag, const std::vector<unsigned char> &data) {
    std::vector<unsigned char> b; put32(b, (unsigned)data.size()); fwrite(b.data(), 1, 4, f);
    std::vector<unsigned char> body(tag, tag + 4); body.insert(body.end(), data.begin(), data.end());
    fwrite(body.data(), 1, body.size(), f);
    b.clear(); put32(b, crc32(body.data(), body.size(), 0)); fwrite(b.data(), 1, 4, f);
}
static void write_png(const char *fn, const unsigned char *rgb, int W, int H) {
    for (unsigned n = 0; n < 256; n++) { unsigned c = n; for (int k = 0; k < 8; k++) c = c & 1 ? 0xEDB88320u ^ (c >> 1) : c >> 1; crc_tab[n] = c; }
    std::vector<unsigned char> raw; raw.reserve((size_t)H * (W * 3 + 1));
    for (int y = 0; y < H; y++) { raw.push_back(0); raw.insert(raw.end(), rgb + (size_t)y * W * 3, rgb + (size_t)(y + 1) * W * 3); }
    std::vector<unsigned char> z; z.push_back(0x78); z.push_back(0x01);          // zlib header, then "stored" blocks
    unsigned a = 1, b = 0; for (unsigned char c : raw) { a = (a + c) % 65521; b = (b + a) % 65521; }
    for (size_t pos = 0; pos < raw.size();) {
        size_t n = std::min<size_t>(65535, raw.size() - pos);
        z.push_back(pos + n == raw.size()); z.push_back(n & 255); z.push_back(n >> 8); z.push_back(~n & 255); z.push_back((~n >> 8) & 255);
        z.insert(z.end(), raw.begin() + pos, raw.begin() + pos + n); pos += n;
    }
    put32(z, (b << 16) | a);
    FILE *f = fopen(fn, "wb"); if (!f) { perror(fn); return; }
    const unsigned char sig[8] = {137, 80, 78, 71, 13, 10, 26, 10}; fwrite(sig, 1, 8, f);
    std::vector<unsigned char> ih; put32(ih, W); put32(ih, H); ih.push_back(8); ih.push_back(2); ih.push_back(0); ih.push_back(0); ih.push_back(0);
    chunk(f, "IHDR", ih); chunk(f, "IDAT", z); chunk(f, "IEND", {}); fclose(f);
}

// =====================================================================================
//  SECTION K — MAIN
// =====================================================================================
int main(int argc, char **argv) {
    // ---- read  name=value  arguments (names as listed in SECTION A)
    for (int i = 1; i < argc; i++) {
        char *eq = strchr(argv[i], '='); if (!eq) { fprintf(stderr, "ignored '%s' (use name=value)\n", argv[i]); continue; }
        *eq = 0; const char *k = argv[i]; double x = atof(eq + 1);
        if (!strcmp(k, "w")) IMG_W = (int)x; else if (!strcmp(k, "h")) IMG_H = (int)x; else if (!strcmp(k, "aa")) AA = (int)x;
        else if (!strcmp(k, "out")) OUT_FILE = eq + 1;
        else if (!strcmp(k, "cx")) CAM.x = x; else if (!strcmp(k, "cy")) CAM.y = x; else if (!strcmp(k, "cz")) CAM.z = x;
        else if (!strcmp(k, "lx")) LOOK.x = x; else if (!strcmp(k, "ly")) LOOK.y = x; else if (!strcmp(k, "lz")) LOOK.z = x;
        else if (!strcmp(k, "fov")) FOV = x;
        else if (!strcmp(k, "m1")) M1 = x; else if (!strcmp(k, "m2")) M2 = x; else if (!strcmp(k, "a")) ORBIT_A = x;
        else if (!strcmp(k, "e")) ORBIT_E = x; else if (!strcmp(k, "phase")) ORBIT_PHASE = x; else if (!strcmp(k, "incl")) ORBIT_INCL = x;
        else if (!strcmp(k, "wa")) W_MASS_POW = x; else if (!strcmp(k, "wb")) W_DIST_POW = x;
        else if (!strcmp(k, "lapse")) LAPSE = x; else if (!strcmp(k, "lens")) LENS = x;
        else if (!strcmp(k, "pn")) BULB_POWER = x; else if (!strcmp(k, "sa")) BULB_SIZE = x; else if (!strcmp(k, "bs")) BOX_SCALE = x;
        else if (!strcmp(k, "sb")) BOX_SIZE = x; else if (!strcmp(k, "minr2")) BOX_MINR2 = x;
        else if (!strcmp(k, "maxit")) MAX_ITER = (int)x; else if (!strcmp(k, "bail")) BAILOUT = x;
        else if (!strcmp(k, "epsw")) SURF_EPS = x; else if (!strcmp(k, "fud")) STEP_SAFETY = x; else if (!strcmp(k, "steps")) MAX_STEPS = (int)x;
        else if (!strcmp(k, "sx")) SUN_DIR.x = x; else if (!strcmp(k, "sy")) SUN_DIR.y = x; else if (!strcmp(k, "sz")) SUN_DIR.z = x;
        else if (!strcmp(k, "suni")) SUN_POWER = x;
        else if (!strcmp(k, "px")) { LAMP_POS.x = x; LAMP_SET = true; } else if (!strcmp(k, "py")) LAMP_POS.y = x; else if (!strcmp(k, "pz")) LAMP_POS.z = x;
        else if (!strcmp(k, "headlamp")) LAMP_SET = !(x != 0);       // headlamp=1: put the lamp at the camera
        else if (!strcmp(k, "lampi")) LAMP_POWER = x; else if (!strcmp(k, "ambi")) AMBIENT = x; else if (!strcmp(k, "expo")) EXPOSURE = x;
        else if (!strcmp(k, "refl")) REFLECT = (int)x; else if (!strcmp(k, "pockets")) POCKETS = (int)x; else if (!strcmp(k, "help")) HELP = (int)x;
        else fprintf(stderr, "unknown parameter '%s' (see SECTION A of the source)\n", k);
    }
    if (HELP) {
        printf("image   w=%d h=%d aa=%d out=%s\n", IMG_W, IMG_H, AA, OUT_FILE);
        printf("camera  cx=%g cy=%g cz=%g  lx=%g ly=%g lz=%g  fov=%g\n", CAM.x, CAM.y, CAM.z, LOOK.x, LOOK.y, LOOK.z, FOV);
        printf("masses  m1=%g m2=%g  a=%g e=%g phase=%g incl=%g\n", M1, M2, ORBIT_A, ORBIT_E, ORBIT_PHASE, ORBIT_INCL);
        printf("mixing  wa=%g wb=%g lapse=%g lens=%g\n", W_MASS_POW, W_DIST_POW, LAPSE, LENS);
        printf("fractal pn=%g sa=%g bs=%g sb=%g minr2=%g maxit=%d bail=%g\n", BULB_POWER, BULB_SIZE, BOX_SCALE, BOX_SIZE, BOX_MINR2, MAX_ITER, BAILOUT);
        printf("quality epsw=%g fud=%g steps=%d refl=%d\n", SURF_EPS, STEP_SAFETY, MAX_STEPS, REFLECT);
        printf("light   sx=%g sy=%g sz=%g suni=%g  px=%g py=%g pz=%g lampi=%g (headlamp=1 puts it at the camera)  ambi=%g expo=%g\n",
               SUN_DIR.x, SUN_DIR.y, SUN_DIR.z, SUN_POWER, LAMP_POS.x, LAMP_POS.y, LAMP_POS.z, LAMP_POWER, AMBIENT, EXPOSURE);
        printf("other   pockets=1 lists camera positions; every parameter is explained in SECTION A of the source\n");
        return 0;
    }

    // ---- place the two masses on a Kepler orbit about their barycentre (the origin)
    //      solve Kepler's equation  E - e sin E = M  for the eccentric anomaly E
    double Mt = M1 + M2, Ma = ORBIT_PHASE * M_PI / 180, E = Ma;
    for (int i = 0; i < 30; i++) E -= (E - ORBIT_E * sin(E) - Ma) / (1 - ORBIT_E * cos(E));
    double ox = ORBIT_A * (cos(E) - ORBIT_E), oz = ORBIT_A * sqrt(1 - ORBIT_E * ORBIT_E) * sin(E), ci = cos(ORBIT_INCL * M_PI / 180), si = sin(ORBIT_INCL * M_PI / 180);
    V rel = v(ox, oz * si, oz * ci);                      // vector from mass 1 to mass 2
    P1 = mul(rel, -M2 / Mt); P2 = mul(rel, M1 / Mt); BARY = v(0, 0, 0);
    S1M = pow(M1, W_MASS_POW); S2M = pow(M2, W_MASS_POW);
    SUN = nrm(SUN_DIR); if (!LAMP_SET) LAMP_POS = CAM;
    fprintf(stderr, "Mandelbulb (mass 1) at %.3f %.3f %.3f   Mandelbox/globe (mass 2) at %.3f %.3f %.3f\n", P1.x, P1.y, P1.z, P2.x, P2.y, P2.z);
    if (POCKETS) { find_pockets(); return 0; }

    // ---- camera frame: forward, right, up
    V fw = nrm(sub(LOOK, CAM)), rt = nrm(cross(fw, v(0, 1, 0))), up = cross(rt, fw);
    double tf = tan(FOV * M_PI / 360); SUBPIX = 2 * tf / (IMG_H * AA);
    fprintf(stderr, "camera clearance de=%.4f   %dx%d, %d rays per pixel, %d threads\n", map(CAM).de, IMG_W, IMG_H, AA * AA, omp_get_max_threads());

    // ---- render. Rows are independent, so OpenMP hands them out to the threads one at a
    //      time (dynamic schedule: rows differ a lot in cost). Set OMP_NUM_THREADS to limit threads.
    int W = IMG_W, H = IMG_H;
    float *img = (float *)malloc(sizeof(float) * 3 * W * H); int done = 0;
    #pragma omp parallel for schedule(dynamic, 1)
    for (int y = 0; y < H; y++) {
        for (int x = 0; x < W; x++) {
            V acc = v(0, 0, 0);
            for (int sy = 0; sy < AA; sy++) for (int sx = 0; sx < AA; sx++) {        // AA x AA regular sub-pixel grid
                double u = ((x + (sx + 0.5) / AA) / W * 2 - 1) * tf * W / H, w_ = (1 - (y + (sy + 0.5) / AA) / H * 2) * tf;
                V rd = nrm(add(fw, add(mul(rt, u), mul(up, w_))));
                Hit h = march(CAM, rd, PRIMARY_RANGE, MAX_STEPS, 1, 1.0);
                V c = h.hit ? shade_pbr(h, mul(h.rd, -1), 0) : (h.absorbed ? v(0, 0, 0) : sky(h.rd));
                // tone-map each sample before averaging, so very bright edges still antialias cleanly
                acc = add(acc, v(pow(aces(c.x * EXPOSURE), 1 / GAMMA), pow(aces(c.y * EXPOSURE), 1 / GAMMA), pow(aces(c.z * EXPOSURE), 1 / GAMMA)));
            }
            acc = mul(acc, 1.0 / (AA * AA));
            img[3 * (y * W + x)] = acc.x; img[3 * (y * W + x) + 1] = acc.y; img[3 * (y * W + x) + 2] = acc.z;
        }
        #pragma omp critical
        { done++; if (done % 20 == 0) { fprintf(stderr, "rows %d/%d\n", done, H); fflush(stderr); } }
    }

    // ---- vignette, convert to 8 bits, write
    std::vector<unsigned char> rgb((size_t)W * H * 3);
    for (int i = 0; i < W * H; i++) {
        int px = i % W, py = i / W; double dx = (px + 0.5) / W - 0.5, dy = (py + 0.5) / H - 0.5, vg = 1 - VIGNETTE * (dx * dx + dy * dy);
        for (int k = 0; k < 3; k++) rgb[3 * (size_t)i + k] = (unsigned char)(255 * clampd(img[3 * i + k] * vg, 0, 1) + 0.5);
    }
    size_t L = strlen(OUT_FILE);
    if (L > 4 && !strcmp(OUT_FILE + L - 4, ".png")) write_png(OUT_FILE, rgb.data(), W, H);
    else { FILE *f = fopen(OUT_FILE, "wb"); if (!f) { perror(OUT_FILE); return 1; } fprintf(f, "P6\n%d %d\n255\n", W, H); fwrite(rgb.data(), 1, rgb.size(), f); fclose(f); }
    fprintf(stderr, "wrote %s\n", OUT_FILE);
    free(img); return 0;
}
