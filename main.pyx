import cython
#game.hpp の関数や変数など
cdef extern from "game.hpp":
  cdef int f_height
  cdef int f_width
  cdef int size
  cdef int get_field_val(int y, int x)
  cdef int update(double time, int r)

cdef extern from "game.hpp" namespace "python":
  cdef int get_posy(int i)
  cdef int get_posx(int i)
  cdef int get_rot(int i)
  cdef int get_state(int i)
  cdef int get_is_stop(int i)
  cdef int get_is_limit(int i)
  cdef int get_eat_num()

#同じ名前だとコンパイルエラーが出るため
#Pythonで使用する時には末尾に_pをつけることにする

h = f_height
w = f_width
sizec = size #cppでの1blockの大きさ

def get_field(y: int, x: int):
  return get_field_val(y, x)

def update_pos(time: double, r):
  return update(time, r)

def get_xyrs(i: int):
  return get_posx(i), get_posy(i), get_rot(i), get_state(i)

def get_isstop(i: int):
  return get_is_stop(i)

def eat_num():
  return get_eat_num()

def is_limit(i: int):
  return get_is_limit(i)
