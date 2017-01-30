#ifndef __jem_ray__h
#define __jem_ray__h

#include "vec3.h"

namespace jtrace {
    struct Ray {
        Ray(double x0, double y0, double z0, double vx, double vy, double vz, double t);
        Ray(Vec3 _p0, Vec3 _v, double t);
        Vec3 p0; // reference position
        Vec3 v;  // "velocity" Vec3
        double t0; // reference time
        Vec3 operator()(double t) const {return p0+v*(t-t0);}

        std::string repr() const {
            std::ostringstream oss(" ");
            oss << "Ray(" << p0 << ", " << v << ", " << t0 << ")";
            return oss.str();
        }
    };

    inline std::ostream& operator<<(std::ostream& os, const Ray &r);
}

#endif
