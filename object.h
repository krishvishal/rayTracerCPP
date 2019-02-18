#ifndef _OBJECT_H
#define _OBJECT_H

#include "ray.h"
#include "linalg.h"
#include "color.h"

class Object {
public:

    Object ();

    // method functions
    virtual Color getColor () { return Color (0.0, 0.0, 0.0, 0); }

    virtual Vect getNormalAt(Vect intersection_position) {
        return Vect (0, 0, 0);
    }

    virtual double findIntersection(Ray ray) {
        return 0;
    }

};

Object::Object () {}

#endif