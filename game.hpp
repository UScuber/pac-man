#include <iostream>


constexpr int height = 22;
constexpr int width = 19;
enum{
  none, wall, //黒、壁
  pac, //Pac-Man
  red, blue, orange, pink //enemies
};
//フィールドの初期状態
int field[height][width] = {
  {wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall},
  {wall,none,none,none,none,none,none,none,none,wall,none,none,none,none,none,none,none,none,wall},
  {wall,none,wall,wall,none,wall,wall,wall,none,wall,none,wall,wall,wall,none,wall,wall,none,wall},
  {wall,none,wall,wall,none,wall,wall,wall,none,wall,none,wall,wall,wall,none,wall,wall,none,wall},
  {wall,none,none,none,none,none,none,none,none,none,none,none,none,none,none,none,none,none,wall},
  {wall,none,wall,wall,none,wall,none,wall,wall,wall,wall,wall,none,wall,none,wall,wall,none,wall},
  {wall,none,none,none,none,wall,none,none,none,wall,none,none,none,wall,none,none,none,none,wall},
  {wall,wall,wall,wall,none,wall,wall,wall,none,wall,none,wall,wall,wall,none,wall,wall,wall,wall},
  {wall,wall,wall,wall,none,wall,none,none,none,none,none,none,none,wall,none,wall,wall,wall,wall},
  {wall,wall,wall,wall,none,wall,none,wall,wall,pink,wall,wall,none,wall,none,wall,wall,wall,wall},
  {none,none,none,none,none,none,none,wall,blue,orange,red,wall,none,none,none,none,none,none,none},
  {wall,wall,wall,wall,none,wall,none,wall,wall,wall,wall,wall,none,wall,none,wall,wall,wall,wall},
  {wall,wall,wall,wall,none,wall,none,none,none,none,none,none,none,wall,none,wall,wall,wall,wall},
  {wall,wall,wall,wall,none,wall,none,wall,wall,wall,wall,wall,none,wall,none,wall,wall,wall,wall},
  {wall,none,none,none,none,none,none,none,none,wall,none,none,none,none,none,none,none,none,wall},
  {wall,none,wall,wall,none,wall,wall,wall,none,wall,none,wall,wall,wall,none,wall,wall,none,wall},
  {wall,none,none,wall,none,none,none,none,none,pac ,none,none,none,none,none,wall,none,none,wall},
  {wall,wall,none,wall,none,wall,none,wall,wall,wall,wall,wall,none,wall,none,wall,none,wall,wall},
  {wall,none,none,none,none,wall,none,none,none,wall,none,none,none,wall,none,none,none,none,wall},
  {wall,none,wall,wall,wall,wall,wall,wall,none,wall,none,wall,wall,wall,wall,wall,wall,none,wall},
  {wall,none,none,none,none,none,none,none,none,none,none,none,none,none,none,none,none,none,wall},
  {wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall}
};

//前回の状態からどれだけ動くのかを出力する
//それを受けてpythonで移動の処理をする

int get_field_val(int y, int x){
  return field[y][x];
}