import cython
#game.hpp の関数や変数など
cdef extern from "game.hpp":
  cdef int f_height
  cdef int f_width
  cdef int size

cdef extern from "game.hpp" namespace "python":
  cdef int get_field_val(int y, int x)
  cdef void update()
  cdef int get_move_posy(int i)
  cdef int get_move_posx(int i)

#同じ名前だとコンパイルエラーが出るため
#Pythonで使用する時には末尾に_pをつけることにする

h = f_height
w = f_width
sizec = size #cppでの1blockの大きさ

def get_field(int y, int x):
  return get_field_val(y, x)

def update_pos():
  update()

def get_posy(int i):
  return get_move_posy(i)

def get_posx(int i):
  return get_move_posx(i)
