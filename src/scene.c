#include "scene.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "ncurses.h"
#include "include.h"

/**
 * Normalize a vector.
 * @param vector the vector
 */
void vector_normalize(vector_t *vector) {
  double n = sqrt(vector->x * vector->x + vector->y * vector->y + vector->z * vector->z);
  if(n != 0.) {
    vector->x /= n;
    vector->y /= n;
    vector->z /= n;
  }
}

/**
 * Initialize a picture.
 * @param picture the picture
 * @param height the height
 * @param width the width
 */
void picture_initialize(picture_t *picture, int height, int width) {
  picture->height = height;
  picture->width = width;

  if((picture->pixels = (int*)malloc(sizeof(int) * height * width)) == NULL) {
    ncurses_stop();
    perror("Error when allocating the picture");
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
 * @param area the area of the scene
 * @param camera the position of the camera
 * @param focal the focal angle
 */
void scene_initialize(scene_t *scene, area_t *area, vector_t *camera, double focal) {
  int i;

  scene->area = *area;
  scene->camera = *camera;
  scene->focal = focal;
  scene->nb = 0;
  memset(scene->objs, 0, sizeof(scene->objs));
  memset(scene->directions, 0, sizeof(scene->directions));
  for(i = 0; i < MAX_SPHERES; i++)
    scene->empty[i] = TRUE;
}

/**
 * Add a sphere to a scene.
 * @param scene the scene
 * @param index the index
 * @param sphere the sphere
 * @param direction the direction
 */
void scene_add(scene_t *scene, unsigned int index, sphere_t *sphere, vector_t *direction) {
  if(index < MAX_SPHERES) {
    if(scene->empty[index] == TRUE) {
      scene->nb++;
      scene->empty[index] = FALSE;
    }
    scene->objs[index] = *sphere;
    scene->directions[index] = *direction;
  }
}

/**
 * Check if a ray intersects a sphere.
 * @param r the ray
 * @param c the sphere
 * @param t the distance along the ray 
 * @return the color or 0 if no intersection
 */
int intersect_sphere(ray_t r, sphere_t c, double *t){
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
 * Launch a ray on the scene and computes the pixels.
 * @param r the ray
 * @param scene the scene
 * @return the color or 0 if no intersection
 */
int launch_ray(ray_t r, scene_t *scene) {
  int i, res = 0, color = 0;
  double p =0., max = 0.;
  
  for(i = 0; i < MAX_SPHERES; ++i) {
    if((scene->empty[i] == FALSE) &&
       (color = intersect_sphere(r, scene->objs[i], &p)) != 0) {
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
  double step_y = ((double)picture->height)/((double)picture->height)*2.;
  double step_x = ((double)picture->width)/((double)picture->width);
  int i, j;
  ray_t r;
  int *iterator = &picture->pixels[0];

  /* Origin of the rays */
  r.origin = scene->camera;

  /* Compute pixels */
  for(i = 0; i < picture->height; ++i) {
    for(j = 0; j < picture->width; ++j) {
      /* Compute the direction of the ray */
      r.direction.x = (r.origin.x - (j * step_x - picture->width / 2.)) * scene->focal;
      r.direction.y = (r.origin.y - (i * step_y - picture->height)) * scene->focal;
      r.direction.z = 1;
      vector_normalize(&r.direction);

      /* Compute the pixel color */
      *iterator = launch_ray(r, scene);
      ++iterator;
      /*picture->pixels[i * picture->width + j] = launch_ray(r, scene);*/
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
      if((i == 0) && (j == 0)) {
	mvwprintw(window, picture->height - i, j, "X");
      }
      else {
      if(*iterator != 0) {
        wattron(window, COLOR_PAIR(*iterator));
        mvwprintw(window, picture->height - i - 1, j, " ");
        wattroff(window, COLOR_PAIR(*iterator));
      }
      }
      ++iterator;
    }
  }
}

/**
 * Check if there's a collision between two spheres.
 * @param s1 the first sphere
 * @param s2 the second sphere
 * @return TRUE if there's a collision
 */
int sphere_collision(sphere_t *s1, sphere_t *s2) {
  vector_t tmp;
  double dist;

  /* Compute the distance between the spheres */
  tmp.x = s1->center.x - s2->center.x;
  tmp.y = s1->center.y - s2->center.y;
  tmp.z = s1->center.z - s2->center.z;
  dist = sqrt(tmp.x * tmp.x + tmp.y * tmp.y + tmp.z * tmp.z);

  /* Return true if the distance is lower the sum of raduises */
  return (dist < s1->radius + s2->radius);  
}

/**
 * Move a sphere of the scene.
 * @param scene the scene
 * @param index the index of the sphere
 */
void sphere_move(scene_t *scene, int index) {
  sphere_t tmp = scene->objs[index];
  int i = 0, stop = 0;

  /* Compute the new center of the sphere */
  tmp.center.x += scene->directions[index].x;
  tmp.center.y += scene->directions[index].y;
  tmp.center.z += scene->directions[index].z;

  /* Check for collision with other spheres of the scene */
  while((i < MAX_SPHERES) && (stop == 0)) {
    if((i != index) && (scene->empty[i] == FALSE) &&
       (sphere_collision(&scene->objs[i], &tmp)))
      stop = 1;
    i++;
  }

  if(stop == 1) {
    /* A collision: invert the direction of the sphere */
    scene->directions[index].x = -scene->directions[index].x;
    scene->directions[index].y = -scene->directions[index].y;
    scene->directions[index].z = -scene->directions[index].z;
  }
  else {
    /* Move the sphere and check the area of the scene */

    scene->objs[index].center.x += scene->directions[index].x;
    if(scene->objs[index].center.x < scene->area.min_x) {
      scene->directions[index].x = -scene->directions[index].x;
      scene->objs[index].center.x = scene->area.min_x;
    }
    if(scene->objs[index].center.x > scene->area.max_x) {
      scene->directions[index].x = -scene->directions[index].x;
      scene->objs[index].center.x = scene->area.max_x;
    }

    scene->objs[index].center.y += scene->directions[index].y;
    if(scene->objs[index].center.y < scene->area.min_y) {
      scene->directions[index].y = -scene->directions[index].y;
      scene->objs[index].center.y = scene->area.min_y;
    }
    if(scene->objs[index].center.y > scene->area.max_y) {
      scene->directions[index].y = -scene->directions[index].y;
      scene->objs[index].center.y = scene->area.max_y;
    }

    scene->objs[index].center.z += scene->directions[index].z;
    if(scene->objs[index].center.z < scene->area.min_z) {
      scene->directions[index].z = -scene->directions[index].z;
      scene->objs[index].center.z = scene->area.min_z;
    }
    if(scene->objs[index].center.z > scene->area.max_z) {
      scene->directions[index].z = -scene->directions[index].z;
      scene->objs[index].center.z = scene->area.max_z;
    }
  }
}

/**
 * Update the scene.
 * @param scene the scene
 */
void scene_update(scene_t *scene) {
  int i;

  for(i = 0; i < MAX_SPHERES; ++i)
    if(scene->empty[i] == FALSE)
      sphere_move(scene, i);
}
