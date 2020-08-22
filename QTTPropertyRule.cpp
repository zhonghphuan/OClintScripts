#include "oclint/AbstractASTVisitorRule.h"
#include "oclint/RuleSet.h"
#include "oclint/RuleConfiguration.h"
#include <iostream>
using namespace std;
using namespace clang;
using namespace oclint;

class QTTPropertyRule : public AbstractASTVisitorRule<QTTPropertyRule>
{
    
private:
    string _threshold;
    
public:
    virtual const string name() const override
    {
        return "修饰规范";
    }

    virtual int priority() const override
    {
        return 3;
    }

    virtual const string category() const override
    {
        return _threshold + "属性";
    }

#ifdef DOCGEN
    virtual const std::string since() const override
    {
        return "0.13.1";
    }

    virtual const std::string description() const override
    {
        return ""; // TODO: fill in the description of the rule.
    }

    virtual const std::string example() const override
    {
        return R"rst(
.. code-block:: cpp

    void example()
    {
        // TODO: modify the example for this rule.
    }
        )rst";
    }

    /* fill in the file name only when it does not match the rule identifier
    virtual const std::string fileName() const override
    {
        return "";
    }
    */

    /* add each threshold's key, description, and its default value to the map.
    virtual const std::map<std::string, std::string> thresholds() const override
    {
        std::map<std::string, std::string> thresholdMapping;
        return thresholdMapping;
    }
    */

    /* add additional document for the rule, like references, notes, etc.
    virtual const std::string additionalDocument() const override
    {
        return "";
    }
    */

    /* uncomment this method when the rule is enabled to be suppressed.
    virtual bool enableSuppress() const override
    {
        return true;
    }
    */
#endif

    virtual void setUp() override {
        _threshold = RuleConfiguration::stringForKey("TEAM_NAME", "趣头条");
    }
    virtual void tearDown() override {}
    
    bool starts_with(const string& s1, const string& s2) {
        
        return s2.size() <= s1.size() && s1.compare(0, s2.size(), s2) == 0;
        
    }
    
    bool VisitObjCIvarDecl(ObjCIvarDecl *node)
    {
        string name = node->getNameAsString();
        string::iterator itor = name.begin();
        if(*itor != '_'){
            addViolation(node, this, "成员变量 "+name+" 应该以下划线开头");
        }
        return true;
    }

    bool VisitObjCPropertyDecl(ObjCPropertyDecl *node)
    {
        if (!node->getTypeSourceInfo()) {
            return false;
        }
        
        ObjCPropertyDecl::PropertyAttributeKind attributeKind = node->getPropertyAttributes();
        if (node->isAtomic()) {
            string description = "不应该用 atomic 修饰 " + node->getNameAsString() + " 请改用 nonatomic";
            addViolation(node, this, description);
        }
        
        string typeStr = node->getType().getAsString();
        
        if(typeStr.find("<")!=string::npos && typeStr.find(">")!=string::npos && starts_with(typeStr, "id")){
            if(!(attributeKind & ObjCPropertyDecl::OBJC_PR_weak)){
                addViolation(node, this, node->getNameAsString() + " 尽可能用 weak 修饰");
            }
        }
        
        if (node->getType()->isBlockPointerType()) { // 此处避免block中也有NSString参数类型导致的误报
            if(!((attributeKind & ObjCPropertyDecl::OBJC_PR_copy) || (attributeKind & ObjCPropertyDecl::OBJC_PR_strong))){
                addViolation(node, this, node->getNameAsString() + " 应该用 copy / strong 修饰");
            }
        } else {
            if(starts_with(typeStr, "NSString") && !(attributeKind & ObjCPropertyDecl::OBJC_PR_copy)){
                addViolation(node, this, typeStr + node->getNameAsString() + " 应尽量用 copy 修饰");
            }
        }
        
        string name = node->getNameAsString();
        string::iterator itor = name.begin();
        if(*itor >= 'A' && *itor <= 'Z'){
            addViolation(node, this, name+" 应该遵循驼峰命名，不应该以大写字母开头");
        }
        if(*itor == '_'){
            addViolation(node, this, name+" 应该遵循驼峰命名，不应该以下划线开头");
        }
        
        return true;
    }
    

};

static RuleSet rules(new QTTPropertyRule());
