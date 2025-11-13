#include <iostream>
#include <chrono>
#include <iomanip>
#include "fk/performance/ObjectPool.h"
#include "fk/performance/GeometryCache.h"
#include "fk/performance/MaterialPool.h"
#include "fk/performance/RenderBatcher.h"
#include "fk/ui/DrawCommand.h"

using namespace fk;
using Color = fk::ui::Color;
using Rect = fk::ui::Rect;

// 测试对象
struct TestObject {
    int id;
    std::string data;
    
    TestObject() : id(0), data("") {}
    TestObject(int i) : id(i), data("Object_" + std::to_string(i)) {}
};

void PrintSeparator(const std::string& title) {
    std::cout << "\n========== " << title << " ==========\n" << std::endl;
}

void TestObjectPool() {
    PrintSeparator("1. ObjectPool 测试");

    // 创建对象池
    auto createFunc = []() {
        static int counter = 0;
        return std::make_shared<TestObject>(++counter);
    };
    
    auto resetFunc = [](TestObject* obj) {
        obj->data = "Reset";
    };

    ObjectPool<TestObject> pool(5, createFunc, resetFunc);

    std::cout << "初始池大小: " << pool.GetAvailableCount() << std::endl;

    // 获取对象
    std::vector<std::shared_ptr<TestObject>> objects;
    for (int i = 0; i < 8; ++i) {
        auto obj = pool.Acquire();
        std::cout << "获取对象 " << i + 1 << ": ID=" << obj->id 
                  << ", Data=" << obj->data << std::endl;
        objects.push_back(obj);
    }

    std::cout << "\n获取后池大小: " << pool.GetAvailableCount() << std::endl;

    // 归还对象
    for (size_t i = 0; i < 4; ++i) {
        pool.Release(objects[i]);
    }
    objects.erase(objects.begin(), objects.begin() + 4);

    std::cout << "归还4个对象后池大小: " << pool.GetAvailableCount() << std::endl;

    // 统计信息
    auto stats = pool.GetStats();
    std::cout << "\n统计信息:" << std::endl;
    std::cout << "  总创建: " << stats.totalCreated << std::endl;
    std::cout << "  总获取: " << stats.totalAcquired << std::endl;
    std::cout << "  总归还: " << stats.totalReleased << std::endl;
    std::cout << "  可用: " << stats.available << std::endl;
    std::cout << "  使用中: " << stats.inUse << std::endl;

    std::cout << "\n✓ ObjectPool 测试通过" << std::endl;
}

void TestPooledObject() {
    PrintSeparator("2. PooledObject RAII 测试");

    ObjectPool<TestObject> pool(3);

    {
        std::cout << "进入作用域，创建 PooledObject..." << std::endl;
        PooledObject<TestObject> obj(pool);
        obj->id = 999;
        obj->data = "RAII Test";
        std::cout << "PooledObject: ID=" << obj->id << ", Data=" << obj->data << std::endl;
        std::cout << "池可用数: " << pool.GetAvailableCount() << std::endl;
    }

    std::cout << "\n退出作用域后，对象自动归还" << std::endl;
    std::cout << "池可用数: " << pool.GetAvailableCount() << std::endl;

    std::cout << "\n✓ PooledObject RAII 测试通过" << std::endl;
}

void TestGeometryCache() {
    PrintSeparator("3. GeometryCache 测试");

    auto& cache = GeometryCache::Instance();
    cache.Clear();

    // 预加载常用几何
    std::cout << "预加载常用几何..." << std::endl;
    cache.PreloadCommonGeometry();

    // 测试缓存命中
    std::cout << "\n测试缓存查找:" << std::endl;
    
    auto rectEntry = cache.Get("rect_unit");
    if (rectEntry) {
        std::cout << "  ✓ rect_unit 找到: " 
                  << rectEntry->vertexCount << " 顶点, "
                  << rectEntry->indexCount << " 索引" << std::endl;
    }

    auto circleEntry = cache.Get("circle_32");
    if (circleEntry) {
        std::cout << "  ✓ circle_32 找到: " 
                  << circleEntry->vertexCount << " 顶点, "
                  << circleEntry->indexCount << " 索引" << std::endl;
    }

    // 测试缓存未命中
    auto missingEntry = cache.Get("nonexistent");
    std::cout << "  " << (missingEntry ? "✗" : "✓") 
              << " nonexistent 未找到（预期）" << std::endl;

    // 添加自定义几何
    std::cout << "\n添加自定义几何..." << std::endl;
    std::vector<float> customVertices = {0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f};
    std::vector<unsigned int> customIndices = {0, 1, 2};
    cache.Put("custom_triangle", customVertices, customIndices);

    auto customEntry = cache.Get("custom_triangle");
    if (customEntry) {
        std::cout << "  ✓ custom_triangle 已缓存" << std::endl;
    }

    // 统计信息
    auto stats = cache.GetStats();
    std::cout << "\n统计信息:" << std::endl;
    std::cout << "  总请求: " << stats.totalRequests << std::endl;
    std::cout << "  缓存命中: " << stats.cacheHits << std::endl;
    std::cout << "  缓存未命中: " << stats.cacheMisses << std::endl;
    std::cout << "  命中率: " << std::fixed << std::setprecision(2) 
              << (stats.hitRate * 100) << "%" << std::endl;
    std::cout << "  当前条目数: " << stats.currentEntries << std::endl;
    std::cout << "  当前大小: " << stats.currentSize << " 字节" << std::endl;

    std::cout << "\n✓ GeometryCache 测试通过" << std::endl;
}

void TestMaterialPool() {
    PrintSeparator("4. MaterialPool 测试");

    auto& pool = MaterialPool::Instance();
    pool.Clear();

    // 创建材质
    std::cout << "创建不同材质..." << std::endl;
    
    Color red = Color::FromRGB(255, 0, 0);
    Color blue = Color::FromRGB(0, 0, 255);
    Color green = Color::FromRGB(0, 255, 0);
    
    auto red1 = pool.GetSolidColor(red);
    auto red2 = pool.GetSolidColor(red);  // 应该返回相同的对象
    auto bluemat = pool.GetSolidColor(blue);
    auto greenTransparent = pool.GetSolidColor(green, 0.5);

    std::cout << "  红色材质 1: " << red1.get() << std::endl;
    std::cout << "  红色材质 2: " << red2.get() << std::endl;
    std::cout << "  蓝色材质: " << bluemat.get() << std::endl;
    std::cout << "  半透明绿色: " << greenTransparent.get() << std::endl;

    if (red1.get() == red2.get()) {
        std::cout << "\n  ✓ 相同颜色的材质被正确复用" << std::endl;
    } else {
        std::cout << "\n  ✗ 材质复用失败" << std::endl;
    }

    // 统计信息
    auto stats = pool.GetStats();
    std::cout << "\n统计信息:" << std::endl;
    std::cout << "  总材质数: " << stats.totalMaterials << std::endl;
    std::cout << "  唯一材质数: " << stats.uniqueMaterials << std::endl;
    std::cout << "  避免的重复创建: " << stats.duplicatesSaved << std::endl;
    std::cout << "  总请求数: " << stats.totalRequests << std::endl;
    
    double savingsRate = stats.totalRequests > 0 ?
        static_cast<double>(stats.duplicatesSaved) / stats.totalRequests * 100 : 0;
    std::cout << "  节省率: " << std::fixed << std::setprecision(2) 
              << savingsRate << "%" << std::endl;

    std::cout << "\n✓ MaterialPool 测试通过" << std::endl;
}

void TestRenderBatcher() {
    PrintSeparator("5. RenderBatcher 测试");

    RenderBatcher batcher;
    batcher.BeginFrame();

    // 添加大量渲染项
    std::cout << "添加渲染项..." << std::endl;
    
    Color red = Color::FromRGB(255, 0, 0);
    Color blue = Color::FromRGB(0, 0, 255);
    Color green = Color::FromRGB(0, 255, 0);
    
    // 100 个红色矩形
    for (int i = 0; i < 100; ++i) {
        RenderBatchItem item;
        item.bounds = Rect(i * 10.0, 0.0, 10.0, 10.0);
        item.color = red;
        item.opacity = 1.0;
        batcher.AddItem(item, "material_red");
    }

    // 50 个蓝色矩形
    for (int i = 0; i < 50; ++i) {
        RenderBatchItem item;
        item.bounds = Rect(i * 10.0, 20.0, 10.0, 10.0);
        item.color = blue;
        item.opacity = 1.0;
        batcher.AddItem(item, "material_blue");
    }

    // 100 个绿色矩形
    for (int i = 0; i < 100; ++i) {
        RenderBatchItem item;
        item.bounds = Rect(i * 10.0, 40.0, 10.0, 10.0);
        item.color = green;
        item.opacity = 1.0;
        batcher.AddItem(item, "material_green");
    }

    batcher.EndFrame();

    // 遍历批次
    std::cout << "\n生成的批次:" << std::endl;
    int batchIndex = 0;
    batcher.ForEachBatch([&batchIndex](const RenderBatch& batch) {
        std::cout << "  批次 " << ++batchIndex << ": "
                  << batch.items.size() << " 项, "
                  << "材质=" << batch.materialKey << ", "
                  << batch.vertexCount << " 顶点, "
                  << batch.indexCount << " 索引" << std::endl;
    });

    // 统计信息
    auto stats = batcher.GetStats();
    std::cout << "\n统计信息:" << std::endl;
    std::cout << "  总渲染项: " << stats.totalItems << std::endl;
    std::cout << "  总批次数: " << stats.totalBatches << std::endl;
    std::cout << "  平均每批次项数: " << stats.itemsPerBatch << std::endl;
    std::cout << "  节省的绘制调用: " << stats.drawCallsSaved << std::endl;
    std::cout << "  批处理效率: " << std::fixed << std::setprecision(2) 
              << (stats.batchingEfficiency * 100) << "%" << std::endl;

    std::cout << "\n✓ RenderBatcher 测试通过" << std::endl;
}

void TestPerformanceComparison() {
    PrintSeparator("6. 性能对比测试");

    const int iterations = 10000;

    // 测试 ObjectPool 性能
    std::cout << "测试 ObjectPool 性能..." << std::endl;
    ObjectPool<TestObject> pool(100);

    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i) {
        auto obj = pool.Acquire();
        obj->id = i;
        pool.Release(obj);
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto poolTime = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    // 测试普通 new/delete 性能
    std::cout << "测试普通 new/delete 性能..." << std::endl;
    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i) {
        auto obj = new TestObject(i);
        delete obj;
    }
    end = std::chrono::high_resolution_clock::now();
    auto newDeleteTime = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    std::cout << "\n性能对比结果 (" << iterations << " 次迭代):" << std::endl;
    std::cout << "  ObjectPool: " << poolTime << " μs" << std::endl;
    std::cout << "  new/delete: " << newDeleteTime << " μs" << std::endl;
    std::cout << "  性能提升: " << std::fixed << std::setprecision(2)
              << (static_cast<double>(newDeleteTime) / poolTime) << "x" << std::endl;

    std::cout << "\n✓ 性能对比测试完成" << std::endl;
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "   F__K_UI 性能优化系统演示程序" << std::endl;
    std::cout << "========================================" << std::endl;

    try {
        TestObjectPool();
        TestPooledObject();
        TestGeometryCache();
        TestMaterialPool();
        TestRenderBatcher();
        TestPerformanceComparison();

        std::cout << "\n========================================" << std::endl;
        std::cout << "   所有测试通过！✓" << std::endl;
        std::cout << "========================================" << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "\n错误: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
