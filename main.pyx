import cython
#game.hpp の関数や変数など
cdef extern from "game.hpp":
  cdef int height
  cdef int width
  cdef int size

cdef extern from "game.hpp" namespace "Python":
  cdef int update_frame(double time, int r)
  cdef void start_game()
  cdef void stop_game()
  cdef void restart_game()
  cdef void reset_game()
  cdef int get_field_value(int y, int x)
  cdef int get_posy(int i)
  cdef int get_posx(int i)
  cdef int get_rot(int i)
  cdef int get_state(int i)
  cdef int get_is_stop(int i)
  cdef double get_limit_time(int i)
  cdef int get_eat_num()
  cdef int get_remain_num()
  cdef int get_is_game_over()
  cdef int get_is_game_cleared()
  cdef int get_current_score()
  cdef int get_current_mode()

#同じ名前だとコンパイルエラーが出るため

h = height
w = width
sizec = size #cppでの1blockの大きさ

def get_field(y: int, x: int):
  return get_field_value(y, x)

def update_pos(time: double, r: int):
  return update_frame(time, r)

def start_move():
  start_game()

def stop_move():
  stop_game()

def restart():
  restart_game()
def reset():
  reset_game()

def get_xyrs(i: int):
  return get_posx(i), get_posy(i), get_rot(i), get_state(i)

def get_isstop(i: int):
  return get_is_stop(i)

def get_score():
  return get_current_score()

def eat_num():
  return get_eat_num()

def remain_num():
  return get_remain_num()

def limit_time(i: int):
  return get_limit_time(i)

def get_mode():
  return get_current_mode()

def is_game_over():
  return get_is_game_over()

def is_game_cleared():
  return get_is_game_cleared()
