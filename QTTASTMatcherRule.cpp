#include "oclint/AbstractASTMatcherRule.h"
#include "oclint/RuleSet.h"
#include <iostream>
using namespace std;
using namespace clang;
using namespace clang::ast_matchers;
using namespace oclint;

class QTTASTMatcherRule : public AbstractASTMatcherRule
{
public:
    virtual const string name() const override
    {
        return "q t t a s t matcher";
    }

    virtual int priority() const override
    {
        return 3;
    }

    virtual const string category() const override
    {
        return "custom";
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

    virtual void callback(const MatchFinder::MatchResult &result) override
    {
        const EnumDecl *enumDecl = result.Nodes.getNodeAs<EnumDecl>("enumDecl");
        if (enumDecl)
        {
            addViolation(node, this, "枚举尽量使用NS_ENUM/NS_OPTIONS");
        }
    }

    virtual void setUpMatcher() override
    {
        addMatcher(enumDecl().bind("enumDecl"));
    }

};

static RuleSet rules(new QTTASTMatcherRule());
