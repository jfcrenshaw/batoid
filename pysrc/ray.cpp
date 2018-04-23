#include "ray.h"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>
#include <pybind11/operators.h>
#include <pybind11/complex.h>
#include <pybind11/numpy.h>
#include <pybind11/eigen.h>


// PYBIND11_MAKE_OPAQUE(std::vector<batoid::Ray>);

namespace py = pybind11;
using namespace pybind11::literals;

namespace batoid {
    void pyExportRay(py::module& m) {
        // PYBIND11_NUMPY_DTYPE(Vector3d, data);
        // PYBIND11_NUMPY_DTYPE(Ray, p0, v, t0, wavelength, isVignetted, failed);
        py::class_<Ray>(m, "Ray")
            .def(py::init<double,double,double,double,double,double,double,double,bool>(),
                 "init",
                 "x0"_a, "y0"_a, "z0"_a, "vx"_a, "vy"_a, "vz"_a, "t"_a=0.0,
                 "w"_a=0.0, "isV"_a=false)
            .def(py::init<Vector3d,Vector3d,double,double,bool>(),
                 "init",
                 "p0"_a, "v"_a, "t"_a=0.0, "w"_a=0.0, "isV"_a=false)
            // .def(py::init<std::array<double,3>,std::array<double,3>,double,double,bool>(),
            //      "init",
            //      "p0"_a, "v"_a, "t"_a=0.0, "w"_a=0.0, "isV"_a=false)
            .def_readonly("p0", &Ray::p0)
            .def_readonly("v", &Ray::v)
            .def_readonly("t0", &Ray::t0)
            .def_readonly("wavelength", &Ray::wavelength)
            .def_readonly("isVignetted", &Ray::isVignetted)
            .def_readonly("failed", &Ray::failed)
            .def_property_readonly("x0", [](const Ray& r){ return r.p0[0]; })
            .def_property_readonly("y0", [](const Ray& r){ return r.p0[1]; })
            .def_property_readonly("z0", [](const Ray& r){ return r.p0[2]; })
            .def_property_readonly("vx", [](const Ray& r){ return r.v[0]; })
            .def_property_readonly("vy", [](const Ray& r){ return r.v[1]; })
            .def_property_readonly("vz", [](const Ray& r){ return r.v[2]; })
            .def_property_readonly("k", &Ray::k)
            .def_property_readonly("omega", &Ray::omega)
            .def("positionAtTime", &Ray::positionAtTime)
            .def("__repr__", &Ray::repr)
            .def(py::self == py::self)
            .def(py::self != py::self)
            .def("phase", &Ray::phase)
            .def("amplitude", &Ray::amplitude)
            .def("propagatedToTime", &Ray::propagatedToTime)
            .def("propagateInPlace", &Ray::propagateInPlace)
            .def(py::pickle(
                [](const Ray& r) { // __getstate__
                    return py::make_tuple(r.p0, r.v, r.t0, r.wavelength, r.isVignetted, r.failed);
                },
                [](py::tuple t) { // __setstate__
                    Ray r(
                        t[0].cast<Vector3d>(),
                        t[1].cast<Vector3d>(),
                        t[2].cast<double>(),
                        t[3].cast<double>(),
                        t[4].cast<bool>()
                    );
                    if (t[5].cast<bool>())
                        r.setFail();
                    return r;
                }
            ))
            .def("__hash__", [](const Ray& r) {
                auto result = py::hash(py::make_tuple("Ray", r.t0, r.wavelength, r.isVignetted, r.failed));
                const double* d = &r.p0[0];
                for (int i=0; i<3; i++)
                    result = 1000003*result ^ py::hash(py::float_(d[i]));
                d = &r.v[0];
                for (int i=0; i<3; i++)
                    result = 1000003*result ^ py::hash(py::float_(d[i]));
                result = (result == -1) ? -2 : result;
                return result;
            });

        m.def("amplitudeMany", [](const RayVector& rv, const Vector3d& r, double t){
            return amplitudeMany(rv.rays, r, t);
        });
        m.def("phaseMany", [](const RayVector& rv, const Vector3d& r, double t){
            return phaseMany(rv.rays, r, t);
        });
        m.def("propagatedToTimesMany", [](const RayVector &rv, const std::vector<double>& t){
            RayVector result;
            result.rays = std::move(propagatedToTimesMany(rv.rays, t));
            return result;
        });
        m.def("propagateInPlaceMany", [](RayVector& rv, const std::vector<double>& t){
            propagateInPlaceMany(rv.rays, t);
        });

        py::class_<RayVector>(m, "RayVector")
            .def(py::init<>())
            .def(py::init<RayVector>())
            .def(py::init<std::vector<Ray>>())
            .def_property_readonly(
                "x",
                [](RayVector& rv) {
                    return py::array_t<double>(
                        py::buffer_info(
                            &rv.rays[0].p0[0],
                            sizeof(double),
                            py::format_descriptor<double>::format(),
                            1,
                            {rv.size()},
                            {sizeof(Ray)}
                        )
                    );
                }
            )
            .def_property_readonly(
                "y",
                [](RayVector& rv) {
                    return py::array_t<double>(
                        py::buffer_info(
                            &rv.rays[0].p0[1],
                            sizeof(double),
                            py::format_descriptor<double>::format(),
                            1,
                            {rv.size()},
                            {sizeof(Ray)}
                        )
                    );
                }
            )
            .def_property_readonly(
                "z",
                [](RayVector& rv) {
                    return py::array_t<double>(
                        py::buffer_info(
                            &rv.rays[0].p0[2],
                            sizeof(double),
                            py::format_descriptor<double>::format(),
                            1,
                            {rv.size()},
                            {sizeof(Ray)}
                        )
                    );
                }
            )
            .def_property_readonly(
                "vx",
                [](RayVector& rv) {
                    return py::array_t<double>(
                        py::buffer_info(
                            &rv.rays[0].v[0],
                            sizeof(double),
                            py::format_descriptor<double>::format(),
                            1,
                            {rv.size()},
                            {sizeof(Ray)}
                        )
                    );
                }
            )
            .def_property_readonly(
                "vy",
                [](RayVector& rv) {
                    return py::array_t<double>(
                        py::buffer_info(
                            &rv.rays[0].v[1],
                            sizeof(double),
                            py::format_descriptor<double>::format(),
                            1,
                            {rv.size()},
                            {sizeof(Ray)}
                        )
                    );
                }
            )
            .def_property_readonly(
                "vz",
                [](RayVector& rv) {
                    return py::array_t<double>(
                        py::buffer_info(
                            &rv.rays[0].v[2],
                            sizeof(double),
                            py::format_descriptor<double>::format(),
                            1,
                            {rv.size()},
                            {sizeof(Ray)}
                        )
                    );
                }
            )
            .def_property_readonly(
                "t0",
                [](RayVector& rv) {
                    return py::array_t<double>(
                        py::buffer_info(
                            &rv.rays[0].t0,
                            sizeof(double),
                            py::format_descriptor<double>::format(),
                            1,
                            {rv.size()},
                            {sizeof(Ray)}
                        )
                    );
                }
            )
            .def_property_readonly(
                "wavelength",
                [](RayVector& rv) {
                    return py::array_t<double>(
                        py::buffer_info(
                            &rv.rays[0].wavelength,
                            sizeof(double),
                            py::format_descriptor<double>::format(),
                            1,
                            {rv.size()},
                            {sizeof(Ray)}
                        )
                    );
                }
            )
            .def_property_readonly(
                "isVignetted",
                [](RayVector& rv) {
                    return py::array_t<bool>(
                        py::buffer_info(
                            &rv.rays[0].isVignetted,
                            sizeof(bool),
                            py::format_descriptor<bool>::format(),
                            1,
                            {rv.size()},
                            {sizeof(Ray)}
                        )
                    );
                }
            )
            .def_property_readonly(
                "failed",
                [](RayVector& rv) {
                    return py::array_t<bool>(
                        py::buffer_info(
                            &rv.rays[0].failed,
                            sizeof(bool),
                            py::format_descriptor<bool>::format(),
                            1,
                            {rv.size()},
                            {sizeof(Ray)}
                        )
                    );
                }
            )

            .def("__getitem__",
                [](RayVector &rv, typename std::vector<Ray>::size_type i) -> Ray& {
                    if (i >= rv.size())
                        throw py::index_error();
                    return rv.rays[i];
                },
                py::return_value_policy::reference_internal
            )
            .def("__iter__",
                [](RayVector &rv) {
                    return py::make_iterator<
                        py::return_value_policy::reference_internal,
                        typename std::vector<Ray>::iterator,
                        typename std::vector<Ray>::iterator,
                        Ray&>(rv.rays.begin(), rv.rays.end());
                },
                py::keep_alive<0, 1>()
            )
            .def("append", [](RayVector& rv, const Ray& r) { rv.rays.push_back(r); })
            .def("__len__", &RayVector::size)
            .def("__eq__", [](const RayVector& lhs, const RayVector& rhs) { return lhs.rays == rhs.rays; }, py::is_operator())
            .def("__ne__", [](const RayVector& lhs, const RayVector& rhs) { return lhs.rays != rhs.rays; }, py::is_operator());
    }
}
