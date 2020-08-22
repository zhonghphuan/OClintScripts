#include "oclint/AbstractASTVisitorRule.h"
#include "oclint/RuleSet.h"
#include "oclint/RuleConfiguration.h"
#include <iostream>
using namespace std;
using namespace clang;
using namespace oclint;

class QTTEnumRule : public AbstractASTVisitorRule<QTTEnumRule>
{
private:
    string _teamname;
    
public:
    virtual const string name() const override
    {
        return "枚举规范";
    }

    virtual int priority() const override
    {
        return 3;
    }

    virtual const string category() const override
    {
        return _teamname + "枚举";
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

    
#endif

    virtual void setUp() override {
        _teamname = RuleConfiguration::stringForKey("TEAM_NAME", "趣头条");
    }
    virtual void tearDown() override {}
    
    bool VisitEnumDecl(EnumDecl *node)
    {
        addViolation(node, this, node->getNameAsString()+"枚举尽量使用NS_ENUM/NS_OPTIONS");
        return true;
    }
    
    bool VisitEnumConstantDecl(EnumConstantDecl *node)
    {
        string name = node->getNameAsString();
        string::iterator itor = name.begin();
        if(*itor == '_'){
            addViolation(node, this, "枚举不要下划线开头");
        }
        return true;
    }
};


static RuleSet rules(new QTTEnumRule());
