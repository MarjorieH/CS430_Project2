#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Structure to hold object data in the scene
typedef struct {
  int kind; // 0 = plane, 1 = sphere
  //double color[3];
  union {
    struct {
      double position[3];
      double normal[3];
    } plane;
    struct {
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
// t = -(AX0 + BY0 + CZ0 + D) / (AXd + BYd + CZd)
// D = -(N • P)
  double D = -(N[0] * P[0] + N[1] * P[1] + N[2] * P[2]); // distance from origin to plane
  double t = -(N[0] * Ro[0] + N[1] * Ro[1] + N[2] * Ro[2] + D) /
              (N[0] * Rd[0] + N[1] * Rd[1] + N[2] * Rd[2]);

  if (t >= 0) return t;

  return -1; // no intersection
}


int main() {

  Object** objects;
  objects = malloc(sizeof(Object*) * 2);
  objects[0] = malloc(sizeof(Object));
  objects[0]->kind = 0;
  objects[0]->plane.normal[0] = 0;
  objects[0]->plane.normal[1] = 1;
  objects[0]->plane.normal[2] = 1;
  // object[0]->teapot.handle_length = 2;
  objects[0]->plane.position[0] = 0;
  objects[0]->plane.position[1] = 0;
  objects[0]->plane.position[2] = 20;
  objects[1] = NULL;

  double cx = 0;
  double cy = 0;
  double h = 0.7;
  double w = 0.7;

  int M = 20;
  int N = 20;

  double pixheight = h / M;
  double pixwidth = w / N;
  for (int y = 0; y < M; y += 1) {
    for (int x = 0; x < N; x += 1) {
      double Ro[3] = {0, 0, 0};
      // Rd = normalize(P - Ro)
      double Rd[3] = {cx - (w/2) + pixwidth * (x + 0.5),
                      cy - (h/2) + pixheight * (y + 0.5),
                      1};
      normalize(Rd);

      double best_t = INFINITY;
      for (int i=0; objects[i] != 0; i += 1) {
        double t = 0;

        switch(objects[i]->kind) {
          case 0:
            t = plane_intersection(Ro, Rd, objects[i]->plane.position, objects[i]->plane.normal);
            break;
          case 1:
            t = sphere_intersection(Ro, Rd, objects[i]->sphere.position, objects[i]->sphere.radius);
            break;
          default:
            // Horrible error
            exit(1);
          }
          if (t > 0 && t < best_t) best_t = t;
        }
        if (best_t > 0 && best_t != INFINITY) {
          printf("#");
        } else {
          printf(".");
        }

      }
      printf("\n");
    }

    return 0;
  }
