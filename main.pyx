import cython
#game.hpp の関数や変数など
cdef extern from "game.hpp":
  cdef int f_height
  cdef int f_width
  cdef int size
  cdef int get_field_val(int y, int x)
  cdef int update(double time)

cdef extern from "game.hpp" namespace "python":
  cdef int get_posy(int i)
  cdef int get_posx(int i)
  cdef int get_rot(int i)
  cdef void turn(int r)

#同じ名前だとコンパイルエラーが出るため
#Pythonで使用する時には末尾に_pをつけることにする

h = f_height
w = f_width
sizec = size #cppでの1blockの大きさ

def get_field(int y, int x):
  return get_field_val(y, x)

def update_pos(double time):
  return update(time)

def get_xyr(i):
  return get_posx(i), get_posy(i), get_rot(i)

def rotate(r):
  turn(r)































