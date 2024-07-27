# Hive-chess

- [x] 回合制部分;
- [x] 棋子可行移动;
    - [x] 蚱蜢;
    - [x] 蜘蛛;
    - [x] 蚂蚁;
- [x] 渲染函数需要改变;
    - 目前的问题是: 当两个棋子位于 $(0, 0)$ 和 $(29, 0)$ 时, 计算的重心会在 $(15, 0)$ 附近, 导致两个棋子没有挨起来;
    - 解决方案: 用 `bfs` 的方法获取所有在棋盘上的棋子, 并用起点棋子的相对位置来计算重心. 
- [x] 问题: 当萨姆走到棋子(如花火)身上后, 点击萨姆会出来花火. 修改方法: 在 `mouseEvent` 中, 查询点击了哪个棋子时, 先去看萨姆.
- [ ] 问题: 蜘蛛的寻路目前是"找到所有和当前位置距离为 3 的点", 但是可能存在从另一个方向走, 距离更长但是刚好是三步的点.
- [ ] 问题: 棋子重心偏移. 解决方案: 将重心变成 float 类型.

## 更新记录

- 7.26: 
- - 参考 Meisenheimer 的建议, 把计算渲染位置的代码移动到了 `mouseEvent()` 中;
- - 完成了所有棋子 `getPossibleDest` 的实现, 明确了蜘蛛行走的规则(不能在移动中分开).
- - 基本完成了全部内容, 进入测试环节.

- 7.27:
- - 补充规则: 当蜂后没有被放到场上的时候, 不能移动棋子.