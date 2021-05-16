import cython
#game.hpp の関数や変数など
cdef extern from "game.hpp":
  cdef int height
  cdef int width
  cdef int get_field_val(int y, int x)

#同じ名前だとコンパイルエラーが出るため
#Pythonで使用する時には末尾に_pをつけることにする

h = height
w = width

def get_field(int y, int x):
  return get_field_val(y, x)

