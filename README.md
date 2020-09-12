# GSSplineTest

### 操作
* UE4打开工程以后，打开PathEditLevel场景，或者新建场景。
* 从Modes按钮选择SplinePathEditorEdMode界面。
* 点击面板中的Add Path按钮，在Level内新建一个“SplinePathActor” Actor，并自动选中
* 在选择SplinePathActor的情况下，Ctrl+鼠标左键点击场景，会在坐标平面上生成预备路径点。
* 按回车键，由上述备选点生成曲线
* 再次Ctrl+鼠标左键，添加新的备选点，但是不会立即生成曲线，再次按下回车，则后续添加的备选点转正
* 点选黄色控制点或者绿色位置点，可以调整曲线。
* 红色为当前选中的点。当选中控制点时，该控制点所在的位置节点也被标红。
* 按L键切换曲线闭合，
* 按D键移除当前选中的位置节点，选中控制点按D键无效。
* 可以添加多条曲线Actor，SplinePathEditorEdMode面板中的AddPath按钮
* 可以删除曲线Actor，SplinePathEditorEdMode面板中的RemovePath按钮
* Add Ramdom Point按钮为完成点选添加之前测试用。
* Make Spline按钮功能和回车键一致，前期测试用。
* 勾选Show All Path，显示全部的曲线路径，不管是否选中Actor
* 勾选Unite Control Point，一个路径节点上的2个控制点可以联动，保证曲线顺滑



### 已知问题：
* 不能保存！！可能是内存管理或者序列化问题，造成会编辑器崩溃，待深入研究。
* 对节点等操作都需要选中SplinePathActor，但是点击场景空白处后会选择其他Actor，可能添加锁定选择功能会好点。
