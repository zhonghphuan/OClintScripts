#include "oclint/AbstractASTVisitorRule.h"
#include "oclint/RuleSet.h"
#include "oclint/RuleConfiguration.h"
#include <iostream>
using namespace std;
using namespace clang;
using namespace oclint;

class QTTClassNameRule : public AbstractASTVisitorRule<QTTClassNameRule>
{
private:
    string _threshold;
public:
    virtual const string name() const override
    {
        return "类名规范";
    }

    virtual int priority() const override
    {
        return 3;
    }

    virtual const string category() const override
    {
        return _threshold + "类名";
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

 
    bool VisitObjCProtocolDecl(ObjCProtocolDecl *node)
    {
        string name = node->getNameAsString();
        string::iterator itor = name.begin();
        if(!(*itor >= 'A' && *itor <= 'Z')){
            addViolation(node, this, name+"协议名请用大写字母开头");
        }
        return true;
    }

  
    bool VisitObjCImplDecl(ObjCImplDecl *node)
    {
        string name = node->getNameAsString();
        string::iterator itor = name.begin();
        if(!(*itor >= 'A' && *itor <= 'Z')){
            addViolation(node, this, name+"类名请用大写字母开头");
        }
        return true;
    }



    

};

static RuleSet rules(new QTTClassNameRule());
