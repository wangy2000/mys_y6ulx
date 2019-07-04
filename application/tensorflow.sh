#!/usr/bin/python
#coding=utf8
import tensorflow as tf
sess = tf.Session();
hello = tf.constant('Hello, 你奶奶个腿  TensorFlow!')
print sess.run(hello)
a = tf.constant(10000000001.1233)
b = tf.constant(32.2341)
print sess.run(a * b)
sess.close()
