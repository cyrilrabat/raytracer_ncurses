#include "scene.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "ncurses.h"
#include "include.h"

/**
 * Initialize a picture.
 * @param picture the picture
 * @param height the height
 * @param width the width
 */
void picture_initialize(picture_t *picture, int height, int width) {
  picture->height = height;
  picture->width = width;

  picture->pixels = (int*)malloc(sizeof(int) * height * width);
  if(picture->pixels == NULL) {
    ncurses_stop();
    perror("Error when initializing the picture");
    exit(EXIT_FAILURE);
  }

  memset(picture->pixels, 0, sizeof(int) * height * width);
}

/**
 * Delete a picture.
 * @param picture the picture
 */
void picture_delete(picture_t *picture) {
  if(picture->pixels != NULL) {
    free(picture->pixels);
    picture->pixels = NULL;
  }
}

/**
 * Initialize a scene.
 * @param scene the scene
 */
void scene_initialize(scene_t *scene) {
  int i;

  scene->nb = 0;
  memset(&scene->objs, 0, sizeof(scene->objs));
  for(i = 0; i < MAX_OBJECTS; i++)
    scene->objs[i].type = OBJECT_NONE;
}

/**
 * Add an object to a scene.
 * @param scene the scene
 * @param obj the object
 * @param index the index
 */
void scene_add(scene_t *scene, object_t *obj, unsigned int index) {
  if(index < MAX_OBJECTS) {
    if(scene->objs[index].type == OBJECT_NONE)
      scene->nb++;
    scene->objs[index] = *obj;
  }
}

/**
 * Check if a ray intersects a sphere.
 * @param r the ray
 * @param c the sphere
 * @param t the distance along the ray 
 * @return the color or 0 if no intersection
 */
int intersect_sphere(ray_t r, sphere_t c, double * t){
  vector_t l;
  double tc, d, t1c;
  int result = 0;
  double t1;

  /* Vector L = (ray.origin,sphere.center) */
  l.x = c.center.x - r.origin.x; 
  l.y = c.center.y - r.origin.y; 
  l.z = c.center.z - r.origin.z;
  /* Dot product (L).(ray.direction) = projection of L on ray = tc */
  tc = l.x * r.direction.x + l.y * r.direction.y + l.z * r.direction.z;
  
  /* If dot product>=0 => angle of less than 90deg = possible intersection */ 
  if(tc >= 0.0) {
    /* Right triangle defined by L, d and tc  
    * d^2 = L.L - tc*tc*/
    d = sqrt((l.x * l.x + l.y * l.y + l.z * l.z) - tc * tc);
    
    /* If d less than the radius, two intersections*/
    if(d <= c.radius) {
      /* Distance in triangle d, t1c and radius
      * t1c^2 = r^2 - d^2. (Koona t'chuta Solo?)
      */
      t1c = sqrt(c.radius*c.radius-d*d);
      t1 = tc - t1c;
      *t = tc + t1c;
      
      /* Keep the closest solution */
      if(t1 > *t)
	      *t = t1;

      /* Get object's color */
      result = c.color;
    }
  }

  return result; 
}

/**
 * Check if a ray intersects an object.
 * @param r the ray
 * @param c the object
 * @param t the distance
 * @return the color or 0 if no intersection
 */
int intersect_object(ray_t r, object_t c, double * t) {
  int result = 0;

  switch(c.type) {
  case OBJECT_SPHERE:
    result = intersect_sphere(r, c.obj.sphere, t);
    break;
    /* Add other cases for other objects */
  }

  return result;
}

/**
 * Launch a ray on the scene and computes the pixels.
 * @param r the ray
 * @param scene the scene
 * @return the color or 0 if no intersection
 */
int launch_ray(ray_t r, scene_t *scene) {
  int i, res = 0, color = 0;
  double p =0., max = 0.;
  
  for(i = 0; i < MAX_OBJECTS; ++i) {
    if((scene->objs[i].type != OBJECT_NONE) &&
       (color = intersect_object(r, scene->objs[i], &p)) != 0) {
      if(max < p){
        max = p;
        res = color;
      }
    }
  }

  return res;
}

/**
 * Launch rays on the scene and computes the pixels of the picture.
 * @param scene the scene
 * @param picture the picture
 */
void launch_rays(scene_t *scene, picture_t *picture) {
  double step_y = ((double)HEIGHT/(double)HEIGHT)*2.;
  double step_x = ((double)WIDTH/(double)WIDTH);
  int i, j;
  ray_t r;

  for(i = 0; i < picture->height; ++i){
    r.direction.x = 0.;
    r.direction.y = 0.;
    r.direction.z = 1.;
    r.origin.y = i * step_y - (2 * picture->height) / 2.;
    r.origin.z = -50.;
    for(j = 0; j < picture->width; ++j){
      r.origin.x = j * step_x - picture->width / 2.;
      picture->pixels[i * picture->width + j] = launch_ray(r, scene);
    } 
  }
}

/**
 * Display a picture into a window.
 * @param window the window
 * @param picture the picture
 */
void update_window(WINDOW *window, picture_t *picture) {
  int i, j;
  int *iterator = &picture->pixels[0];

  for(i = 0; i < picture->height; ++i){
    for(j = 0; j < picture->width; ++j){
      if(*iterator != 0) {
        wattron(window, COLOR_PAIR(*iterator));
        mvwprintw(window, picture->height - i, j, " ");
        wattroff(window, COLOR_PAIR(*iterator));
      }
      ++iterator;
    }
  }
}

/**
 * Rotate an object of the scene.
 * @param angle the angle
 * @param obj the object
 */
void object_rotate(double angle, object_t *obj) {
  sphere_t *sphere;
  double new_x, new_z;

  switch(obj->type) {
  case OBJECT_SPHERE:
    sphere = &obj->obj.sphere;
    new_x = sphere->center.x * cos(angle) +
      sphere->center.z * sin(angle);
    new_z = -sphere->center.x * sin(angle) +
      sphere->center.z * cos(angle);
    sphere->center.x = new_x;
    sphere->center.z = new_z; 
    break;
    /* Add other cases for other object types */
  }
}

/**
 * Rotate the scene.
 * @param angle the angle
 * @param scene the scene
 */
void scene_rotate(double angle, scene_t *scene) {
  int i;

  for(i = 0; i < MAX_OBJECTS; ++i)
    if(scene->objs[i].type != OBJECT_NONE)
      object_rotate(angle, &scene->objs[i]);
}
