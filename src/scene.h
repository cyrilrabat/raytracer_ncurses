#ifndef _SCENE_
#define _SCENE_

#include <ncurses.h>

/* A vector */
typedef struct {
  double x;            /* x coordinate */
  double y;            /* y coordinate */
  double z;            /* z coordinate */
} vector_t; 

typedef vector_t point_t;

/* A sphere */
typedef struct {
  point_t center;     /* Center */
  double radius;      /* Radius */
  int color;          /* Color */
} sphere_t;

#define OBJECT_SPHERE 1

/* An object of a scene */
typedef struct {
  int type;           /* Object type */
  union {
    sphere_t sphere;  /* Sphere */
                      /* We can add any other objects here */
  } obj;
} object_t;

#define MAX_OBJECTS 10

/* A scene that gathers objects */
typedef struct {
  int nb;             /* Number of objects in the scene */
  object_t objs[MAX_OBJECTS]; /* Objects of the scene */
} scene_t;

/* A ray */
typedef struct {
  point_t origin;     /* Origin */
  vector_t direction; /* Direction */
} ray_t;

/* A picture of pixels */
typedef struct {
  int height;         /* Height */
  int width;          /* Width */
  int *pixels;        /* Pixels */
} picture_t;

/**
 * Initialize a picture.
 * @param picture the picture
 * @param height the height
 * @param width the width
 */
void picture_initialize(picture_t *picture, int height, int width);

/**
 * Delete a picture.
 * @param picture the picture
 */
void picture_delete(picture_t *picture);

/**
 * Initialize a scene.
 * @param scene the scene
 */
void scene_initialize(scene_t *scene);

/**
 * Add an object to a scene.
 * @param scene the scene
 * @param obj the object
 */
void scene_add(scene_t *scene, object_t *obj);

/**
 * Check if a ray intersects a sphere.
 * @param r the ray
 * @param c the sphere
 * @param t the distance
 * @return the color or 0 if no intersection
 */
int intersect_sphere(ray_t r, sphere_t c, double *t);

/**
 * Check if a ray intersects an object.
 * @param r the ray
 * @param c the object
 * @param t the distance
 * @return the color or 0 if no intersection
 */
int intersect_object(ray_t r, object_t c, double * t);

/**
 * Launch a ray on the scene and computes the pixels.
 * @param r the ray
 * @param scene the scene
 * @return the color or 0 if no intersection
 */
int launch_ray(ray_t r, scene_t *scene);

/**
 * Launch rays on the scene and computes the pixels of the picture.
 * @param scene the scene
 * @param dist_x
 * @param dist_y
 * @param picture the picture
 */
void launch_rays(scene_t *scene, double dist_x, double dist_y, picture_t *picture);

/**
 * Display a picture into a window.
 * @param window the window
 * @param picture the picture
 */
void update_window(WINDOW *window, picture_t *picture);

/**
 * Rotate the scene.
 * @param angle the angle
 * @param scene the scene
 */
void rotate_y(double angle, scene_t *scene);

#endif
