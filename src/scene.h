#ifndef _SCENE_
#define _SCENE_

#include <ncurses.h>

/* A vector */
typedef struct {
  double x;           /* x coordinate */
  double y;           /* y coordinate */
  double z;           /* z coordinate */
} vector_t; 

/* An area for the scene */
typedef struct {
  double min_x, max_x;
  double min_y, max_y;
  double min_z, max_z;
} area_t;

/* A sphere */
typedef struct {
  vector_t center;    /* Center */
  double radius;      /* Radius */
  int color;          /* Color */
} sphere_t;

#define MAX_SPHERES 10

/* A scene that gathers spheres */
typedef struct {
  int nb;             /* Number of spheres in the scene */
  area_t area;        /* Area of the scene */
  int empty[MAX_SPHERES]; /* TRUE if it's empty */
  sphere_t objs[MAX_SPHERES]; /* Spheres of the scene */
  vector_t directions[MAX_SPHERES]; /* Directions of the spheres */
  vector_t camera;    /* Position of the camera */
  double focal;       /* Focal angle */
} scene_t;

/* A ray */
typedef struct {
  vector_t origin;    /* Origin */
  vector_t direction; /* Direction */
} ray_t;

/* A picture of pixels */
typedef struct {
  int height;         /* Height */
  int width;          /* Width */
  int *pixels;        /* Pixels */
} picture_t;

/**
 * Normalize a vector.
 * @param vector the vector
 */
void vector_normalize(vector_t *vector);

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
 * @param area the area of the scene
 * @param camera the position of the camera
 * @param focal the focal angle
 */
void scene_initialize(scene_t *scene, area_t *area, vector_t *camera, double focal);

/**
 * Add a sphere to a scene.
 * @param scene the scene
 * @param index the index
 * @param sphere the sphere
 * @param direction the direction
 */
void scene_add(scene_t *scene, unsigned int index, sphere_t *sphere, vector_t *direction);

/**
 * Check if a ray intersects a sphere.
 * @param r the ray
 * @param c the sphere
 * @param t the distance
 * @return the color or 0 if no intersection
 */
int intersect_sphere(ray_t r, sphere_t c, double *t);

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
 * @param picture the picture
 */
void launch_rays(scene_t *scene, picture_t *picture);

/**
 * Display a picture into a window.
 * @param window the window
 * @param picture the picture
 */
void update_window(WINDOW *window, picture_t *picture);

/**
 * Check if there's a collision between two spheres.
 * @param s1 the first sphere
 * @param s2 the second sphere
 * @return TRUE if there's a collision
 */
int sphere_collision(sphere_t *s1, sphere_t *s2);

/**
 * Move a sphere of the scene.
 * @param scene the scene
 * @param index the index of the sphere
 */
void sphere_move(scene_t *scene, int index);

/**
 * Move a sphere of the scene.
 * @param scene the scene
 * @param index the index of the sphere
 */
void sphere_move(scene_t *scene, int index);

/**
 * Update the scene.
 * @param scene the scene
 */
void scene_update(scene_t *scene);

#endif
