/**
 * A ray tracor in ncurses.
 * @author Julien Loiseau, Cyril Rabat
 * @version 15/03/2018
 **/

#define _BSD_SOURCE

#include <stdlib.h>
#include <unistd.h>

#include "ncurses.h"
#include "include.h"
#include "scene.h"

/**
 * Create the scene.
 * @param scene the scene
 */
void create_scene(scene_t *scene) {
  object_t objs[4] = {
    { OBJECT_SPHERE, { { {0. ,0. ,0.}, 8., 1} } },
    { OBJECT_SPHERE, { { {20., 0., 0.}, 6., 2} } },
    { OBJECT_SPHERE, { { {-10., 10., 0.}, 6., 3} } },
    { OBJECT_SPHERE, { { {-25.,-15., 0.}, 10., 4} } }
  };
  int i;

  for(i = 0; i < 4; i++)
    scene_add(scene, &objs[i], i);
}

/**
 * Main function.
 * @return EXIT_SUCCESS on success or EXIT_FAILURE on error
 */
int main() {
  WINDOW *window, *display;
  picture_t picture;
  scene_t scene;
  int timer = 0;
  int step_time = 100000;
  int max_timer = 10*1000000;
  double rot_angle = M_PI / 20.;
  
  /* Initialize the scene and the picture */
  scene_initialize(&scene);
  create_scene(&scene);
  picture_initialize(&picture, HEIGHT - 2, WIDTH - 2);

  /* ncurses initialization */
  ncurses_initialize();
  ncurses_colors(); 
  if(ncurses_checksize(WIDTH, HEIGHT) == FALSE) {
    ncurses_stop();
    fprintf(stderr, "Terminal is too small; actual size (%d,%d); must be (%d,%d)\n", COLS, LINES, WIDTH, HEIGHT);
    exit(EXIT_FAILURE);
  }
  
  /* colors initialisation */
  init_pair(1, COLOR_RED, COLOR_RED);
  init_pair(2, COLOR_BLUE, COLOR_BLUE);
  init_pair(3, COLOR_GREEN, COLOR_GREEN);
  init_pair(4, COLOR_YELLOW, COLOR_YELLOW);
  
  /* Create the window */
  bkgd(COLOR_PAIR(2));
  refresh();
  window = newwin(HEIGHT, WIDTH, 0, 0);
  box(window,0,0);
  display = subwin(window, HEIGHT - 2, WIDTH - 2, 1, 1);

  /* Compute first picture */
  launch_rays(&scene, &picture); 
  update_window(display, &picture);
  wrefresh(window);

  /* Main loop */
  while(timer < max_timer){
    werase(display);
    scene_rotate(rot_angle, &scene);
    launch_rays(&scene, &picture); 
    update_window(display, &picture);
    wrefresh(display);
    usleep(step_time);
    timer += step_time;
  }

  /* Delete window */
  delwin(display);
  delwin(window);

  /* Free ressources */
  picture_delete(&picture);
  
  /* Stop ncurses */
  ncurses_stop();

  return EXIT_SUCCESS;
}
