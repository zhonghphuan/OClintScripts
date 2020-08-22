#include "oclint/AbstractASTVisitorRule.h"
#include "oclint/RuleSet.h"
#include "oclint/RuleConfiguration.h"
#include <iostream>
using namespace std;
using namespace clang;
using namespace oclint;

class QTTBlankStyleRule : public AbstractASTVisitorRule<QTTBlankStyleRule>
{
private:
    string _teamname;
public:
    virtual const string name() const override
    {
        return "空格规范";
    }

    virtual int priority() const override
    {
        return 3;
    }

    virtual const string category() const override
    {
        return _teamname + "空格";
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
         _teamname = RuleConfiguration::stringForKey("TEAM_NAME", "趣头条");
    }
    virtual void tearDown() override {}

    
    bool VisitObjCPropertyDecl(ObjCPropertyDecl *node)
    {
        string methodDeclStr;
        ASTContext *context = _carrier->getASTContext();
        SourceLocation begin = node->getSourceRange().getBegin();
        SourceLocation end = node->getSourceRange().getEnd(); methodDeclStr.assign(context->getSourceManager().getCharacterData(begin),end.getRawEncoding()-begin.getRawEncoding());

        for(string::iterator it = methodDeclStr.begin(); it!= methodDeclStr.end(); it++)
        {
            if (*it == '(') {
                if (it-2 >= methodDeclStr.begin()) {
                    if (!(*(it-1) == ' ' && *(it-2) == 'y') && *(it+1) != '^') {
                        addViolation(node, this, node->getNameAsString() + "@property左括号请遵循空格规范");
                        return true;
                    }
                }
            }
            
            if (*it == ',') {
                if (it+1 < methodDeclStr.end()) {
                    if (*(it+1) != ' ') {
                        addViolation(node, this, node->getNameAsString() + "@property中逗号请遵循空格规范");
                        return true;
                    }
                }
            }
            
            if (*it == ')') {
                if (it+1 < methodDeclStr.end()) {
                    if (*(it+1) != ' ') {
                        addViolation(node, this, node->getNameAsString() + "@property右括号请遵循空格规范");
                        return true;
                    }
                }
            }
            
            if (*it == '*') {
                if (it-1 >= methodDeclStr.begin()) {
                    if (*(it-1) != ' ') {
                        addViolation(node, this, node->getNameAsString() + "@property属性*修饰请遵循空格规范");
                        return true;
                    }
                }
            }
        }
        return true;
    }
    
    bool VisitObjCMethodDecl(ObjCMethodDecl *node)
    {
        string methodDeclStr;
        ASTContext *context = _carrier->getASTContext();
        SourceLocation begin = node->getSourceRange().getBegin();
        SourceLocation end = node->getSourceRange().getEnd(); methodDeclStr.assign(context->getSourceManager().getCharacterData(begin),end.getRawEncoding()-begin.getRawEncoding());
        for(string::iterator it = methodDeclStr.begin(); it!= methodDeclStr.end(); it++)
        {
            if (*it =='-' || *it =='+') {
                if (it+2 < methodDeclStr.end()) {
                    if (!((*(it+1) == ' ')&&(*(it+2) == '('))) {
                        addViolation(node, this, "方法前面-或+请遵循空格规范");
                        return true;
                    }
                }
            }
            
            if (*it =='{') {
                if (it-1 >= methodDeclStr.begin()) {
                    if (*(it-1) != ' ' && *(it-1) != '\n') {
                        addViolation(node, this, "方法大括号请遵循空格规范");
                    }
                }
                return true;
            }
        }
        return true;
    }
};

static RuleSet rules(new QTTBlankStyleRule());
