import unittest
from unittest.util import safe_repr
from parameterized import parameterized

from alea.sim.kernel.frames import Universe, FrameTransformation, ReferenceFrame
from alea.sim.math_lib import Quaternion
import numpy as np

class FramesTest(unittest.TestCase):

    def test_tf_identity(self):
        # Identity transformation should yield the identity matrix
        tf_identity = FrameTransformation.identity()
        np.testing.assert_array_almost_equal(tf_identity.transformation_matrix(), np.eye(4))

    def test_tf_inverse(self):
        # Multiplying a transform with its inverse should yield the identity matrix
        q0 = Quaternion.from_axang(np.array([1, 0, 0]), np.pi/4)
        t0 = np.array([1, 2, 3])
        tf = FrameTransformation(q0, t0)
        tf_product = tf.transformation_matrix() @ tf.inv_transformation_matrix()
        np.testing.assert_array_almost_equal(tf_product, np.eye(4))

    def test_rotate_vector_roundtrip(self):
        # A vector rotated by the transform and then inverse-rotated should return to the original.
        q0 = Quaternion.from_axang(np.array([0, 1, 0]), np.pi/3)
        t0 = np.array([1, 2, 3])
        tf = FrameTransformation(q0, t0)
        vec = np.array([4, 5, 6])
        vec_rotated = tf.rotate_vector(vec)
        vec_back = tf.inverse_rotate_vector(vec_rotated)
        np.testing.assert_array_almost_equal(vec, vec_back)

    def test_rotate_vector_known(self):
        # For a 90° rotation about the x-axis and no translation,
        # the vector [0, 1, 0] should map to [0, 0, 1]
        q0 = Quaternion.from_axang(np.array([1, 0, 0]), np.pi/2)
        t0 = np.zeros(3)
        tf = FrameTransformation(q0, t0)
        vec = np.array([0, 1, 0])
        expected = np.array([0, 0, 1])
        np.testing.assert_array_almost_equal(tf.rotate_vector(vec), expected)

    @parameterized.expand([
        # Each test: (rotation_axis, angle in radians, translation vector, test_vector, expected_vector after transform)
        (np.array([0, 0, 1]), np.pi/2, [0, 0, 0], [1, 0, 0], [0, 1, 0]),
        (np.array([0, 1, 0]), np.pi, [0, 0, 0], [1, 0, 0], [-1, 0, 0]),
        (np.array([1, 0, 0]), np.pi/2, [1, 1, 1], [0, 1, 0], [1, 1, 2])
    ])
    def test_parameterized_transform(self, axis, angle, translation, vector, expected):
        # Create a transformation from axis-angle rotation and given translation,
        # then test if the transformed vector matches the expected output.
        q = Quaternion.from_axang(np.array(axis), angle)
        tf = FrameTransformation(q, np.array(translation))
        transformed = tf.transform_coordinate(np.array(vector))
        np.testing.assert_array_almost_equal(transformed, np.array(expected), decimal=5)

    def test_tf_mat_1(self):
        # Check that vector forward and inverse rotation works
        q0 = Quaternion.from_axang(np.array([1,0,0]), 0)
        t0 = np.zeros(3)
        tf = FrameTransformation(q0, t0)
        
        print('tf_mat\n',tf.transformation_matrix())

        np.testing.assert_array_almost_equal(tf.transformation_matrix(), np.eye(4))
    
    def test_tf_mat_2(self):
        # Check that vector forward and inverse rotation works
        q0 = Quaternion.from_axang(np.array([1,0,0]), 0)
        t0 = np.zeros(3)
        tf = FrameTransformation(q0, t0)

        # This should be the identity matrix
        tfmat = tf.transformation_matrix() @ tf.inv_transformation_matrix()
        
        np.testing.assert_array_almost_equal(tfmat, np.eye(4))

    def test_rotate_vector_1(self):
        #check that vector forward and inverse rotation works
        q0 = Quaternion.from_axang(np.array([1,0,0]), np.pi/3)
        t0 = np.array([1,2,3])
        tf = FrameTransformation(q0, t0)
        
        print('tf_mat\n',tf.transformation_matrix())
        
        vec = np.array([1,2,3])
        vec2 = tf.inverse_rotate_vector(tf.rotate_vector(vec))

        np.testing.assert_array_almost_equal(vec,vec2)

    def test_rotate_vector_2(self):
        #check that vector forward and inverse rotation works
        q0 = Quaternion.from_axang(np.array([1,0,0]), np.pi/2)
        t0 = np.zeros(3)
        tf = FrameTransformation(q0, t0)
        
        print('tf_mat\n',tf.transformation_matrix())
        
        vec = np.array([0,1,0])

        np.testing.assert_array_almost_equal(tf.rotate_vector(vec), np.array([0,0,1]))

    def test_frames_1(self):

        universe = Universe(Quaternion)
        ref = universe.create_frame("ref")
        body = universe.create_frame("body", derived_from=ref)
        sensor = universe.create_frame("sensor", derived_from=body)

        #set body frame initial transformation relative to ref
        q0 = Quaternion.from_axang(np.array([1,0,0]), np.pi/2)
        t0 = np.zeros(3)
        tf = FrameTransformation(q0, t0)
        universe.get_frame("body").derive_from(ref, tf)

        #set sensor frame initial transformation relative to body, no difference
        tf = FrameTransformation.from_tf_mat(np.eye(4))
        universe.get_frame("sensor").derive_from(body, tf)
        
        #check the sensor transformation updated
        vec = ref.transform_vector_from_frame(np.array([0,1,0]), body)

        print(vec)
        np.testing.assert_array_almost_equal(tf.rotate_vector(vec), np.array([0,0,-1]))
        
        universe.get_frame("sensor").rotate_by_axang(np.array([0,0,1]), np.pi/2, body)
        vec = ref.transform_vector_from_frame(np.array([0,1,0]), body)

        res = np.zeros((4,4))
        res[0,0] = 1
        res[2,1] = 1
        res[1,2] = -1
        res[3,3] = 1
        np.testing.assert_array_almost_equal(ref.get_transformation_to(body).tf, res)

    def test_transform_tree_lookup(self):
        """test case to verify that the frame tree search works as intended"""
        universe = Universe(Quaternion)
        r1 = universe.create_frame('r1')
        r2 = universe.create_frame('r2', r1, FrameTransformation.random())
        r3 = universe.create_frame('r3', r2, FrameTransformation.random())
        r4 = universe.create_frame('r4', r2, FrameTransformation.random())
        r5 = universe.create_frame('r5', r1, FrameTransformation.random())
        r6 = universe.create_frame('r6', r5, FrameTransformation.random())
        tf12 = r1._relations['r2']
        tf15 = r1._relations['r5']
        tf51 = r5._relations['r1']
        tf23 = r2._relations['r3']
        tf24 = r2._relations['r4']
        tf56 = r5._relations['r6']
        tf65 = r6._relations['r5']
        
        np.testing.assert_array_almost_equal(tf12.tf, r1.get_transformation_to(r2).tf)
        np.testing.assert_array_almost_equal((tf56 @ tf15).tf, r1.get_transformation_to(r6).tf)
        np.testing.assert_array_almost_equal((tf24 @ tf12 @ tf51 @ tf65).tf, r6.get_transformation_to(r4).tf)
        
        r2.derive_from(r1, FrameTransformation.random(translation_max_magnitude=int(1e6)))
        r5.derive_from(r1, FrameTransformation.random(translation_max_magnitude=int(1e2)))

        tf12 = r1._relations['r2']
        tf21 = r2._relations['r1']
        tf15 = r1._relations['r5']
        tf51 = r5._relations['r1']
        tf23 = r2._relations['r3']
        tf32 = r3._relations['r2']
        tf56 = r5._relations['r6']
        tf65 = r6._relations['r5']
        
        np.testing.assert_array_almost_equal(tf12.tf, r1.get_transformation_to(r2).tf)
        np.testing.assert_array_almost_equal((tf56 @ tf15).tf, r1.get_transformation_to(r6).tf)
        np.testing.assert_array_almost_equal((tf23 @ tf12 @ tf51 @ tf65).tf, r6.get_transformation_to(r3).tf)
        np.testing.assert_array_almost_equal((tf15 @ tf21 @ tf32).tf, r3.get_transformation_to(r5).tf)

if __name__ == '__main__':
    unittest.main()