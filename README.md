# FK_UI Minimal Framework

这是一个最小的 C++23 UI 框架骨架示例，包含 `Application` 和 `Window` 两个类的基础实现以及一个演示程序。

构建（在 Windows PowerShell 中）:

```powershell
mkdir build; cd build; cmake ..; cmake --build . --config Release
```

运行示例（Release 下）:

```powershell
.\Release\fk_example.exe
```

说明：这是一个非常简化的示例，主循环是模拟的。真实框架需要加入平台窗口代码、消息泵和渲染循环等。
