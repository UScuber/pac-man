import cython
#game.hpp の関数や変数など
cdef extern from "game.hpp":
  pass

#同じ名前だとコンパイルエラーが出るため
#Pythonで使用する時には末尾に_pをつけることにする
