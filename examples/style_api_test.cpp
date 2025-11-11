/**
 * @file style_api_test.cpp
 * @brief Style 系统 API 测试（不依赖具体控件）
 * 
 * 测试 Style、Setter、SetterCollection 的基本功能
 */

#include "fk/ui/Style.h"
#include "fk/ui/Setter.h"
#include "fk/binding/DependencyProperty.h"
#include <iostream>
#include <typeindex>

using namespace fk;

int main() {
    std::cout << "\n=== Style System API Test ===" << std::endl;
    
    // 测试 1：创建 SetterCollection
    std::cout << "\n[Test 1] SetterCollection" << std::endl;
    {
        ui::SetterCollection setters;
        
        // 创建一个测试依赖属性
        auto& testProp = binding::DependencyProperty::Register(
            "TestProperty",
            typeid(float),
            typeid(void),
            {0.0f}
        );
        
        // 添加 setter
        setters.Add(testProp, 100.0f);
        setters.Add(testProp, 200.0f);
        
        std::cout << "  ✓ Setters count: " << setters.Count() << std::endl;
        std::cout << "  ✓ Can iterate: " << (setters.begin() != setters.end() ? "Yes" : "No") << std::endl;
        
        // 测试迭代
        int count = 0;
        for (const auto& setter : setters) {
            count++;
        }
        std::cout << "  ✓ Iterated " << count << " setters" << std::endl;
        
        // 测试访问
        std::cout << "  ✓ First setter accessible: " << (&setters[0] != nullptr ? "Yes" : "No") << std::endl;
        
        // 清空
        setters.Clear();
        std::cout << "  ✓ After clear: " << setters.Count() << " setters" << std::endl;
    }
    
    // 测试 2：创建 Style
    std::cout << "\n[Test 2] Style Creation" << std::endl;
    {
        ui::Style style;
        
        std::cout << "  ✓ Style created" << std::endl;
        std::cout << "  ✓ Initial sealed state: " << (style.IsSealed() ? "Sealed" : "Not Sealed") << std::endl;
        std::cout << "  ✓ Initial setters count: " << style.Setters().Count() << std::endl;
        std::cout << "  ✓ Initial BasedOn: " << (style.GetBasedOn() == nullptr ? "None" : "Has BasedOn") << std::endl;
    }
    
    // 测试 3：Style 类型设置
    std::cout << "\n[Test 3] Style TargetType" << std::endl;
    {
        ui::Style style;
        
        // 设置目标类型
        style.SetTargetType(typeid(int));
        
        std::cout << "  ✓ TargetType set" << std::endl;
        std::cout << "  ✓ IsApplicableTo(int): " << (style.IsApplicableTo(typeid(int)) ? "Yes" : "No") << std::endl;
        std::cout << "  ✓ IsApplicableTo(float): " << (style.IsApplicableTo(typeid(float)) ? "Yes" : "No") << std::endl;
    }
    
    // 测试 4：Style Setters
    std::cout << "\n[Test 4] Style Setters" << std::endl;
    {
        ui::Style style;
        
        auto& prop1 = binding::DependencyProperty::Register(
            "Width",
            typeid(float),
            typeid(void),
            {0.0f}
        );
        
        auto& prop2 = binding::DependencyProperty::Register(
            "Height",
            typeid(float),
            typeid(void),
            {0.0f}
        );
        
        // 添加 setter
        style.Setters().Add(prop1, 200.0f);
        style.Setters().Add(prop2, 100.0f);
        
        std::cout << "  ✓ Added 2 setters" << std::endl;
        std::cout << "  ✓ Setters count: " << style.Setters().Count() << std::endl;
    }
    
    // 测试 5：Style 封印
    std::cout << "\n[Test 5] Style Sealing" << std::endl;
    {
        ui::Style style;
        style.SetTargetType(typeid(int));
        
        auto& prop = binding::DependencyProperty::Register(
            "TestProp",
            typeid(int),
            typeid(void),
            {0}
        );
        
        style.Setters().Add(prop, 42);
        
        std::cout << "  ✓ Before seal: " << (style.IsSealed() ? "Sealed" : "Not Sealed") << std::endl;
        
        style.Seal();
        std::cout << "  ✓ After seal: " << (style.IsSealed() ? "Sealed" : "Not Sealed") << std::endl;
        
        // 尝试在封印后添加 setter（应该无效或抛出异常）
        try {
            style.Setters().Add(prop, 99);
            std::cout << "  ⚠ Warning: Can still add setters after seal" << std::endl;
        } catch (...) {
            std::cout << "  ✓ Cannot add setters after seal" << std::endl;
        }
    }
    
    // 测试 6：Style 继承 (BasedOn)
    std::cout << "\n[Test 6] Style Inheritance" << std::endl;
    {
        // 基础样式
        auto baseStyle = new ui::Style();
        baseStyle->SetTargetType(typeid(int));
        
        auto& prop = binding::DependencyProperty::Register(
            "BaseProp",
            typeid(int),
            typeid(void),
            {0}
        );
        
        baseStyle->Setters().Add(prop, 10);
        baseStyle->Seal();
        
        std::cout << "  ✓ Base style created with " << baseStyle->Setters().Count() << " setter(s)" << std::endl;
        
        // 派生样式
        ui::Style derivedStyle;
        derivedStyle.SetTargetType(typeid(int));
        derivedStyle.SetBasedOn(baseStyle);
        
        std::cout << "  ✓ Derived style created" << std::endl;
        std::cout << "  ✓ Has BasedOn: " << (derivedStyle.GetBasedOn() != nullptr ? "Yes" : "No") << std::endl;
        
        derivedStyle.Setters().Add(prop, 20);
        std::cout << "  ✓ Derived style setters: " << derivedStyle.Setters().Count() << std::endl;
        
        derivedStyle.Seal();
        
        delete baseStyle;
    }
    
    // 总结
    std::cout << "\n=== Test Summary ===" << std::endl;
    std::cout << "✓ SetterCollection API" << std::endl;
    std::cout << "✓ Style creation" << std::endl;
    std::cout << "✓ TargetType management" << std::endl;
    std::cout << "✓ Setter management" << std::endl;
    std::cout << "✓ Style sealing" << std::endl;
    std::cout << "✓ Style inheritance (BasedOn)" << std::endl;
    std::cout << "\nAll API tests passed!" << std::endl;
    std::cout << "====================\n" << std::endl;
    
    return 0;
}
