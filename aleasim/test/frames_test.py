import unittest
from typing import Type, List, Union
import os
import time
import argparse
import sys
import threading
import pathlib
from unittest.util import safe_repr

from aleasim.kernel.frames import Universe, FrameTransformation, ReferenceFrame
from aleasim.math_lib import Quaternion
import numpy as np

class FramesTest(unittest.TestCase):

    def test_tf_mat_1(self):
        #check that vector forward and inverse rotation works
        q0 = Quaternion.from_axang(np.array([1,0,0]), 0)
        t0 = np.zeros(3)
        tf = FrameTransformation(q0, t0)
        
        print('tf_mat\n',tf.transformation_matrix())

        np.testing.assert_array_almost_equal(tf.transformation_matrix(), np.eye(4))
    
    def test_tf_mat_2(self):
        #check that vector forward and inverse rotation works
        q0 = Quaternion.from_axang(np.array([1,0,0]), 0)
        t0 = np.zeros(3)
        tf = FrameTransformation(q0, t0)

        #this should be the identity matrix
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