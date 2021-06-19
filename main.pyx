import cython
#game.hpp の関数や変数など
cdef extern from "game.hpp":
  cdef int height
  cdef int width
  cdef int size
  cdef int started
  cdef int get_field_val(int y, int x)
  cdef int update(double time, int r)
  cdef void start()

cdef extern from "game.hpp" namespace "python":
  cdef int get_posy(int i)
  cdef int get_posx(int i)
  cdef int get_rot(int i)
  cdef int get_state(int i)
  cdef int get_is_stop(int i)
  cdef double get_limit_time(int i)
  cdef int get_eat_num()

#同じ名前だとコンパイルエラーが出るため

h = height
w = width
sizec = size #cppでの1blockの大きさ
is_started = started

def get_field(y: int, x: int):
  return get_field_val(y, x)

def update_pos(time: double, r):
  return update(time, r)

def start_move():
  start()

def get_xyrs(i: int):
  return get_posx(i), get_posy(i), get_rot(i), get_state(i)

def get_isstop(i: int):
  return get_is_stop(i)

def eat_num():
  return get_eat_num()

def limit_time(i: int):
  return get_limit_time(i)
