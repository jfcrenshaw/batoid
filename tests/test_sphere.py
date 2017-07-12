import jtrace
import math
from test_helpers import isclose, timer


@timer
def test_properties():
    import random
    random.seed(5)
    for i in range(100):
        R = random.gauss(0.7, 0.8)
        B = random.gauss(0.8, 1.2)
        sphere = jtrace.Sphere(R, B)
        assert sphere.R == R
        assert sphere.B == B


@timer
def test_sag():
    import random
    random.seed(57)
    for i in range(100):
        R = random.gauss(4.2, 0.3)
        B = random.gauss(0.4, 0.2)
        sphere = jtrace.Sphere(R, B)
        for j in range(10):
            x = random.gauss(0.0, 1.0)
            y = random.gauss(0.0, 1.0)
            assert isclose(sphere.sag(x, y), R*(1-math.sqrt(1.0-(x*x + y*y)/R/R))+B)


@timer
def test_intersect():
    import random
    random.seed(577)
    for i in range(100):
        R = random.gauss(10.0, 0.1)
        B = random.gauss(0.4, 0.2)
        sphere = jtrace.Sphere(R, B)
        for j in range(10):
            x = random.gauss(0.0, 1.0)
            y = random.gauss(0.0, 1.0)

            # If we shoot rays straight up, then it's easy to predict the
            # intersection points.
            r = jtrace.Ray(x, y, -1000, 0, 0, 1, 0)
            isec = sphere.intersect(r)
            assert isclose(isec.point.x, x)
            assert isclose(isec.point.y, y)
            assert isclose(isec.point.z, sphere.sag(x, y), rel_tol=0, abs_tol=1e-9)

            # We can also check just for mutual consistency of the sphere,
            # ray and intersection.

            vx = random.gauss(0.0, 0.1)
            vy = random.gauss(0.0, 0.1)
            vz = 1.0
            v = jtrace.Vec3(vx, vy, vz).UnitVec3()
            r = jtrace.Ray(jtrace.Vec3(x, y, -10), v, 0)
            isec = sphere.intersect(r)
            p1 = r.positionAtTime(isec.t)
            p2 = isec.point
            assert isclose(p1.x, p2.x)
            assert isclose(p1.y, p2.y)
            assert isclose(p1.z, p2.z)
            assert isclose(sphere.sag(p1.x, p2.y), p1.z, rel_tol=0, abs_tol=1e-6)


@timer
def test_intersect_vectorized():
    import random
    random.seed(5772)
    rays = [jtrace.Ray([random.gauss(0.0, 0.1),
                        random.gauss(0.0, 0.1),
                        random.gauss(10.0, 0.1)],
                       [random.gauss(0.0, 0.1),
                        random.gauss(0.0, 0.1),
                        random.gauss(-1.0, 0.1)],
                       random.gauss(0.0, 0.1))
            for i in range(1000)]
    rays = jtrace.RayVector(rays)

    for i in range(100):
        R = random.gauss(0.05, 0.01)
        B = random.gauss(0.4, 0.2)
        sphere = jtrace.Sphere(R, B)
        intersections = sphere.intersect(rays)
        intersections2 = [sphere.intersect(ray) for ray in rays]
        intersections2 = jtrace.IntersectionVector(intersections2)
        assert intersections == intersections2


if __name__ == '__main__':
    test_properties()
    test_sag()
    test_intersect()
    test_intersect_vectorized()
