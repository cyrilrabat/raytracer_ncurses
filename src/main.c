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
  sphere_t objs[4] = {
    { {0. ,0. ,0.}, 2., 2 },
    { {20., 0., 0.}, 2., 3 },
    { {-10., 10., 0.}, 2., 4 },
    { {-15.,-15., 0.}, 2., 5 }
  };
  vector_t directions[4] = {
    { 1., 0., 0. },
    { 1., 1., 0. },
    { 0., 0., 1. },
    { 0., 0.5, 1. }
  };
  int i;
  area_t area = { -30., 30., -30., 30., -30, 30. }; 
  vector_t camera = { 0., 0., -50 };

  scene_initialize(scene, &area, &camera, 0.018);
  for(i = 0; i < 4; i++)
    scene_add(scene, i, &objs[i], &directions[i]);
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
  int max_timer = 50*1000000;
  
  /* Initialize the scene and the picture */
  create_scene(&scene);
  picture_initialize(&picture, HEIGHT - 2, WIDTH - 2);

  /* ncurses initialization */
  ncurses_initialize();
  ncurses_colors(); 
  if(ncurses_checksize(HEIGHT, WIDTH) == FALSE) {
    ncurses_stop();
    fprintf(stderr, "Terminal is too small; actual size (%d,%d); must be (%d,%d)\n", COLS, LINES, WIDTH, HEIGHT);
    exit(EXIT_FAILURE);
  }
  
  /* colors initialisation : no more colors (ncurses!!!) */
  init_pair(1, COLOR_WHITE, COLOR_BLACK);
  init_pair(2, COLOR_RED, COLOR_RED);
  init_pair(3, COLOR_BLUE, COLOR_BLUE);
  init_pair(4, COLOR_GREEN, COLOR_GREEN);
  init_pair(5, COLOR_YELLOW, COLOR_YELLOW);
  
  /* Create the window */
  bkgd(COLOR_PAIR(1));
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
    scene_update(&scene);
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
