import batoid
import numpy as np
from test_helpers import timer, do_pickle, all_obj_diff


@timer
def test_sag():
    rng = np.random.default_rng(5)
    for i in range(100):
        plane = batoid.Plane()
        for j in range(10):
            x = rng.normal()
            y = rng.normal()
            result = plane.sag(x, y)
            np.testing.assert_equal(result, 0.0)
            # Check that it returned a scalar float and not an array
            assert isinstance(result, float)
        # Check vectorization
        x = rng.normal(size=(10, 10))
        y = rng.normal(size=(10, 10))
        np.testing.assert_allclose(plane.sag(x, y), 0.0)
        # Make sure non-unit stride arrays also work
        np.testing.assert_allclose(plane.sag(x[::5,::2], y[::5,::2]), 0.0)
        do_pickle(plane)


@timer
def test_normal():
    rng = np.random.default_rng(57)
    for i in range(100):
        plane = batoid.Plane()
        for j in range(10):
            x = rng.normal()
            y = rng.normal()
            result = plane.normal(x, y)
            np.testing.assert_equal(result, np.array([0., 0., 1.]))
        # Check vectorization
        x = rng.normal(size=(10, 10))
        y = rng.normal(size=(10, 10))
        np.testing.assert_allclose(
            plane.normal(x, y),
            np.broadcast_to(np.array([0., 0., 1.]), (10, 10, 3))
        )
        # Make sure non-unit stride arrays also work
        np.testing.assert_allclose(
            plane.normal(x[::5,::2], y[::5,::2]),
            np.broadcast_to(np.array([0., 0., 1.]), (2, 5, 3))
        )


@timer
def test_intersect():
    rng = np.random.default_rng(577)
    size = 10
    plane = batoid.Plane()
    coordSys = batoid.CoordSys(origin=[0, 0, -1])
    x = rng.normal(0.0, 1.0, size=size)
    y = rng.normal(0.0, 1.0, size=size)
    z = np.full_like(x, -100.0)
    # If we shoot rays straight up, then it's easy to predict the
    vx = np.zeros_like(x)
    vy = np.zeros_like(x)
    vz = np.ones_like(x)
    rv = batoid.RayVector(x, y, z, vx, vy, vz)
    np.testing.assert_allclose(rv.z, -100.0)
    rv2 = plane.intersect(rv.copy(), coordSys)
    assert rv2.coordSys == coordSys
    rv2 = rv2.toCoordSys(batoid.CoordSys())
    np.testing.assert_allclose(rv2.x, x)
    np.testing.assert_allclose(rv2.y, y)
    np.testing.assert_allclose(rv2.z, -1, rtol=0, atol=1e-12)


# @timer
# def test_ne():
#     objs = [
#         batoid.Plane(),
#         batoid.Plane(allowReverse=True),
#         batoid.Paraboloid(2.0),
#     ]
#     all_obj_diff(objs)
#
#
# @timer
# def test_fail():
#     plane = batoid.Plane()
#     assert plane.allowReverse == False
#     ray = batoid.Ray([0,0,-1], [0,0,-1])
#     ray = plane.intersect(ray)
#     assert ray.failed
#
#     ray = batoid.Ray([0,0,-1], [0,0,-1])
#     plane.intersect(ray)
#     assert ray.failed
#
#     # These should succeed though if allowReverse is True
#     plane = batoid.Plane(allowReverse=True)
#     assert plane.allowReverse == True
#     ray = batoid.Ray([0,0,-1], [0,0,-1])
#     ray = plane.intersect(ray)
#     assert not ray.failed
#
#     ray = batoid.Ray([0,0,-1], [0,0,-1])
#     plane.intersect(ray)
#     assert not ray.failed


if __name__ == '__main__':
    test_sag()
    test_normal()
    test_intersect()
    # test_intersect_vectorized()
    # test_ne()
    # test_fail()
