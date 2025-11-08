# RenderList - 渲染列表

## 概述

`RenderList` 是优化过的渲染命令集合，由 RenderScene 生成，供渲染器执行。它可能包含批处理、深度排序等优化。

## 主要用途

- 承载优化后的命令
- 提供渲染器友好的接口
- 支持多种渲染优化技术

## 相关文档

- [RenderScene.md](./RenderScene.md) - 渲染场景
- [IRenderer.md](./IRenderer.md) - 渲染器接口
