#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define maxColor 255
#define format '3'

// Structure to hold RGB pixel data
typedef struct RGBpixel {
  unsigned char R, G, B;
} RGBpixel;

// Global variables to hold image data
int numPixels; // total number of pixels in image (w * h)
RGBpixel *pixmap; // array of pixels to hold the image data

// Structure to hold object data in the scene
typedef struct {
  int kind; // 0 = plane, 1 = sphere
  double color[3];
  union {
    struct {
      char color;
      double position[3];
      double normal[3];
    } plane;
    struct {
      char color;
      double position[3];
      double radius;
    } sphere;
  };
} Object;


static inline double sqr(double v) {
  return v*v;
}


static inline void normalize(double* v) {
  double len = sqrt(sqr(v[0]) + sqr(v[1]) + sqr(v[2]));
  v[0] /= len;
  v[1] /= len;
  v[2] /= len;
}


double sphere_intersection(double* Ro, double* Rd, double* C, double r) {

  double a = sqr(Rd[0]) + sqr(Rd[1]) + sqr(Rd[2]);
  double b = 2 * (Rd[0] * (Ro[0] - C[0]) + Rd[1] * (Ro[1] - C[1]) + Rd[2] * (Ro[2] - C[2]));
  double c = sqr(Ro[0] - C[0]) + sqr(Ro[1] - C[1]) + sqr(Ro[2] - C[2]) - sqr(r);

  double det = sqr(b) - 4 * a * c;
  if (det < 0) return -1; // no intersection

  det = sqrt(det);

  double t0 = (-b - det) / (2 * a);
  if (t0 > 0) return t0;

  double t1 = (-b + det) / (2 * a);
  if (t1 > 0) return t1;

  return -1;
}


double plane_intersection(double* Ro, double* Rd, double* P, double* N) {
  double D = -(N[0] * P[0] + N[1] * P[1] + N[2] * P[2]); // distance from origin to plane
  double t = -(N[0] * Ro[0] + N[1] * Ro[1] + N[2] * Ro[2] + D) /
  (N[0] * Rd[0] + N[1] * Rd[1] + N[2] * Rd[2]);

  if (t > 0) return t;

  return -1; // no intersection
}


int main() {

  Object** objects;
  objects = malloc(sizeof(Object*) * 3);

  objects[0] = malloc(sizeof(Object));
  objects[0]->kind = 1;
  objects[0]->color[0] = 1.0;
  objects[0]->color[1] = 0;
  objects[0]->color[2] = 0;
  objects[0]->sphere.radius = 1;
  objects[0]->sphere.position[0] = 0;
  objects[0]->sphere.position[1] = 0;
  objects[0]->sphere.position[2] = 10;

  // objects[1] = malloc(sizeof(Object));
  // objects[1]->kind = 1;
  // objects[1]->color[0] = 0.5;
  // objects[1]->color[1] = 0;
  // objects[1]->color[2] = 0.8;
  // objects[1]->sphere.radius = 1;
  // objects[1]->sphere.position[0] = 0;
  // objects[1]->sphere.position[1] = 2;
  // objects[1]->sphere.position[2] = 2;

  objects[1] = malloc(sizeof(Object));
  objects[1]->kind = 0;
  objects[1]->color[0] = 0;
  objects[1]->color[1] = 1.0;
  objects[1]->color[2] = 0;
  objects[1]->plane.normal[0] = 0;
  objects[1]->plane.normal[1] = 1;
  objects[1]->plane.normal[2] = 1;
  objects[1]->plane.position[0] = 0;
  objects[1]->plane.position[1] = 0;
  objects[1]->plane.position[2] = 20;

  objects[2] = NULL;

  // camera
  double cx = 0;
  double cy = 0;
  double h = 0.7;
  double w = 0.7;

  int M = 100; // height in pixels
  int N = 100; // width in pixels

  numPixels = M * N;

  double pixheight = h / M;
  double pixwidth = w / N;

  pixmap = malloc(sizeof(RGBpixel) * numPixels);
  int pixIndex = 0;
  for (int y = 0; y < M; y++) { // for each row
    double y_coord = cy - (h/2) + pixheight * (y + 0.5); // y coord of the row

    for (int x = 0; x < N; x++) { // for each column

      double x_coord = cx - (w/2) + pixwidth * (x + 0.5); // x coord of the column
      double Ro[3] = {cx, cy, 0}; // position of camera
      double Rd[3] = {x_coord, y_coord, 1}; // position of pixel
      normalize(Rd); // normalize (P - Ro)

      double best_t = INFINITY;
      Object* object;
      for (int i = 0; objects[i] != NULL; i++) {
        double t = 0;

        switch (objects[i]->kind) {
          case 0:
          t = plane_intersection(Ro, Rd, objects[i]->plane.position, objects[i]->plane.normal);
          break;
          case 1:
          t = sphere_intersection(Ro, Rd, objects[i]->sphere.position, objects[i]->sphere.radius);
          break;
          default:
          fprintf(stderr, "Unrecognized object.");
          exit(1);
        }
        if (t > 0 && t < best_t) {
          best_t = t;
          object = malloc(sizeof(Object));
          memcpy(object, objects[i], sizeof(Object));
        }
      }

      if (best_t > 0 && best_t != INFINITY) {
        pixmap[pixIndex].R = (unsigned char)(object->color[0] * maxColor);
        pixmap[pixIndex].G = (unsigned char)(object->color[1] * maxColor);
        pixmap[pixIndex].B = (unsigned char)(object->color[2] * maxColor);
        //printf("%c", pixmap[x * y].R);
        //printf("[%i, %i, %i] ", pixmap[x * y].R, pixmap[x * y].G, pixmap[x * y].B);
      }
      else {
        pixmap[pixIndex].R = 255;
        pixmap[pixIndex].G = 255;
        pixmap[pixIndex].B = 255;
        //printf("[%i, %i, %i] ", pixmap[x * y].R, pixmap[x * y].G, pixmap[x * y].B);
      }
      pixIndex++;
      //printf(" %i ", x*y);
      printf("[%i, %i, %i] ", pixmap[x * y].R, pixmap[x * y].G, pixmap[x * y].B);
    }
    printf("\n");
  }

  return 0;
}
